#define TRUE 1
#define FALSE 0
#define SEPARATOR ' '
#define PATH_DIVIDER '/'
#define ARROW '\f'

#define SECTOR_SIZE 512
#define MAP_SECTOR 256
#define FILES_SECTOR_1 257
#define FILES_SECTOR_2 258
#define SECTORS_SECTOR 259
#define MAX_BYTE 256
#define MAX_FILES 32
#define FILE_SIZE 8192
#define NAME_OFFSET 2
#define SECTORS_COLUMNS 16
#define FILES_COLUMNS 16
#define ROOT 0xFF
#define USED 0xFF
#define EMPTY 0x0
#define DIR 0xFF
#define BIN_INDEX 0x0

// command type
#define cd 1
#define ls 2
#define mkdir 3
#define run 4
#define bin 5

// handler
#define HISTORY_LENGTH 64
#define ARGC_LENGTH 10
#define ARGV_LENGTH 64

int getCommandType(char* argc);
void commandHandler(int type, char* argc, char* argv, char* curDirIndex, char* curPath);
void getArg(char* input, char* argc, char* argv);
int isDirExist(char* dirname, char curDirIndex);
int isFileExist(char* dirname, char curDirIndex);
char getDirIndexByName(char* dirname, char curDirIndex);
char getParentIndexByCurIndex(char curDirIndex);

void clear(char *buffer, int length); 
int len(char* string);
void pS(char *string, int newLine);
void pI(int i, int newLine);
void pC(char c, int newLine);
void copy(char* src, char* dest);
void copyRange(char* src, char* dest, int l, int h);
int strCmp(char* str1, char* str2);
int strSubset(char* str1, char* str2);
void concat(char* src, char* dest);
void addHistory(char* input, char* history);
void autocompleteFileInDir(char* input, char curDirIndex);
void autoCompletePath(char* input, char curDirIndex);
void autoCompleteDirInDir(char* input, char* dirname, char curDirIndex);

// command methods
void _ls_(char curDirIndex);
void _mkdir_(char* dirname, char parentIndex);
void _cd_(char* dirname, char* curDirIndex, char* curPath);
void _run_(char* filename, char curDirIndex);
void _bin_(char* filename);

main()
{
    int i;
    int running;
    int type;
    char input[100];
    char curPath[100];
    char argc[10];
    char argv[64]; 
    char curDirIndex;
    char files[SECTOR_SIZE * 2];
    char history[HISTORY_LENGTH * 4];
    int curHistory;
    int inputLength;
    int ret;

    clear(curPath, 100);
    clear(argc, 10);
    clear(argv, 64);

    running = 1;
    curPath[0] = '$'; 
    curDirIndex = ROOT; // Begin in root
    while(running)
    {
        curHistory = 0;

        // clear input container
        clear(input, 100);

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
        getArg(input, argc, argv);
        type = getCommandType(argc);
        commandHandler(type, argc, argv, &curDirIndex, curPath);      

        // Add command to history
        addHistory(input, history);
    }
}

// ================= HANDLER =====================
int getCommandType(char* argc)
{
    if (strCmp(argc, "cd")) return cd;
    else if (strCmp(argc, "ls")) return ls;
    else if (strCmp(argc, "mkdir")) return mkdir;
    else if (strCmp(argc, "./")) return run;
    else return bin;
    
}

void commandHandler(int type, char* argc, char* argv, char* curDirIndex, char* curPath)
{
    switch (type)
    {
        case cd:
            _cd_(argv, curDirIndex, curPath);
            break;
        case ls:
            _ls_(*curDirIndex);
            break;
        case mkdir:
            _mkdir_(argv, *curDirIndex);
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

void getArg(char* input, char* argc, char* argv)
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

int isDirExist(char* dirname, char curDirIndex)
{
    int i, j, filefound;
    char files[SECTOR_SIZE * 2];

    // Load files
    interrupt(0x21, (0 << 8) | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, (0 << 8) | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    // Find filename in parentIndex
    filefound = 0;
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        
        
        if (files[i] == curDirIndex && files[i + 1] == DIR) // Check only dir
        {
            filefound = 1;
            j = 0;
            while (dirname[j] != '\0')
            {
                if (dirname[j] != files[i + NAME_OFFSET + j])
                {
                    filefound = 0;
                    break;
                }
                j++;
            }
            if (filefound) return TRUE; 
        }
    }
    return FALSE;
}

int isFileExist(char* dirname, char curDirIndex)
{
    int i, j, filefound;
    char files[SECTOR_SIZE * 2];

    // Load files
    interrupt(0x21, (0 << 8) | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, (0 << 8) | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    // Find filename in parentIndex
    filefound = 0;
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i] == curDirIndex && files[i + 1] != DIR) // Check only dir
        {
            filefound = 1;
            j = 0;
            while (dirname[j] != '\0')
            {
                if (dirname[j] != files[i + NAME_OFFSET + j])
                {
                    filefound = 0;
                    break;
                }
                j++;
            }
            if (filefound) return TRUE;
        }
    }
    return FALSE;
}

char getDirIndexByName(char* dirname, char curDirIndex)
{
    int i, j, filefound;
    char files[SECTOR_SIZE * 2];

    // Load files
    interrupt(0x21, (0 << 8) | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, (0 << 8) | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i] == curDirIndex && files[i + 1] == DIR)
        {
            if (strCmp(files + i + NAME_OFFSET, dirname))
            {
                return (i >> 0x4);
            }
        }
    }
}

char getParentIndexByCurIndex(char curDirIndex)
{
    char files[SECTOR_SIZE * 2];

    // Load files
    interrupt(0x21, (0 << 8) | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, (0 << 8) | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    return files[curDirIndex * FILES_COLUMNS];
}

void clear(char *buffer, int length)
{
    int i = 0;
    for (i = 0; i < length; i++)
      buffer[i] = 0x0;
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

    getArg(input, argc, argv);
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

int len(char* string)
{
    int i = 0;
    while (string[i] != '\0') i++;
    return i;
}

void pS(char *string, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x0, string, newLine, 0);
}

void pI(int i, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x7, i, newLine, 0);
}

void pC(char c, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x8, c, newLine, 0);
}

void copy(char* src, char* dest)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0'; 
}

void copyRange(char* src, char* dest, int l, int h)
{
    int i, j; 

    i = 0;
    while (src[l + i] != '\0' && i <= h-l)
    {
        dest[i] = src[l+i];
        i++;
    }
    dest[l+i] = '\0'; 
}

int strCmp(char* str1, char* str2)
{
    int i;
    int length = len(str1);

    if (length != len(str2)){
        return FALSE;
    }
    else
    {
        for (i = 0; i < length; i++)
        {
            if (str1[i] != str2[i]){
                break;
            }
        }
    }
    return (str1[i] == str2[i]);
}

int strSubset(char* str1, char* str2)
{
    int i;
    int length;;

    length = len(str1);
    for (i = 0; i < length; i++)
    {
        if (str1[i] != str2[i])
        {
            return FALSE;
        }
    }
    return TRUE;
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

void concat(char* src, char* dest)
{
    int i, j;

    i = 0;
    j = 0;
    while (src[i] != '\0') i++;
    while (dest[j] != '\0')
    {
        src[i + j] = dest[j];
        j++;
    }
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