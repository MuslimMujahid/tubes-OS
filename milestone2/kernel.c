#define SECTOR_SIZE 512
#define MAP_SECTOR 1
#define FILES_SECTOR_1 2
#define FILES_SECTOR_2 3
#define SECTORS_SECTOR 4
#define MAX_BYTE 512
#define NAME_OFFSET 2
#define SECTORS_COLUMNS 16
#define FILES_COLUMNS 16
#define ROOT 0xFF
#define USED 0xFF
#define EMPTY 0x0

void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void writeSector(char *buffer, int sector);
void readSector(char *buffer, int sector);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void executeProgram(char *filename, int segment, int *success);
void clear(char *buffer, int length); 
int mod(int a, int b);
int div(int a, int b);
void printChar(char c);
void printLogo();
int strCmp(char* str1, char* str2);

main()
{
    char buffer[10240];
	int suc;
	makeInterrupt21();
	printLogo();
	interrupt(0x21, 0x4, buffer, "key.txt", &suc);
	if (suc)
	{
		interrupt(0x21,0x0, "Key : ", 0, 0);
	 	interrupt(0x21,0x0, buffer, 0, 0);
	}
	else
	{
		interrupt(0x21, 0x6, "milestone1", 0x2000, &suc);
	}
	while (1)
    {
        
    }
} 

void handleInterrupt21 (int AX, int BX, int CX, int DX)
{
    switch (AX) {
      case 0x0:
          printString(BX);
          break;
      case 0x1:
          readString(BX);
          break;
      case 0x2:
        readSector(BX, CX);
        break;
      case 0x3:
        writeSector(BX, CX);
        break;
      case 0x4:
        // readFile(BX, CX, DX);
        break;
      case 0x5:
        // writeFile(BX, CX, DX);
        break;
      case 0x6:
        executeProgram(BX, CX, DX);
        break;
      default:
          printString("Invalid interrupt");
    }
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

void readString(char* string)
{
    int dashn = 0xa;
    int endStr = 0x0;
    int enter = 0xd;
    int backsp = 0x8;
    int dashr = 0xd;
    int count = 0;

    while(1)
    {
        /* Call interrupt 0x16 */
        /* interrupt #, AX, BX, CX, DX */
        char ascii = interrupt(0x16,0,0,0,0);
        if (ascii == enter)
        {              
            string[count] = 0x0;
            interrupt(0x10,0xe*256+dashn,0,0,0);
            interrupt(0x10,0xe*256+dashr,0,0,0);
            return;
        }
        else if (ascii == backsp)
        {
            if (count > 1)
            {
                string[count] = 0x0;
                count--;
                interrupt(0x10,0xe*256+0x8,0,0,0);
                count++;
                interrupt(0x10,0xe*256+0x0,0,0,0);
                count--;
                interrupt(0x10,0xe*256+0x8,0,0,0);
            }
        }
        else
        {
            string[count] = ascii;
            interrupt(0x10, 0xe*256+ascii, 0, 0, 0);
            count++;
        }     
    }
}

void readSector(char *buffer, int sector)
{
    interrupt(
      0x13, 
      0x201, 
      buffer, 
      div(sector, 36) * 0x100 + mod(sector, 18) + 1, 
      mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector)
{
    interrupt(
      0x13, 
      0x301, 
      buffer, 
      div(sector, 36) * 0x100 + mod(sector, 18) + 1, 
      mod(div(sector, 18), 2) * 0x100);
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex)
{
    int i, j;

    // Read sector map, files, and sectors
    char map[SECTOR_SIZE], files[SECTOR_SIZE * 2], sectors[SECTOR_SIZE];
    readSector(map, MAP_SECTOR);
    readSector(files, FILES_SECTOR_1);
    readSector(files + SECTOR_SIZE, FILES_SECTOR_2);
    readSector(sectors, SECTORS_SECTOR);

    // find a free entry in the files
    for (i = 0; i < SECTOR_SIZE; i += FILES_COLUMNS)
        if (files[i] == EMPTY) break;
    if (i == SECTOR_SIZE * 2) return;
    int fileindex = i;

    // find a free sectors entry
    for (i = 0; i < SECTOR_SIZE; i += SECTORS_COLUMNS)
        if (sectors[i] == EMPTY) break;
    if (i == SECTOR_SIZE) return;
    int sectorindex = i;

    // Check if there enough space to store file
    int empty_sector;
    for (i = 0, empty_sector = 0; i < MAX_BYTE && empty_sector < *sectors; i++)
    {
        if (map[i] != USED)
            empty_sector++;
    }
    if (i == MAX_BYTE) return;

    // Put files in the parentIndex
    files[fileindex] = parentIndex;

    // Point the files to the sector
    files[fileindex + 1] = sectorindex;

    // clear the memory to put name
    clear(files + fileindex + NAME_OFFSET, 14);

    // Clear memory that will be used to store filename
    clear(files + fileindex + NAME_OFFSET, 14);

    // Store filename
    i = 0, j = 0;
    while (path[i++] != '\0' && i < 14);
    while (path[i--] != '/' && i > 0);
    while (path[++i] != '\0' && i < 14);
        files[fileindex + NAME_OFFSET + j++] = path[i];

    // Store file 
    int sectorId, sectorCount;
    char tmp_buff[SECTOR_SIZE];
    for (sectorId = 0, sectorCount = 0; sectorId < MAX_BYTE && sectorCount < *sectors; sectorId++)
    {
        if (map[sectorId] == EMPTY)
        {
            // Mark sector as used
            map[sectorId] = USED;

            // Put sectorId to sectors correspondent to files
            sectors[sectorindex + sectorCount] = sectorId;
        
            // clear temp buffer
            clear(tmp_buff, 512);

            // Write to the empty sector
            for (i = 0; i < SECTOR_SIZE; i++)
                tmp_buff[i] = buffer[sectorCount * SECTOR_SIZE + i];
            writeSector(tmp_buff, sectorId);

            sectorCount++;
        }
    }

    // Write to system
    writeSector(map, MAP_SECTOR);
    writeSector(files, FILES_SECTOR_1);
    writeSector(files + SECTOR_SIZE, FILES_SECTOR_2);
    writeSector(sectors, SECTORS_SECTOR);

}

void readFile(char *buffer, char *path, int *result, char parentIndex)
{
    
}

void executeProgram(char *filename, int segment, int *success)
{
	char buffer[10240];
	int i;
	// readFile(buffer, filename, success);
	if (*success)
	{
		for (i = 0; i < 10240; i++)
		{
			putInMemory(segment, i, buffer[i]);
		}
		launchProgram(segment);
	}
}

void clear(char *buffer, int length)
{
    int i = 0;
    for (i = 0; i < length; i++)
      buffer[i] = 0x0;
}

int mod(int a, int b)
{
    while(a >= b)
        a = a - b;
    return a;
}

int div(int a, int b)
{
    int q = 0;
    while(q*b <= a)
        q = q+1;
    return q-1;
}

void printChar(char c)
{
    interrupt(0x10, 0xe*256+c, 0, 0, 0);
}

void printLogo()
{
    char buffer[10240];
    int success;
    // readFile(buffer, "logo.txt", 0);
    printString(buffer);
}

int strCmp(char* str1 ,char* str2)
{
    int it = 0;
    while(str1[it] != '\0' && str2[it] != '\0')
    {
        if (str1[it] != str2[it])
            break;
        it++;
    }
    return (str1[it] == str2[it]);
}