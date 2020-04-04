#include "../macro.h"

main()
{
    int i, index;
    char oriPath[128];
    char destPath[128];
    char parentIndex, newParentIndex;

    // clear(buffer, FILE_SIZE);
    clear(oriPath, 128);
    clear(destPath, 128);

    getCurDir(&parentIndex);
    getArgv(0, &oriPath);
    getArgv(1, &destPath);

    pI(parentIndex, TRUE);

    getCurDir(&parentIndex);
    index = pathFindIndex(oriPath, parentIndex, TRUE, TRUE);
    parentIndex = getParentIndexByCurIndex(index);
    newParentIndex = pathFindIndex(destPath, parentIndex, TRUE, TRUE);
    
    /* Add filename to destPath */
    i = len(oriPath)-1;
    while (oriPath[i] != '/' && i > 0) i--;
    if (oriPath[i] == '/') i++;

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