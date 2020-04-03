#ifndef FOLDERIO2_H
#define FOLDERIO2_H

#include "../../macro.h"

int isDirExist(char* dirname, char curDirIndex);
char getDirIndexByName(char* dirname, char curDirIndex);
char getParentIndexByCurIndex(char curDirIndex);
int findFilenameInDir(char* path, char parentIndex);
int findIndex(char* name, char parentIndex, int isDir, int isFile);
int pathFindIndex(char* path, char parentIndex, int isDir, int isFile);

#endif