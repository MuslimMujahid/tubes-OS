#include "../macro.h"

main()
{
    char parentIndex;
    char path[MAX_FILENAME_LENGTH];
    char buffer[FILE_SIZE];
    int index, i;

    /* clear buffer */
    clear(buffer, FILE_SIZE);
    /* get current index and first command argument*/
    getCurDir(&parentIndex);
    getArgv(0, &path);

    i = len(path)-1;
    while(path[i] != '/' && i > 0) i--;
    if (path[i] == '/') i++;

    index = pathFindIndex(path, parentIndex, FALSE, TRUE); 
    parentIndex = getParentIndexByCurIndex(index);
    pS("selesai", TRUE);

    /* read file */
    if (!isFileExist(path + i, parentIndex))
    {
        pS(" There is no such file or directory!", TRUE);
        terminate();
    }
    readFile(buffer, path + i, 0, parentIndex);

    /* Display file content to the screen */
    pS(buffer, TRUE);
    
    terminate();
}