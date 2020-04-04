#include "../macro.h"

main()
{
    char parentIndex;
    char path[MAX_FILENAME_LENGTH];
    char buffer[FILE_SIZE];
    int index;

    /* clear buffer */
    clear(buffer, FILE_SIZE);

    /* get current index and first command argument*/
    getCurDir(&parentIndex);
    getArgv(0, &path);

    parentIndex = pathFindIndex(path, parentIndex); 
    /* read file */
    if (!isFileExist(path, parentIndex))
    {
        pS(" There is no such file or directory!", TRUE);
        terminate();
    }
    readFile(buffer, path, 0, parentIndex);

    /* Display file content to the screen */
    pS(buffer, TRUE);
    terminate();
}