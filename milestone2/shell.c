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
void printString(char *string);
int getCommandHandler(char* command);
int strCmp(char* str1, char* str2);

main()
{
    int running;
    int command;
    char* input;
    char* curDir = "$"; // Begin in root
    printString(curDir);

    running = 1;
    while(running)
    {
        interrupt(0x21, (0 << 8) | 0x1, &input, 0, 0);
        command = getCommandHandler(input);
        if (command == 1)
        {
            printString("did you just command cd?\n");
        }
        printString("You just put a command\n");
    }
    printString("Exitttttttt");
}

int len(char* string)
{
    int i = 0;
    while (string[i] != '\0') i++;
    return i;
}

void printString(char* string)
{   
    int i = 0;
    while(string[i] != '\0' && string[i])
    {
        int ch = string[i];

        if (ch == '\n')
            interrupt(0x10, 0xe*256+'\r', 0, 0, 0);

        interrupt(0x10, 0xe*256+ch, 0, 0, 0);
        i++;
    }
}

int getCommandHandler(char* command)
{
    if (strCmp(command, "cd"))
    {
        printString("You just used cd command\n");
        return cd;
    }
}

int strCmp(char* str1, char* str2)
{
    int i;
    int length = len(str1);

    if (length != len(str2)){
        printString("Not to fast! 11111111");
        return FALSE;
    }
    else
    {
        for (i = 0; i < length; i++)
        {
            if (str1[i] != str2[i]){
                printString("Not to fast!");
                break;
            }
        }
    }
    return (str1[i] == str2[i]);
}
