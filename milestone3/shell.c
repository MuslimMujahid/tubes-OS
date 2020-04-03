#include "macro.h"

void getCommandType(char* argc, char* type);
void commandHandler(int type, char* argc, char* argv, char* curDirIndex, char* curPath);

void addHistory(char* input, char* history);
void autocompleteFileInDir(char* input, char curDirIndex);
void autoCompletePath(char* input, char curDirIndex);
void autoCompleteDirInDir(char* input, char* dirname, char curDirIndex);

void splitArgs(char* input, char* argc, char* argv);


// command methods
void _ls_(char curDirIndex);
void _mkdir_(char* dirname, char parentIndex);
void _cd_(char* dirname, char* curDirIndex, char* curPath);
void _run_(char* filename, char curDirIndex);
void _bin_(char* filename);

main()
{
    int i;
    int tmp;
    char tmp2[10];
    int running;
    int type;
    char input[100];
    char curPath[100];
    char argc[ARGC_LENGTH];
    char argv[ARGV_LENGTH]; 
    char curDirIndex;
    char files[SECTOR_SIZE * 2];
    char history[HISTORY_LENGTH * 4];
    int curHistory;
    int inputLength;
    int ret;

    clear(curPath, 100);

    running = 1;
    curPath[0] = '$'; 
    curDirIndex = ROOT; // Begin in root
    while(running)
    {
        curHistory = 0;

        // clear input container
        clear(input, 100);
        clear(argc, ARGC_LENGTH);
        clear(argv, ARGV_LENGTH);
        clear(tmp2, 10);

        // Print current directory path
        pS(curPath, FALSE); pC(' ', FALSE);
        interrupt(0x21, (0 << 8) | 0x1, input, &ret, 0);

        // History handler
        if (ret == ARROW)
        {
            // Add input to history
            addHistory(input, history);

            if (history[1] != EMPTY)
            {
                while (TRUE)
                {
                    if (curHistory < 3)
                    {
                        for (i = 0; i < len(history + HISTORY_LENGTH*(curHistory)); i++)
                        {
                            interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
                        }
                    }
                    else
                    {
                        for (i = 0; i < len(history + HISTORY_LENGTH*3); i++)
                        {
                            interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
                        }
                    }
                    
                    if (ret == ARROW)
                    {
                        if (curHistory < 3) curHistory++;
                        else curHistory = 1;
                    }

                    pS(history + HISTORY_LENGTH*curHistory, FALSE);
                    
                    clear(input, 100);
                    interrupt(0x21, 0x01, input, &ret, 0);

                    if (ret == '\r') 
                    {
                        copy(history + HISTORY_LENGTH*curHistory, input);
                        break;
                    }
                }
            }
        }
        else if (ret == '\t')
        {
            if (input[0] == '.' && input[1] == '/')
            {   
                autocompleteFileInDir(input, curDirIndex);
                while (TRUE)
                {
                    interrupt(0x21, 0x01, input, &ret, 0);
                    if (ret != '\t') break;
                }
            }
            else if (input[0] == 'c' && input[1] == 'd' && input[2] == ' ')
            {
                autoCompletePath(input, curDirIndex);
                while (TRUE)
                {
                    interrupt(0x21, 0x01, input, &ret, 0);
                    if (ret != '\t') break;
                }
            }
        }

        // Read commandget
        splitArgs(input, argc, argv);
        getCommandType(argc, &type);
        putArgs(curDirIndex, type, argv);

        commandHandler(type, argc, argv, &curDirIndex, curPath);      

        // Add command to history
        addHistory(input, history);
    }
}

// ================= HANDLER =====================
void getCommandType(char* argc, char* type)
{
    if (strCmp(argc, "cd")) *type = cd;
    else if (strCmp(argc, "ls")) *type = ls;
    else if (strCmp(argc, "mkdir")) *type = mkdir;
    else if (strCmp(argc, "./")) *type = run;
    else *type = bin;
}

void commandHandler(int type, char* argc, char* argv, char* curDirIndex, char* curPath)
{
    switch (type)
    {
        case cd:
            _cd_(argv, curDirIndex, curPath);
            break;
        case ls:
            // interrupt(0x21, 0xFF << 8 | 0x6, "ls", 0x4000, &test);
            _ls_(*curDirIndex);
            break;
        case mkdir:
            interrupt(0x21, 0xFF << 8 | 0x6, "mkdir", 0x3000, 0);
            // _mkdir_(argv, *curDirIndex);
            break;
        case run:
            _run_(argv, *curDirIndex);
            break;
        case bin:
            _bin_(argc);
        default:
            break;
    }
}

void addHistory(char* input, char* history)
{
    int i;
    for (i = HISTORY_LENGTH * 2; i >= 0; i -= HISTORY_LENGTH)
    {
        clear(history + i + HISTORY_LENGTH, HISTORY_LENGTH);
        copy(history + i, history + i + HISTORY_LENGTH);
    }
    copy(input, history);
}

void autocompleteFileInDir(char* input, char curDirIndex)
{
    int i;
    char files[SECTOR_SIZE * 2];
    char filename[14];

    // Load files
    interrupt(0x21, (0 << 8) | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, (0 << 8) | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    copy(input + 2, filename);
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i] == curDirIndex && files[i + 1] != DIR)
        {
            if (strSubset(filename, files + i + NAME_OFFSET))
            {
                pS(files + i + NAME_OFFSET + len(filename), FALSE);
                copy(files + i + NAME_OFFSET, input + len(input)-len(filename));
                break;
            }
        }
    }
}

