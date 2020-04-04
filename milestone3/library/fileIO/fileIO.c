#include "fileIO.h"

void writeFile(char *buffer, char *path, int *sector, char parentIndex)
{
    interrupt(0x21, (parentIndex << 8) | 0x05, buffer, path, sector);
}

void readFile(char *buffer, char *path, int *result, char parentIndex)
{
    interrupt(0x21, (parentIndex << 8) | 0x04, buffer, path, result);
}

void deleteFile(char *filename, char parentIndex)
{
    int i;
    char map[SECTOR_SIZE];
    char files[SECTOR_SIZE * 2];
    char sectors[SECTOR_SIZE];
    int fileIndex;
    int sectorindex;

    // Read sector map, files, and sectors
    interrupt(0x21, 0x02, map, MAP_SECTOR, 0);
    interrupt(0x21, 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
    interrupt(0x21, 0x02, sectors, SECTORS_SECTOR, 0);

    fileIndex = findIndex(filename, parentIndex, FALSE, TRUE) * FILES_COLUMNS;
    sectorindex = files[fileIndex + 1] * SECTORS_COLUMNS;
    
    i = 0;
    while (sectors[sectorindex + i] != EMPTY)
    {
        map[sectors[sectorindex + i]] = EMPTY;
        sectors[sectorindex + i] = EMPTY;
        i++;
    }
    clear(files + fileIndex, FILES_COLUMNS);

    // Write to system
    interrupt(0x21, 0x03, map, MAP_SECTOR, 0);
    interrupt(0x21, 0x03, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x03, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
    interrupt(0x21, 0x03, sectors, SECTORS_SECTOR, 0);
}

void moveFile(char* filename, char parentIndex, char newParentIndex)
{
    char files[SECTOR_SIZE * 2];
    int fileIndex;

    // Read sector map, files, and sectors
    interrupt(0x21, 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    fileIndex = findIndex(filename, parentIndex, FALSE, TRUE) * FILES_COLUMNS;
    files[fileIndex] = newParentIndex;

    // Write to system
    interrupt(0x21, 0x03, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x03, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
}

void copyFile(char* filename, char parentIndex, char newParentIndex)
{
    char buffer[FILE_SIZE];
    int sector;

    clear(buffer, FILE_SIZE);

    readFile(buffer, filename, 0, parentIndex);

    sector = len(buffer) >> 0x8;
    if (mod(len(buffer), SECTOR_SIZE) > 0) sector++;

    writeFile(buffer, filename, sector, newParentIndex);
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

