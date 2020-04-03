#include "folderIO.h"

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

void createrDir(char* dirname, char parentIndex)
{
    interrupt(0x21, (parentIndex << 8) | 0x5, 0, dirname, 0);
}

void deleteDir(char* dirname, char parentIndex)
{
    interrupt(0x21, (parentIndex << 8) | 0x5, 0, dirname, 0);
}