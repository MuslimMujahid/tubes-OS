#include "utility.h"

void _cat_()
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
        return;
    }
    readFile(buffer, filename, 0, parentIndex);

    /* Display file content to the screen */
    pS(buffer, TRUE);
}

void _mkdir_()
{
    char parentIndex;
    char dirname[MAX_FILENAME_LENGTH];

    /* Clear buffer */
    clear(dirname, MAX_FILENAME_LENGTH);

    /* get current index and first command argument*/
    getCurDir(&parentIndex);
    getArgv(0, &dirname);

    /* Make directory */
    if (isDirExist(dirname, parentIndex))
    {
        pS(" There is already directory with that name!", TRUE);
        return;
    }
    writeFile(0, dirname, 0, parentIndex);
}