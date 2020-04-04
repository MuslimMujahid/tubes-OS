#include "../macro.h"

main()
{
    int i, index;
    // char buffer[FILE_SIZE];
    char oriPath[128];
    char destPath[128];
    char parentIndex, newParentIndex;
    // char result;
    // int sector;

    // clear(buffer, FILE_SIZE);
    clear(oriPath, 128);
    clear(destPath, 128);

    getCurDir(&parentIndex);
    getArgv(0, &oriPath);
    getArgv(1, &destPath);

    // readFile(buffer, oriPath, &result, parentIndex);

    getCurDir(&parentIndex);
    index = pathFindIndex(oriPath, parentIndex, TRUE, TRUE);
    parentIndex = getParentIndexByCurIndex(index);
    newParentIndex = pathFindIndex(destPath, parentIndex, TRUE, TRUE);
    
    /* Add filename to destPath */
    i = len(oriPath)-1;
    while (oriPath[i] != '/' && i > 0) i--;
    if (oriPath[i] == '/') i++;
    // destPath[len(destPath)] = '/';
    // concat(destPath, oriPath + i);
    
    /* Write file to destpath */
    // sector = (len(buffer) >> 0x9);
    // if (mod(len(buffer), SECTOR_SIZE) > 0) sector++;

    // if (sector == 0)
    // {
    //     copyDir(oriPath + i, )
    // }
    // else
    // {
    //     writeFile(buffer, destPath, &sector, parentIndex);
    // }

    if (isDirExist(oriPath + i, parentIndex))
    {
        copyDir(oriPath + i, parentIndex, newParentIndex);
    }
    else if (isFileExist(oriPath + i, parentIndex))
    {
        copyFile(oriPath + i, parentIndex, newParentIndex);
    }
    else
    {
        pS("There is no such file or directory", TRUE);
    }
    

    terminate();
}