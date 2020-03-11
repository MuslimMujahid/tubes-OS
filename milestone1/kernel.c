void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void writeSector(char *buffer, int sector);
void readSector(char *buffer, int sector);
void writeFile(char *buffer, char *filename, int *sectors);
void readFile(char *buffer, char *filename, int *success);
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
        readFile(BX, CX, DX);
        break;
      case 0x5:
        writeFile(BX, CX, DX);
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
    int count = 0;
    while(1)
    {
        /* Call interrupt 0x16 */
        /* interrupt #, AX, BX, CX, DX */
        char ascii = interrupt(0x16,0,0,0,0);
        if (ascii == '\r')
        {              
            string[count] = 0x0;
            interrupt(0x10, 0xE00+'\n', 0, 0, 0);
            interrupt(0x10, 0xE00+'\r', 0, 0, 0);
            return;
        }
        else if (ascii == '\b')
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

void writeFile(char *buffer, char *filename, int *sectors)
{
    char map[512];
    char dir[512];
    char sub_buff[512];
    int found_empty_dir = 0;
    int empty_dir_row;
    int empty_sector;
    int filename_len;
    int it, it2, sector_id;

    /* read sector dir */
    readSector(map, 1);
    readSector(dir, 2);

    /* check free directory */
    for (it = 0; it < 16; it++)
    {
        if (dir[32*it] == 0x0)
        {
            found_empty_dir = 1;
            empty_dir_row = it;
            break;
        }
    }
    if (found_empty_dir == 0)
    {
        return;
    }
    
    /* Check if there enough space to store file*/
    for (it = 0, empty_sector = 0; it < 512 && empty_sector < *sectors; it++)
    {
        if (map[it] != 0xFF)
            empty_sector++;
    }

    if (empty_sector < *sectors)
    {
        return;
    }

    /* Clear sector that will be used to store filename */
    clear(dir[32 * empty_dir_row], 12);

    /* Put file name to diretory */
    it = 0;
    while (filename[it] != '\0' && it < 12)
    {
        dir[32 * empty_dir_row + it] = filename[it];
        it++;
    }

    /* Check free sector in the map */  
    // for (sector_id = 0, it = 0; it < 20, empty_sector > 0; sector_id++)
    for (sector_id = 0, empty_sector = 0; sector_id < 512 && empty_sector < *sectors; sector_id++)
    {
        if (map[sector_id] != 0xFF)
        {
            /* Mark sector as used */
            map[sector_id] = 0xFF;

            /* Put sector id to direcory */
            dir[32 * empty_dir_row + 12 + empty_sector] = sector_id;
            
            /* Clear temp buffer */
            clear(sub_buff, 512);

            /* Write to the empty sector */
            for (it = 0; it < 512; it++)
                sub_buff[it] = buffer[empty_sector * 512 + it];
            writeSector(sub_buff, sector_id);
            
            empty_sector++;
        }   
    }
     
    /* Write to sector */
    writeSector(map, 1);
    writeSector(dir, 2);
}

void readFile(char *buffer, char *filename, int *success)
{
    char map[512], dir[512];
    char sector_buff[512];
    int sectors[21];
    int file_found = 0;
    int file_found_id;
    int sector_id;
    int buffer_index;
    int filename_len;
    int buff_name1[12], buff_name2[12];
    int it, it2;

    /* read sector dir */
    readSector(map, 1);
    readSector(dir, 2);

    /* Find file name length */
    filename_len = 0;
    it = 0;
    while (filename[it] != '\0')
        it++;
    filename_len = it;

    /* Find file in dir */
    for (it = 0; it < 16; it++)
    {
        if (dir[32 * it] != 0x0)
        {
            file_found = 1;
            for (it2 = 0; it2 < filename_len; it2++)
            {
                if (dir[32*it + it2] != filename[it2])
                {
                    file_found = 0;
                    break;
                }
            }
        }
        if (file_found == 1)
        {
            file_found_id = it;
            break;   
        }
    }
    /* If file not found, load failed */
    if (file_found == 0)
    {
        *success = 0;
        return;
    }

    /* Load file to buffer */
    it = 1;
    buffer_index = 0;
    sector_id = dir[32*file_found_id + 12];
    while (sector_id != 0x0 && it < 20)
    {
        readSector(buffer + buffer_index, sector_id);
        sector_id = dir[32*file_found_id + 12 + it];
        buffer_index += 512;
        it++;
    }
    *success = 1;
}

void executeProgram(char *filename, int segment, int *success)
{
	char buffer[10240];
	int i;
	readFile(buffer, filename, success);
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
    readFile(buffer, "logo.txt", 0);
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