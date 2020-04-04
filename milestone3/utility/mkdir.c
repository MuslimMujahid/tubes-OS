#include "../macro.h"

main()
{
    char parentIndex;
    char dirname[MAX_FILENAME_LENGTH];

    clear(dirname, MAX_FILENAME_LENGTH);

    getCurDir(&parentIndex);
    getArgv(0, &dirname);

    if (isDirExist(dirname, parentIndex))
    {
        pS(" There is already directory with that name!", TRUE);
        terminate();
    }
    
    writeFile(0, dirname, 0, parentIndex);
    terminate();
}