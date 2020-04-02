#define TRUE 1
#define FALSE 0
#define SECTOR_SIZE 512
#define FILES_SECTOR_1 257
#define FILES_SECTOR_2 258
#define ARGS_SECTOR 512
#define FILES_COLUMNS 16
#define NAME_OFFSET 2
#define DIR 0xFF

void pS(char *string, int newLine);
void pC(char c, int newLine);
void getCurDir(char* curdir);

main()
{
    int i, j;
    char files[SECTOR_SIZE * 2];
    char curDirIndex;
    
    pS("jalananananan", TRUE);
    // load files sector
    interrupt(0x21, 0 << 8 | 0x02, files, FILES_SECTOR_1, 0);
    interrupt(0x21, 0 << 8 | 0x02, files + SECTOR_SIZE, FILES_SECTOR_2, 0);
    getCurDir(&curDirIndex);

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
    interrupt(0x21, 0xFF << 8 | 0x6, "shell", 0x2000, 0);
}

void getCurDir(char* curdir)
{
    char args[SECTOR_SIZE];

    interrupt(0x21, 0x02, args, ARGS_SECTOR, SECTOR_SIZE);
    *curdir = args[0]; 
}

void pS(char *string, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x0, string, newLine, 0);
}

void pC(char c, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x8, c, newLine, 0);
}