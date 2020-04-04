#ifndef FOLDERIO_H
#define FOLDERIO_H

#include "../../macro.h"

void createrDir(char* dirname, char parentIndex);
void deleteDir(char* dirname, char parentIndex);
void deleteAllinDir(char* dirname, char parentIndex);
void copyDir(char* dirname, char parentIndex, char newParentIndex);
void copyAllInDir(char* dirname, char parentIndex, char newParentIndex);
void getListContent(char* listContent, char parentIndex);

#endif