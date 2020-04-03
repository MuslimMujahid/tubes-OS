#include "../macro.h"

main()
{
    char parentIndex;
    char dirname[MAX_FILENAME_LENGTH];

    clear(dirname, MAX_FILENAME_LENGTH);

    getCurDir(&parentIndex);
    getArgv(0, &dirname);
    pI(parentIndex, TRUE);
    pS(dirname, TRUE);
    if (isDirExist(dirname, parentIndex))
    {
        pS(" There is already directory with that name!", TRUE);
        return;
    }
    writeFile(0, dirname, 0, parentIndex);
    pS("selesai", TRUE);
    terminate();
}