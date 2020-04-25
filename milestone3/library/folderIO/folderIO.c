#include "folderIO.h"

void createDir(char* dirname, char parentIndex)
{
    interrupt(0x21, (parentIndex << 8) | 0x5, 0, dirname, 0);
}

void deleteDir(char* dirname, char parentIndex)
{
    char files[SECTOR_SIZE * 2];
    int dirIndex;

    // Read sector map, files, and sectors
    interrupt(0x21, 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    dirIndex = findIndex(dirname, parentIndex, TRUE, FALSE) * FILES_COLUMNS;
    clear(files + dirIndex, FILES_COLUMNS);

    // Write to system
    interrupt(0x21, 0x03, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x03, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
}

void deleteAllinDir(char* dirname, char parentIndex)
{
    int i, index;
    char files[SECTOR_SIZE * 2];
    char name[MAX_FILENAME_LENGTH];

    clear(files, SECTOR_SIZE * 2);

    // Read sector map, files, and sectors
    interrupt(0x21, 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    index = findIndex(dirname, parentIndex, TRUE, FALSE);
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i + NAME_OFFSET] != EMPTY)
        {
            if (files[i] == index)
            {
                copyMax(files + i + NAME_OFFSET, name, MAX_FILENAME_LENGTH);
                if (files[i] == DIR)
                {
                    deleteAllinDir(name, index);
                    deleteDir(name, index);
                }
                else
                {
                    deleteFile(name, index);
                }
            }
        }
    }
}

void copyDir(char* dirname, char parentIndex, char newParentIndex)
{
    int index;
    char files[SECTOR_SIZE * 2];

    createDir(dirname, newParentIndex);

    clear(files, SECTOR_SIZE * 2);
    interrupt(0x21, 0x03, files, FILES_SECTOR_1, 0); 
    interrupt(0x21, 0x03, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    index = findIndex(dirname, newParentIndex);
    copyAllInDir(dirname, parentIndex, index);
}

void copyAllInDir(char* dirname, char parentIndex, char newParentIndex)
{
    int i, j, index;
    char files[SECTOR_SIZE * 2];
    char name;

    clear(files, SECTOR_SIZE * 2);
    clear(name, MAX_FILENAME_LENGTH);

    interrupt(0x21, 0x03, files, FILES_SECTOR_1, 0); 
    interrupt(0x21, 0x03, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    index = findIndex(dirname, parentIndex);
    for (i = 0; i < SECTOR_SIZE; i += FILES_COLUMNS)
    {
        if (files[i + NAME_OFFSET] != EMPTY)
        {
            if (files[i] == index)
            {
                if (files[i + 1] == DIR)
                {
                    copyMax(files + i + NAME_OFFSET, name, MAX_FILENAME_LENGTH);
                    copyDir(name, index, newParentIndex);
                }
            }
            else
            {
                copyFile(name, index, newParentIndex);
            }
        }
    }
}

void moveDir(char* filename, char parentIndex, char newParentIndex)
{
    char files[SECTOR_SIZE * 2];
    int fileIndex;

    // Read sector map, files, and sectors
    interrupt(0x21, 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    fileIndex = findIndex(filename, parentIndex, TRUE, FALSE) * FILES_COLUMNS;
    files[fileIndex] = newParentIndex;

    // Write to system
    interrupt(0x21, 0x03, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x03, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
}

void getListContent(char* listContent, char parentIndex)
{
    int i, j, k;
    char files[SECTOR_SIZE * 2];
    clear(files, SECTOR_SIZE * 2);
    interrupt(0x21, 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
    i = -1;
    k = 0;
    j = 0;
    while (k < SECTOR_SIZE * 2)
    {
        if (mod(k, FILES_COLUMNS) == 0) i++;
        if (files[i * FILES_COLUMNS] != parentIndex)
        {
            k += FILES_COLUMNS;
            i++;
        }
        else
        {
            if (mod(k, FILES_COLUMNS) == 0)
            {
                if (k != 0) j++;
                listContent[j] = files[k + 1];
                j++;
            }
            listContent[j] = files[k];
        }
        k++;
    }
}
