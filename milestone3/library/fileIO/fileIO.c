#include "fileIO.h"

void writeFile(char *buffer, char *path, int *sector, char parentIndex)
{
    int i, j;
    char map[SECTOR_SIZE];
    char files[SECTOR_SIZE * 2];
    char sectors[SECTOR_SIZE];
    int fileIndex;
    int sectorindex;
    int empty_sector;
    int sectorId, sectorCount;
    char tmp_buff[SECTOR_SIZE];

    // Read sector map, files, and sectors
    interrupt(0x21, 0x02, map, MAP_SECTOR, 0);
    interrupt(0x21, 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
    interrupt(0x21, 0x02, sectors, SECTORS_SECTOR, 0);

    // Find file in parentIndex
    fileIndex = findFilenameInDir(path, parentIndex);
    if (fileIndex != FILE_NOT_FOUND) // file already exist
    {
        *sector = FILE_EXIST;
        return;
    }

    // Check if there enough space to store file
    for (i = 0, empty_sector = 0; i < MAX_BYTE && empty_sector < *sector; i++)
    {
        if (map[i] != USED)
            empty_sector++;
    }
    if (i == MAX_BYTE)
    {
        *sector = SECTORS_FULL;
        return;
    }

    // find a free entry in the files
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
        if (files[i + NAME_OFFSET] == EMPTY) break;
    if (i == SECTOR_SIZE * 2)
    {
        *sector = FILES_FULL;
        return;
    }
    fileIndex = i;

    // find a free sectors entry
    for (i = 0; i < SECTOR_SIZE; i += SECTORS_COLUMNS)
        if (sectors[i] == EMPTY) break;
    if (i == SECTOR_SIZE)
    {
        *sector = SECTORS_FULL;
        return;
    }
    sectorindex = i;

    // Put files in the parentIndex
    files[fileIndex] = parentIndex;

    // Point the files to the sector
    // if its a directory then set S to 0xFF
    if (buffer == 0)
    {
        files[fileIndex + 1] = DIR;
    }
    else
    {
        files[fileIndex + 1] = sectorindex >> 0x4;
    }
    

    // Clear memory that will be used to store filename
    clear(files + fileIndex + NAME_OFFSET, MAX_FILENAME_LENGTH);

    // Store filename
    i = 0, j = 0;
    while (path[i] != '\0' && i < MAX_FILENAME_LENGTH) i++;
    while (path[i] != '/' && i > 0) i--;
    if (path[i] == '/') i += 1;
    while (path[i] != '\0' && j < MAX_FILENAME_LENGTH)
        files[fileIndex + NAME_OFFSET + j++] = path[i++];

    // Store file 
    for (sectorId = 0, sectorCount = 0; sectorId < MAX_BYTE && sectorCount < *sector; sectorId++)
    {
        if (map[sectorId] == EMPTY)
        {
            // Mark sector as used
            map[sectorId] = USED;

            // Put sectorId to sectors correspondent to files
            sectors[sectorindex + sectorCount] = sectorId;
        
            // clear temp buffer
            clear(tmp_buff, SECTOR_SIZE);

            // Write to the empty sector
            for (i = 0; i < SECTOR_SIZE; i++)
                tmp_buff[i] = buffer[sectorCount * SECTOR_SIZE + i];

            interrupt(0x21, 0x03, tmp_buff, sectorId, 0);

            sectorCount++;
        }
    }

    // Write to system
    interrupt(0x21, 0x03, map, MAP_SECTOR, 0);
    interrupt(0x21, 0x03, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x03, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
    interrupt(0x21, 0x03, sectors, SECTORS_SECTOR, 0);

    *sector = 1;
}

void readFile(char *buffer, char *path, int *result, char parentIndex)
{
    int i;
    char map[SECTOR_SIZE], files[SECTOR_SIZE * 2], sectors[SECTOR_SIZE];
    int fileIndex;
    int sectorIndex;    

    clear(map, SECTOR_SIZE);
    clear(files, SECTOR_SIZE*2);
    clear(sectors, SECTOR_SIZE);

    // Read sector map, files, and sectors
    interrupt(0x21, 0x02, map, MAP_SECTOR, 0);
    interrupt(0x21, 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
    interrupt(0x21, 0x02, sectors, SECTORS_SECTOR, 0);

    // Find filename in parentIndex
    fileIndex = findFilenameInDir(path, parentIndex);
    if (fileIndex == FILE_NOT_FOUND) // file not found
    {
        *result = FILE_NOT_FOUND;
        return;
    }

    // Load file to buffer
    sectorIndex = files[fileIndex + 1] * SECTORS_COLUMNS;
    for (i = 0; i < FILES_COLUMNS && sectors[sectorIndex + i] != EMPTY; i++)
    {
        interrupt(0x21, 0x02, buffer + i*SECTOR_SIZE, sectors[sectorIndex + i], 0);
    }
    *result = 1;
}

void deleteFile(char *buffer, char *path, int*result, char parentIndex)
{
    int i;
    char map[SECTOR_SIZE], files[SECTOR_SIZE * 2], sectors[SECTOR_SIZE];
    char emptySector[SECTOR_SIZE];
    int fileIndex;
    int sectorIndex; 

    clear(map, SECTOR_SIZE);
    clear(files, SECTOR_SIZE*2);
    clear(sectors, SECTOR_SIZE);
    clear(emptySector, SECTOR_SIZE);

    // Read sector
    interrupt(0x21, 0x02, map, MAP_SECTOR, 0);
    interrupt(0x21, 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
    interrupt(0x21, 0x02, sectors, SECTORS_SECTOR, 0);

    // Find filename in parentIndex
    fileIndex = findFilenameInDir(path, parentIndex);
    if (fileIndex == FILE_NOT_FOUND) // file not found
    {
        *result = FILE_NOT_FOUND;
        return;
    }

    // Delete file information from map, files, and sectors 
    sectorIndex = files[fileIndex + 1] * SECTORS_COLUMNS;
    for (i = 0; i < FILES_COLUMNS && sectors[sectorIndex + i] != EMPTY; i++)
    {
        interrupt(0x21, 0x03, emptySector, sectors[sectorIndex + i], 0);
        map[sectors[sectorIndex + i]] = EMPTY;
        sectors[sectorIndex + i] = EMPTY;
    }
    clear(files + fileIndex, FILES_COLUMNS);

    // Write to system
    interrupt(0x21, 0x03, map, MAP_SECTOR, 0);
    interrupt(0x21, 0x03, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x03, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
    interrupt(0x21, 0x03, sectors, SECTORS_SECTOR, 0);

    *result = 1;
}

int isFileExist(char* dirname, char curDirIndex)
{
    int i, j, filefound;
    char files[SECTOR_SIZE * 2];

    // Load files
    interrupt(0x21, (0 << 8) | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, (0 << 8) | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    // Find filename in parentIndex
    filefound = 0;
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i] == curDirIndex && files[i + 1] != DIR) // Check only dir
        {
            filefound = 1;
            j = 0;
            while (dirname[j] != '\0')
            {
                if (dirname[j] != files[i + NAME_OFFSET + j])
                {
                    filefound = 0;
                    break;
                }
                j++;
            }
            if (filefound) return TRUE;
        }
    }
    return FALSE;
}

