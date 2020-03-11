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

// File exception
#define FILE_NOT_FOUND -1
#define FILE_EXIST -1
#define FILES_FULL -2
#define SECTORS_FULL -3
#define FOLDER_UNVALID -4

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
int findFilenameInDir(char* path, char parentIndex);
void fileExceptionHandler(int result);

main()
{
    char buffer[FILE_SIZE];
	int suc;
	makeInterrupt21();
	printLogo();

	// interrupt(0x21, 0x4, buffer, "key.txt", &suc);
	// if (suc)
	// {
	// 	interrupt(0x21,0x0, "Key : ", 0, 0);
	//  	interrupt(0x21,0x0, buffer, 0, 0);
	// }
	// else
	// {
	// 	interrupt(0x21, 0x6, "milestone1", 0x2000, &suc);
	// }
	while (1)
    {
        
    }
} 

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
   char AL, AH;
   AL = (char) (AX);
   AH = (char) (AX >> 8);
   switch (AL) {
      case 0x00:
         printString(BX);
         break;
      case 0x01:
         readString(BX);
         break;
      case 0x02:
         readSector(BX, CX);
         break;
      case 0x03:
         writeSector(BX, CX);
         break;
      case 0x04:
         readFile(BX, CX, DX, AH);
         break;
      case 0x05:
         writeFile(BX, CX, DX, AH);
         break;
      case 0x06:
        //  executeProgram(BX, CX, DX, AH);
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
    char map[SECTOR_SIZE], files[SECTOR_SIZE * 2], sectors[SECTOR_SIZE];
    int fileIndex;
    int sectorindex;
    int empty_sector;
    int sectorId, sectorCount;
    char tmp_buff[SECTOR_SIZE];

    // Read sector map, files, and sectors
    
    readSector(map, MAP_SECTOR);
    readSector(files, FILES_SECTOR_1);
    readSector(files + SECTOR_SIZE, FILES_SECTOR_2);
    readSector(sectors, SECTORS_SECTOR);

    // Find file in parentIndex
    fileIndex = findFilenameInDir(path, parentIndex);
    if (fileIndex != FILE_EXIST) // file already exist
    {
        *sectors = FILE_EXIST;
        return;
    }

    // find a free entry in the files
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
        if (files[i] == EMPTY) break;
    if (i == SECTOR_SIZE * 2)
    {
        *sectors = FILES_FULL;
        return;
    }
    fileIndex = i;

    // find a free sectors entry
    for (i = 0; i < SECTOR_SIZE; i += SECTORS_COLUMNS)
        if (sectors[i] == EMPTY) break;
    if (i == SECTOR_SIZE)
    {
        *sectors = SECTORS_FULL;
        return;
    }
    sectorindex = i;

    // Check if there enough space to store file
    empty_sector;
    for (i = 0, empty_sector = 0; i < MAX_BYTE && empty_sector < *sectors; i++)
    {
        if (map[i] != USED)
            empty_sector++;
    }
    if (i == MAX_BYTE) return;

    // Put files in the parentIndex
    files[fileIndex] = parentIndex;

    // Point the files to the sector
    files[fileIndex + 1] = sectorindex;

    // Clear memory that will be used to store filename
    clear(files + fileIndex + NAME_OFFSET, 14);

    // Store filename
    i = 0, j = 0;
    while (path[i] != '\0' && i < 14) i++;
    while (path[i] != '/' && i > 0) i--;
    if (path[i] == '/') i += 1;
    while (path[i] != '\0' && j < 14);
        files[fileIndex + NAME_OFFSET + j++] = path[i++];

    // Store file 
    for (sectorId = 0, sectorCount = 0; sectorId < MAX_BYTE && sectorCount < *sectors; sectorId++)
    {
        if (map[sectorId] == EMPTY)
        {
            // Mark sector as used
            map[sectorId] = USED;

            // Put sectorId to sectors correspondent to files
            sectors[sectorindex + sectorCount] = sectorId;
        
            // clear temp buffer
            clear(tmp_buff, SECTOR_SIZE);

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
    int i;
    char map[SECTOR_SIZE], files[SECTOR_SIZE * 2], sectors[SECTOR_SIZE];
    int fileIndex;
    int sectorIndex;    

    // Read sector map, files, and sectors
    readSector(map, MAP_SECTOR);
    readSector(files, FILES_SECTOR_1);
    readSector(files + SECTOR_SIZE, FILES_SECTOR_2);
    readSector(sectors, SECTORS_SECTOR);

    // Find filename in parentIndex
    fileIndex = findFilenameInDir(path, parentIndex);
    if (fileIndex == FILE_NOT_FOUND) // file not found
    {
        *result = FILE_NOT_FOUND;
        return;
    }

    // Load file to buffer
    sectorIndex = files[fileIndex + 1] * SECTORS_COLUMNS;
    for (i = 0; i < FILES_COLUMNS && sectors[sectorIndex + i] != EMPTY; i++)
    {
        readSector(buffer + i*SECTOR_SIZE, sectors[sectorIndex + i]);
    }
}

void executeProgram(char *filename, int segment, int *success)
{
	char buffer[FILE_SIZE];
	int i;
	// readFile(buffer, filename, success);
	if (*success)
	{
		for (i = 0; i < FILE_SIZE; i++)
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
    int success;
    char buffer[FILE_SIZE];
    readFile(buffer, "logo.txt", &success, ROOT);
    fileExceptionHandler(success);
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

int findFilenameInDir(char* path, char parentIndex)
{
    int i, j;
    int filenameIndex;
    int filefound;

    // Load files
    char files[SECTOR_SIZE * 2];
    readSector(files, FILES_SECTOR_1);
    readSector(files + SECTOR_SIZE, FILES_SECTOR_2);

    // Find filename in path
    i = 0;
    while (path[i] != '\0' && i < 14) i++;
    while (path[i] != '/' && i > 0) i--;
    if (path[i] == '/') 
        filenameIndex = i+1; 
    else 
        filenameIndex = 0;

    // Find filename in parentIndex
    filefound = 0;
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
    {
        if (files[i] != EMPTY && files[i + 1] != DIR) // Check only files, not directory
        {
            filefound = 1;
            j = 0;
            while (path[filenameIndex + j] != '\0')
            {
                if (path[filenameIndex + j] != files[i + NAME_OFFSET + j])
                {
                    filefound = 0;
                    break;
                }
                j++;
            }
            if (filefound) return i;
        }
    }
    return FILE_NOT_FOUND;
}

void fileExceptionHandler(int result)
{
    if (result == FILE_NOT_FOUND)
    {
        printString("File not found/already exist");
    }
    else if (result == FILES_FULL)
    {
        printString("No empty space in files");
    }
    else if (result == SECTORS_FULL)
    {
        printString("No empty space in sectors");
    }
}