#include "folderIO.h"

int findFilenameInDir(char* path, char parentIndex)
{
    int i, j;
    int filenameIndex;
    int filefound;

    // Load files
    char files[SECTOR_SIZE * 2];
    readSector(files, FILES_SECTOR_1);
    readSector(files + SECTOR_SIZE, FILES_SECTOR_2);

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