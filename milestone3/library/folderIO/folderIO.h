#ifndef FOLDERIO_H
#define FOLDERIO_H

#include "../../macro.h"

int isDirExist(char* dirname, char curDirIndex);
char getDirIndexByName(char* dirname, char curDirIndex);
char getParentIndexByCurIndex(char curDirIndex);
int findFilenameInDir(char* path, char parentIndex);

#endif