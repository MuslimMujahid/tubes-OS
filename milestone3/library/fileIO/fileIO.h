#ifndef FILEIO_H
#define FILEIO_H

#include "../../macro.h"

void writeFile(char *buffer, char *path, int *sector, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void deleteFile(char *buffer, char *path, int *result, char parentIndex);
int isFileExist(char* dirname, char curDirIndex);

#endif