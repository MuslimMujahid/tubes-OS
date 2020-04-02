#include "dir.h"

void getCurDir(char* curdir)
{
    char args[SECTOR_SIZE];

    interrupt(0x21, 0x02, args, ARGS_SECTOR, SECTOR_SIZE);
    *curdir = args[0]; 
}