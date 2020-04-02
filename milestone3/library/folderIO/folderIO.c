#include "folderIO.h"

int isDirExist(char* dirname, char curDirIndex)
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
        
        
        if (files[i] == curDirIndex && files[i + 1] == DIR) // Check only dir
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

char getDirIndexByName(char* dirname, char curDirIndex)
{
    int i, j, filefound;
    char files[SECTOR_SIZE * 2];

    // Load files
    interrupt(0x21, (0 << 8) | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, (0 << 8) | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i] == curDirIndex && files[i + 1] == DIR)
        {
            if (strCmp(files + i + NAME_OFFSET, dirname))
            {
                return (i >> 0x4);
            }
        }
    }
}

char getParentIndexByCurIndex(char curDirIndex)
{
    char files[SECTOR_SIZE * 2];

    // Load files
    interrupt(0x21, (0 << 8) | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, (0 << 8) | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    return files[curDirIndex * FILES_COLUMNS];
}

int findFilenameInDir(char* path, char parentIndex)
{
    int i, j;
    int filenameIndex;
    int filefound;

    // Load files
    char files[SECTOR_SIZE * 2];
    interrupt(0x21, 0x02, files, FILES_SECTOR_1);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, FILES_SECTOR_2);

    // Find filename in path
    i = 0;
    while (path[i] != '\0' && i < MAX_FILENAME_LENGTH) i++;
    while (path[i] != '/' && i > 0) i--;
    if (path[i] == '/') path += i + 1; 

    // Find filename in parentIndex
    filefound = 0;
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i] == parentIndex && files[i + 1] != DIR) // Check only files, not directory
        {
            filefound = 1;
            j = 0;
            while (path[j] != '\0')
            {
                if (path[j] != files[i + NAME_OFFSET + j])
                {
                    filefound = 0;
                    break;
                }
                j++;
            }
            if (filefound) return i;
        }
    }
    return FILE_NOT_FOUND;
}