void autoCompletePath(char* input, char curDirIndex)
{
    int i, j, lastPathDivider;
    char nextDir[14];
    char argc[10];
    char argv[64];

    splitArgs(input, argc, argv);
    lastPathDivider = len(argv);
    while (argv[lastPathDivider] != PATH_DIVIDER && lastPathDivider > 0) lastPathDivider--;
    if (lastPathDivider == 0)
    {
        autoCompleteDirInDir(input, argv, curDirIndex);
        return;
    }
    i = 0;
    while (argv[i] != '\0' && i < lastPathDivider)
    {
        if (i == 0 || argv[i] == PATH_DIVIDER)
        {
            if (argv[i] == PATH_DIVIDER) i++;
            j = i;
            while (argv[j] != PATH_DIVIDER && argv[j] != '\0') 
            {
                j++;
            }
            if (argv[j] == PATH_DIVIDER) j--;

            copyRange(argv, nextDir, i, j);

            if (strCmp(nextDir, ".."))
            {
                curDirIndex = getParentIndexByCurIndex(curDirIndex);
            }
            else
            {
                if (isDirExist(nextDir, curDirIndex))
                {
                    curDirIndex = getDirIndexByName(nextDir, curDirIndex);
                } else return;
                
            }
        }
        i++;
    }   
    autoCompleteDirInDir(input, argv + lastPathDivider + 1, curDirIndex);
}

void autoCompleteDirInDir(char* input, char* dirname, char curDirIndex)
{
    int i;
    char files[SECTOR_SIZE * 2];

    // Load files
    interrupt(0x21, (0 << 8) | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, (0 << 8) | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i] == curDirIndex && files[i + 1] == DIR)
        {
            if (strSubset(dirname, files + i + NAME_OFFSET))
            {
                pS(files + i + NAME_OFFSET + len(dirname), FALSE);
                copy(files + i + NAME_OFFSET, input + len(input)-len(dirname));
                break;
            }
        }
    }
}

// ==================== COMMAND METHODS ==================
void _ls_(char curDirIndex)
{
    int i, j;
    char files[SECTOR_SIZE * 2];

    // load files sector
    interrupt(0x21, 0 << 8 | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0 << 8 | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i] == curDirIndex)
        {
            (files[i + 1] == DIR) ? pS("[d] ", FALSE):pS("[f] ", FALSE);

            j = 0;            
            while (files[i + NAME_OFFSET + j] != '\0' && j < 14)
            {
                pC(files[i + NAME_OFFSET + j], FALSE);
                j++;
            }
            pC(' ', TRUE);
        }
    }
    // pC(' ', TRUE);

    // pC(' ', FALSE);
    // pS(" DIR: ", FALSE);
    // for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    // {
    //     if (files[i + 1] == DIR && files[i] == curDirIndex)
    //     {
    //         j = 0;
    //         while (files[i + NAME_OFFSET + j] != '\0' && j < 14)
    //         {
    //             pC(files[i + NAME_OFFSET + j], FALSE);
    //             j++;
    //         }
    //         pC(' ', FALSE);
    //     }
    // }
    // pC(' ', TRUE);
}

void _mkdir_(char* dirname, char parentIndex)
{
    if (isDirExist(dirname, parentIndex))
    {
        pS(" There is already directory with that name!", TRUE);
        return;
    }
    interrupt(0x21, (parentIndex << 8) | 0x5, 0, dirname, 0);
}

void _cd_(char* dirname, char* curDirIndex, char* curPath)
{
    int i, j;
    char nextDir[14];
    char tmpDirIndex;

    i = 0;
    tmpDirIndex = *curDirIndex;
    while (dirname[i] != '\0')
    {
        if (i == 0 || dirname[i] == PATH_DIVIDER)
        {
            if (dirname[i] == PATH_DIVIDER) i++;
            j = i;
            while (dirname[j] != PATH_DIVIDER && dirname[j] != '\0') 
            {
                j++;
            }
            if (dirname[j] == PATH_DIVIDER) j--;

            clear(nextDir, 14);
            copyRange(dirname, nextDir, i, j);
            if (strCmp(nextDir, ".."))
            {
                j = 0;
                while (curPath[j] != '\0') j++;
                while (curPath[j] != '/') 
                {
                    curPath[j] = EMPTY;
                    j--;
                }
                curPath[j] = EMPTY;
                tmpDirIndex = getParentIndexByCurIndex(tmpDirIndex);
            }
            else
            {
                if (!isDirExist(nextDir, tmpDirIndex))
                {
                    pS(" There is no such file or directory", TRUE);
                }
                else
                {
                    tmpDirIndex = getDirIndexByName(nextDir, tmpDirIndex);
                    concat(curPath, "/");
                    concat(curPath, nextDir);
                }
                
            }
        }
        i++;
    }
    *curDirIndex = tmpDirIndex; 
}

void _run_(char* filename, char curDirIndex)
{
    if (!isFileExist(filename, curDirIndex))
    {
        pS("There is no such file or directory!", TRUE);
        return;
    }
    interrupt(0x21, (curDirIndex << 8) | 0x6, filename, 0x3000, 0);
}

void _bin_(char* filename)
{
    if (!isFileExist(filename, BIN_INDEX))
    {
        pS("Command not found!", TRUE);
        return;
    }
    interrupt(0x21, (BIN_INDEX << 8) | 0x6, filename, 0x3000, 0);
}

void splitArgs(char* input, char* argc, char* argv)
{
    int i;

    if (input[0] == '.' && input[1] == '/')
    // Command in form ""./filename"
    {
        copyRange(input, argc, 0, 1);
    }    
    else
    {
        i = 0;
        while(input[i] != SEPARATOR && input[i] != '\0') i++;

        if (input[i] == SEPARATOR)
        // Command in form "command path"
        {
            copyRange(input, argc, 0, i-1);
            copy(input + i + 1, argv);
        }
        // Command in form "command" or "filename"
        else
        {
            copy(input, argc);
        }
    }
}