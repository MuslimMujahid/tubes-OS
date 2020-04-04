#ifndef FILEIO_H
#define FILEIO_H

#include "../../macro.h"

void writeFile(char *buffer, char *path, int *sector, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void deleteFile(char *filename, char parentIndex);
void moveFile(char* filename, char parentIndex, char newParentIndex);
void copyFile(char* filename, char parentIndex, char newParentIndex);
int isFileExist(char* dirname, char curDirIndex);

#endif