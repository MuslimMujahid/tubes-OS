#include "../macro.h"

main()
{
    int i, index;
    char buffer[FILE_SIZE];
    char oriPath[128];
    char destPath[128];
    char parentIndex;
    char result;
    int sector;

    pS("Masuk", TRUE);

    clear(buffer, FILE_SIZE);
    clear(oriPath, 128);
    clear(destPath, 128);

    getCurDir(&parentIndex);
    getArgv(0, &oriPath);
    getArgv(1, &destPath);

    pS(oriPath, TRUE);
    pS(destPath, TRUE);
    pI(parentIndex, TRUE);

    readFile(buffer, oriPath, &result, parentIndex);

    getCurDir(&parentIndex);
    parentIndex = pathFindIndex(destPath, parentIndex, TRUE, TRUE);
    
    /* Add filename to destPath */
    i = len(oriPath)-1;
    while (oriPath[i] != '/' && i > 0) i--;
    if (oriPath[i] == '/') i++;
    destPath[len(destPath)] = '/';
    concat(destPath, oriPath + i);
    
    /* Write file to destpath */
    pS(buffer, TRUE);
    pS(destPath, TRUE);
    pI((len(buffer) >> 0x9) + 1, TRUE);
    pI(parentIndex, TRUE);
    sector = (len(buffer) >> 0x9) + 1;
    writeFile(buffer, destPath, &sector, parentIndex);

    terminate();
}