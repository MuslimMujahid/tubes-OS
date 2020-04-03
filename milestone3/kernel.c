#include "macro.h"

void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string, int newline);
void readString(char *string, char* ret);
void writeSector(char *buffer, int sector);
void readSector(char *buffer, int sector);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void executeProgram(char *path, int segment, int *success, char parentIndex);
void printChar(char c, int newline);
void printLogo();
void fileExceptionHandler(int result);
void printInt(int i, int newLine);

main()
{
    char buffer[FILE_SIZE];
	  int suc = 0;
	  makeInterrupt21();
	  // printLogo();
	  interrupt(0x21, 0xFF << 8 | 0x6, "shell", 0x2000, 0);
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
            printString(BX, CX);
            break;
        case 0x01:
            readString(BX, CX);
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
            executeProgram(BX, CX, DX, AH);
            break;
        case 0x07:
            printInt(BX, CX);
            break;
        case 0x08:
            printChar(BX, CX);
            break;
        default:
            printString("Invalid interrupt", TRUE);
    }
}

void printString(char* string, int newline)
{
    char ch;
    int i = 0;
    while(string[i] != '\0')
    {
        ch = string[i];
        if (ch == '\n') printChar('\r', FALSE);
        printChar(ch, FALSE);
        i++;
    }
    if (newline) {
		    printChar('\r', TRUE);
	  }
}

void readString(char* string, char* ret)
{
    int count = len(string);
    char input;
    while(1)
    {
        /* Call interrupt 0x16 */
        /* interrupt #, AX, BX, CX, DX */
        input = interrupt(0x16,0,0,0,0);
        if (input == '\r')
        {
            *ret = '\r';
            string[count] = 0x0;
            interrupt(0x10, 0xe*256+'\n', 0, 0, 0);
            interrupt(0x10, 0xe*256+'\r', 0, 0, 0);
            return;
        }
        else if (input == '\b')
        {
            if (count > 0)
            {
                string[count] = 0x0;
                count--;
                interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
                count++;
                interrupt(0x10, 0xe*256+0x0, 0, 0 ,0);
                count--;
                string[count] = 0x0;
                interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
            }
        }
        else if (input == '\x20') // space
        {
            string[count] = ' ';
            interrupt(0x10, 0xe*256+input, 0, 0, 0);
            count++;
        }
        else if (input == '\t')
        {
            *ret = '\t';
            return;
        }
        else if (input == 0) // arrow up and down
        {
            *ret = '\f';
            string[count] = input;
            string[count+1] = 0x0;
            return;
        }
        else
        {
            string[count] = input;
            interrupt(0x10, 0xe*256+input, 0, 0, 0);
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

void executeProgram(char *path, int segment, int *success, char parentIndex)
{
	  char buffer[FILE_SIZE];
	  int i;
	  readFile(buffer, path, success, parentIndex);
	  if (*success)
	  {
		    for (i = 0; i < FILE_SIZE; i++)
		    {
			      putInMemory(segment, i, buffer[i]);
		    }
		    launchProgram(segment);
	  }
}

void printChar(char c, int newline)
{
    interrupt(0x10, 0xE00+c, 0, 0, 0);
    if (newline) {
		    interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
		    interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
	  }
}

void printLogo()
{
    int success;
    char buffer[FILE_SIZE];
    readFile(buffer, "logo.txt", &success, ROOT);
    fileExceptionHandler(success);
    printString(buffer, FALSE);
}

void fileExceptionHandler(int result)
{
    if (result == FILE_NOT_FOUND || result == FILE_EXIST)
    {
        printString("File not found/already exist", TRUE);
    }
    else if (result == FILES_FULL)
    {
        printString("No empty space in files", TRUE);
    }
    else if (result == SECTORS_FULL)
    {
        printString("No empty space in sectors", TRUE);
    }
}

void printInt(int i, int newLine)
{
    char integer[10];
    int digit;
    int j;

    // Count digit
    digit = 1;
    j = i;
    while (j > 10)
    {
        j = div(j, 10);
        digit++;
    }

	  integer[0] = '0';
	  integer[1] = '0';
	  integer[2] = '0';
	  integer[3] = '0';
	  integer[4] = '0';
	  integer[5] = '0';
	  integer[6] = '0';
	  integer[7] = '0';
	  integer[8] = '0';
	  integer[9] = '\0';
	  j = 8;
	  while (i != 0 && j >= 0) {
		    integer[j] = '0' + mod(i, 10);
		    i = div(i, 10);
		    j -= 1;
	  }
	  printString(integer + 9 - digit, newLine);
}

void writeFile(char *buffer, char *path, int *sector, char parentIndex)
{
    int i, j;
    char map[SECTOR_SIZE];
    char files[SECTOR_SIZE * 2];
    char sectors[SECTOR_SIZE];
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
    if (fileIndex != FILE_NOT_FOUND) // file already exist
    {
        *sector = FILE_EXIST;
        return;
    }

    // Check if there enough space to store file
    for (i = 0, empty_sector = 0; i < MAX_BYTE && empty_sector < *sector; i++)
    {
        if (map[i] != USED)
            empty_sector++;
    }
    if (i == MAX_BYTE)
    {
        *sector = SECTORS_FULL;
        return;
    }

    // find a free entry in the files
    for (i = 0; i < SECTOR_SIZE * 2; i += FILES_COLUMNS)
        if (files[i + NAME_OFFSET] == EMPTY) break;
    if (i == SECTOR_SIZE * 2)
    {
        *sector = FILES_FULL;
        return;
    }
    fileIndex = i;

    // find a free sectors entry
    for (i = 0; i < SECTOR_SIZE; i += SECTORS_COLUMNS)
        if (sectors[i] == EMPTY) break;
    if (i == SECTOR_SIZE)
    {
        *sector = SECTORS_FULL;
        return;
    }
    sectorindex = i;

    // Put files in the parentIndex
    files[fileIndex] = parentIndex;

    // Point the files to the sector
    // if its a directory then set S to 0xFF
    if (buffer == 0)
    {
        files[fileIndex + 1] = DIR;
    }
    else
    {
        files[fileIndex + 1] = sectorindex >> 0x4;
    }


    // Clear memory that will be used to store filename
    clear(files + fileIndex + NAME_OFFSET, MAX_FILENAME_LENGTH);

    // Store filename
    i = 0, j = 0;
    while (path[i] != '\0' && i < MAX_FILENAME_LENGTH) i++;
    while (path[i] != '/' && i > 0) i--;
    if (path[i] == '/') i += 1;
    while (path[i] != '\0' && j < MAX_FILENAME_LENGTH)
        files[fileIndex + NAME_OFFSET + j++] = path[i++];

    // Store file
    for (sectorId = 0, sectorCount = 0; sectorId < MAX_BYTE && sectorCount < *sector; sectorId++)
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

    *sector = 1;
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
    *result = 1;
}
