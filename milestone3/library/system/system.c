#include "system.h"

void putArgs(char curDirIndex, char argc, char* argv)
{
    char args[SECTOR_SIZE];
    int i;

    clear(args, SECTOR_SIZE);
    args[0] = curDirIndex;
    args[1] = argc;

    for (i = 0; i < ARGV_LENGTH; i++)
    {
        args[i+2] = argv[i];
    }
    interrupt(0x21, 0x03, args, ARGS_SECTOR, 0);
}

void getArgc(char* argc)
{
    char args[SECTOR_SIZE];

    interrupt(0x21, 0x02, args, ARGS_SECTOR, SECTOR_SIZE);
    *argc = args[1]; 
}

void getArgv(int index, char* argv)
{
    char args[SECTOR_SIZE];
    int i, j, k;

    interrupt(0x21, 0x02, args, ARGS_SECTOR, SECTOR_SIZE);

    i = 0;
    j = 0;
    for (k = 2; k < SECTOR_SIZE && args[k] != '\0'; k++)
    {
        if (i == index)
        {
            argv[j] = args[k];
            j++;

            if (args[k+1] == ' ') break;
        }
        else if (args[k] == ' ')
        {
            i++;
        }
    } 
}

void getCurDir(char* curdir)
{
    char args[SECTOR_SIZE];

    interrupt(0x21, 0x02, args, ARGS_SECTOR, SECTOR_SIZE);
    *curdir = args[0]; 
}

void clearArgs()
{
    char args[SECTOR_SIZE];

    clear(args, SECTOR_SIZE);
    interrupt(0x21, 0x02, args, ARGS_SECTOR, 0);
    clear(args + 1, SECTOR_SIZE-1);
    interrupt(0x21, 0x03, args, ARGS_SECTOR, 0);
}

void terminate()
{
    clearArgs();
    interrupt(0x21, 0xFF << 8 | 0x6, "shell", 0x2000, 0);
}