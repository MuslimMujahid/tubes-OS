#include "../macro.h"

main()
{
    char parentIndex;
    char dirname[MAX_FILENAME_LENGTH];

    clear(dirname, MAX_FILENAME_LENGTH);

    getArgv(0, &dirname);
    if (isDirExist(dirname, parentIndex))
    {
        pS(" There is already directory with that name!", TRUE);
        return;
    }
    writeFile(parentIndex, 0, dirname, 0);
    terminate();
}
