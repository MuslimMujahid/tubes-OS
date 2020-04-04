#include "folderIO3.h"

int findIndex(char* name, char parentIndex, int isDir, int isFile)
{
    int i;

    // Load files
    char files[SECTOR_SIZE * 2];
    interrupt(0x21, 0x02, files, FILES_SECTOR_1);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, FILES_SECTOR_2);
    for (i = 0; i < SECTOR_SIZE*2; i += FILES_COLUMNS)
    {
        if (files[i + NAME_OFFSET] != EMPTY && files[i] == parentIndex)
        {
            if ((isDir && files[i+1] == DIR) || (isFile && files[i+1] != DIR))
            {
                if (strCmpMax(name, files + i + NAME_OFFSET, MAX_FILENAME_LENGTH))
                {
                    return (i >> 0x4);
                }
            }
        }
    }
    return FILE_NOT_FOUND;
}

int pathFindIndex(char* path, char parentIndex, int isDir, int isFile)
{
    int i, j;
    char name[MAX_FILENAME_LENGTH];
    i = 0;
    j = 0;
    while (path[i] != '\0')
    {
        if (path[i] == '/')
        {
            j = 0;
            parentIndex = findIndex(name, parentIndex, TRUE, FALSE);

            if (parentIndex == FILE_NOT_FOUND)
            {
                return FILE_NOT_FOUND;
            }

            clear(name, MAX_FILENAME_LENGTH);
        }
        else
        {
            name[j] = path[i];
            j++;
        }
        i++;
    }
    return findIndex(name, parentIndex, isDir, isFile);
}