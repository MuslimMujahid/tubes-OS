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

void _cp_()
{
    int i, index;
    char buffer[FILE_SIZE];
    char oriPath[128];
    char destPath[128];
    char parentIndex;
    char result;

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
    writeFile(buffer, destPath, (len(buffer) >> 0x9) + 1, parentIndex);
}
void _rm_()
{
    char parentIndex;
    char dirname[MAX_FILENAME_LENGTH];
    char option[2];
    clear(dirname, MAX_FILENAME_LENGTH);

    getCurDir(&parentIndex);
    getArgv(0, &dirname);
    pI(parentIndex, TRUE);
    pS(dirname, TRUE);
    if (!isDirExist(dirname, parentIndex))
    {
        pS(" There is no directory with that name!", TRUE);
        return;
    }
    deleteFile(0, dirname, 0, parentIndex);
    pS("selesai", TRUE);
    terminate();
}