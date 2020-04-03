#ifndef FOLDERIO2_H
#define FOLDERIO2_H

#include "../../macro.h"

int findIndex(char* name, char parentIndex, int isDir, int isFile);
int pathFindIndex(char* path, char parentIndex, int isDir, int isFile);

#endif