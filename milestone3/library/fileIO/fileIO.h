#ifndef FILEIO_H
#define FILEIO_H

#include "../../macro.h"

// void fileExceptionHandler(int result);
int isDirExist(char* dirname, char curDirIndex);
int isFileExist(char* dirname, char curDirIndex);
char getDirIndexByName(char* dirname, char curDirIndex);
char getParentIndexByCurIndex(char curDirIndex);

#endif