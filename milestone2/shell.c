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


int len(char* string);
void pS(char *string, int newLine);
void pI(int i, int newLine);
void pC(char c, int newLine);
int getCommandHandler(char* command);
int strCmp(char* str1, char* str2);

main()
{
    int running;
    int command;
    char* input;
    char curDir[100]; 

    running = 1;
    curDir[0] = '$'; // Begin in root
    while(running)
    {
        pS(curDir, FALSE);
        interrupt(0x21, (0 << 8) | 0x1, &input, 0, 0);
        command = getCommandHandler(input);
        if (command == 1)
        {
            pS("did you just command cd?", TRUE);
        }
        pS("You just put a command", TRUE);
    }
    pS("Exitttttttt", TRUE);
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
    if (strCmp(command, "cc"))
    {
        pS("You just used cd command", TRUE);
        return cd;
    }
}

int strCmp(char* str1, char* str2)
{
    int i;
    int length = len(str1);

    if (length != len(str2)){
        pI(length, TRUE);
        pI(len(str2), TRUE);
        pS("Not to fast! 11111111", TRUE);
        return FALSE;
    }
    else
    {
        for (i = 0; i < length; i++)
        {
            if (str1[i] != str2[i]){
                pS("Not to fast!", TRUE);
                break;
            }
        }
    }
    return (str1[i] == str2[i]);
}
