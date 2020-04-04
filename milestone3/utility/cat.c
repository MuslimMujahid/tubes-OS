#include "../macro.h"

main()
{
    char parentIndex;
    char filename[MAX_FILENAME_LENGTH];
    char buffer[FILE_SIZE];

    /* clear buffer */
    clear(buffer, FILE_SIZE);

    /* get current index and first command argument*/
    getCurDir(&parentIndex);
    getArgv(0, &filename);

    /* read file */
    if (!isFileExist(filename, parentIndex))
    {
        pS(" There is no such file or directory!", TRUE);
        terminate();
    }
    readFile(buffer, filename, 0, parentIndex);

    /* Display file content to the screen */
    pS(buffer, TRUE);
    terminate();
}