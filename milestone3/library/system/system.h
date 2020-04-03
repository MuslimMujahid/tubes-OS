#ifndef SYSTEM_H
#define SYSTEM_H

#include "../../macro.h"

void terminate();
void putArgs(char curDirIndex, char argc, char* argv);
void getArgc(char* argc);   
void getArgv(int index, char* argv);
void getCurDir(char* curdir);

#endif