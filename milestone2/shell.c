#define TRUE 1
#define FALSE 0
#define SEPARATOR '|'
#define PATH_DIVIDER '/'

#define SECTOR_SIZE 512
#define MAP_SECTOR 1
#define FILES_SECTOR_1 2
#define FILES_SECTOR_2 3
#define SECTORS_SECTOR 4
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

// command type
#define cd 1
#define ls 2
#define mkdir 3

// handler
int len(char* string);
void pS(char *string, int newLine);
void pI(int i, int newLine);
void pC(char c, int newLine);
int getCommandHandler(char* command);
void commandHandler(int type, char* input, char* curDirIndex);
void copy(char* src, char* dest);
void copyRange(char* src, char* dest, int l, int h);
int strCmp(char* str1, char* str2);
int commandCmp(char* str1, char* str2);
void getArg(char* input, char* arg);
int isDirExist(char* dirname, char curDirIndex);
char getDirIndexByName(char* dirname, char curDirIndex);
char getParentIndexByCurIndex(char curDirIndex);

// command methods
void _ls_(char curDirIndex);
void _mkdir_(char* dirname, char parentIndex);
void _cd_(char* dirname, char* curDirIndex);

main()
{
    int running;
    int command;
    char input[100];
    char argInput[100];
    char curDir[100]; 
    char curDirIndex;
    char files[SECTOR_SIZE * 2];

    running = 1;
    curDir[0] = '$'; 
    curDirIndex = ROOT; // Begin in root
    while(running)
    {
        // Print current directory path
        interrupt(0x21, 0x0, "$ ", 0, 0);
        interrupt(0x21, (0 << 8) | 0x1, input, 0, 0);

        // Read command
        command = getCommandHandler(input);
        commandHandler(command, input, &curDirIndex);          
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

int getCommandHandler(char* command)
{
    if (commandCmp(command, "cd")) return cd;
    else if (commandCmp(command, "ls")) return ls;
    else if (commandCmp(command, "mkdir")) return mkdir;
}

void commandHandler(int type, char* input, char* curDirIndex)
{
    char arg[100];
    getArg(input, arg);
    switch (type)
    {
        case cd:
            _cd_(arg, curDirIndex);
            break;
        case ls:
            _ls_(*curDirIndex);
            break;
        case mkdir:
            _mkdir_(arg, *curDirIndex);
            break;
        default:
            break;
    }
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

int commandCmp(char* str1, char* str2)
{
    int i;

    i = 0;
    while(str1[i] != SEPARATOR && str1[i] != '\0' && str2[i] != '\0')
    {
        if (str1[i] != str2[i])
            break;
        i++;
    }
    if (str1[i] == SEPARATOR) i--;
    return (str1[i] == str2[i]);
}

void _ls_(char curDirIndex)
{
    int i, j;
    char files[SECTOR_SIZE * 2];

    // load files sector
    interrupt(0x21, 0 << 8 | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0 << 8 | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    // print all filena,e sin curDirIndex
    pC(' ', FALSE);
    pS(" FILES: ", FALSE);
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i + 1] != DIR && files[i] == curDirIndex)
        {
            j = 0;
            while (files[i + NAME_OFFSET + j] != '\0' && j < 14)
            {
                pC(files[i + NAME_OFFSET + j], FALSE);
                j++;
            }
            pC(' ', FALSE);
        }
    }
    pC(' ', TRUE);

    pC(' ', FALSE);
    pS(" DIR: ", FALSE);
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i + 1] == DIR && files[i] == curDirIndex)
        {
            j = 0;
            while (files[i + NAME_OFFSET + j] != '\0' && j < 14)
            {
                pC(files[i + NAME_OFFSET + j], FALSE);
                j++;
            }
            pC(' ', FALSE);
        }
    }
    pC(' ', TRUE);
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

void getArg(char* input, char* arg)
{
    int i = 0;
    while (input[i] != SEPARATOR && input[i] != '\0')
    {
        i++;
    }
    copy(input + i + 1, arg);
}

void _cd_(char* dirname, char* curDirIndex)
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

            copyRange(dirname, nextDir, i, j);

            if (strCmp(nextDir, ".."))
            {
                tmpDirIndex = getParentIndexByCurIndex(tmpDirIndex);
            }
            else
            {
                if (!isDirExist(nextDir, tmpDirIndex))
                {
                    pS(nextDir, TRUE);
                    pS(" There is no such file", TRUE);
                }
                else
                {
                    tmpDirIndex = getDirIndexByName(nextDir, tmpDirIndex);
                }
            }
        }
        if (dirname[i] != '\0') i++;
    }
    *curDirIndex = tmpDirIndex; 
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