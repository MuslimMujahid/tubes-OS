#include "../macro.h"

main()
{
    int i;
    char parentIndex, index;
    char path[128];

    /* Get arguments and current directory */
    getCurDir(&parentIndex);
    getArgv(0, &path);

    /* find file/directory name in path */
    i = len(path)-1;
    while (path[i] != '/' && i > 0) i--;
    if (path[i] == '/') i++;
    
    /* find index and parent index of file/directory */
    index = findIndex(path + i, parentIndex);
    parentIndex = getParentIndexByCurIndex(index);

    /* Delete file/directory */
    if (isDirExist(path + i, parentIndex))
    {
        deleteAllinDir(path + i, parentIndex);
        deleteDir(path + i, parentIndex);
    }
    else if (isFileExist(path + i, parentIndex))
    {
        deleteFile(path + i, parentIndex);
    }

    terminate();
}