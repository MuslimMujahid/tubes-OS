#include "../macro.h"

main()
{
    int i;
    char buffer[FILE_SIZE];
    char oriPath[128];
    char destPath[128];
    char parentIndex, newParentIndex, index;

    /* Get arguments and current directory */
    getCurDir(&parentIndex);
    getArgv(0, &oriPath);
    getArgv(1, &destPath);

    /* find file/directory name in path */
    i = len(oriPath)-1;
    while (oriPath[i] != '/' && i > 0) i--;
    if (oriPath[i] == '/') i++;
    
    /* find index and parent index of file/directory */
    index = findIndex(oriPath + i, parentIndex);
    newParentIndex = findIndex(destPath, parentIndex, TRUE, FALSE);
    parentIndex = getParentIndexByCurIndex(index);

    if (isFileExist(oriPath + i, parentIndex))
    {
        moveFile(oriPath + i, parentIndex, newParentIndex);
    }
    else if (isDirExist(oriPath + i, parentIndex))
    {
        moveDir(oriPath + i, parentIndex, newParentIndex);
    }
    else
    {
        pS("There is no such file or directory.", TRUE);
    }

    terminate();
}