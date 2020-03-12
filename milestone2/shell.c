#define TRUE 1
#define FALSE 0

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

int len(char* string);
void pS(char *string, int newLine);
void pI(int i, int newLine);
void pC(char c, int newLine);
int getCommandHandler(char* command);
void commandHandler(int type, char* curDirIndex);
int strCmp(char* str1, char* str2);

// command methods
void _ls_(char curDirIndex);
void _mkdir_(char* dirname, char parentIndex);

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
        commandHandler(command, &curDirIndex);          
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
    if (strCmp(command, "cd")) return cd;
    else if (strCmp(command, "ls")) return ls;
    else if (strCmp(command, "mkdir")) return mkdir;
}

void commandHandler(int type, char* curDirIndex)
{
    switch (type)
    {
        case cd:
            pS("You just command cd!", TRUE);
            break;
        case ls:
            _ls_(0xFF);
            break;
        case mkdir:
            pS("You just created a dir", TRUE);
            _mkdir_("tttttttttt", ROOT);
            break;
        default:
            break;
    }
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

void _ls_(char curDirIndex)
{
    int i, j;
    char files[SECTOR_SIZE * 2];

    // load files sector
    interrupt(0x21, 0 << 8 | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0 << 8 | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);

    // print all filena,e sin curDirIndex
    pC(' ', FALSE);
    pS("FILES: ", FALSE);
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
    pS("DIR: ", FALSE);
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
    interrupt(0x21, (parentIndex << 8) | 0x5, 0, dirname, 0);
}