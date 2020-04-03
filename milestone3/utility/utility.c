#include "utility.h"

void _mkdir_()
{
    char parentIndex;
    char dirname[MAX_FILENAME_LENGTH];

    /* Clear data */
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