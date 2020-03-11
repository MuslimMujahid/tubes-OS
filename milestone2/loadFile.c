// loadFile.c
// Michael Black, 2007
//
// Loads a file into the file system
// This should be compiled with gcc and run outside of the OS

#include <stdio.h>

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

void readSector (char *buffer, FILE *file, int sector) {
  int i;  
  fseek(file, sector * SECTOR_SIZE, SEEK_SET);
	for (i = 0; i < SECTOR_SIZE; ++i) buffer[i] = fgetc(file);
}

void writeSector (char *buffer, FILE *file, int sector) {
  int i;
  fseek(file, sector * SECTOR_SIZE, SEEK_SET);
  for (i = 0; i < SECTOR_SIZE; ++i) fputc(buffer[i], file);
}

void clear(char *buffer, int length)
{
    int i = 0;
    for (i = 0; i < length; i++)
      buffer[i] = 0x0;
}


void main(int argc, char* argv[]) {
  int i;

  if (argc < 2) {
    printf("Specify file name to load\n");
    return;
  }

  // open the source file
  FILE* loadFil;
  loadFil = fopen(argv[1], "r");
  if (loadFil == 0) {
    printf("File not found\n");
    return;
  }

  // open the floppy image
  FILE* floppy;
  floppy = fopen("system.img", "r+");
  if (floppy == 0) {
    printf("system.img not found\n");
    return;
  }

  // load the disk map
  char map[SECTOR_SIZE];
  readSector(map, floppy, MAP_SECTOR);

  // load the files
  char files[SECTOR_SIZE * 2];
  readSector(files, floppy, FILES_SECTOR_1);
  readSector(files + SECTOR_SIZE, floppy, FILES_SECTOR_2);

  // load the sectors
  char sectors[SECTOR_SIZE];
  readSector(sectors, floppy, SECTORS_SECTOR);

  // find a free entry in the files
  for (i = 0; i < SECTOR_SIZE; i += FILES_COLUMNS)
    if (files[i] == EMPTY) break;
  if (i == SECTOR_SIZE * 2) {
    printf("Not enough room in files\n");
    return;
  }
  int fileindex = i;

  // find a free sectors entry
  for (i = 0; i < SECTOR_SIZE; i += SECTORS_COLUMNS)
    if (sectors[i] == EMPTY) break;
  if (i == SECTOR_SIZE)
  {
    printf("Not enough room in sectors\n");
    return;
  }
  int sectorindex = i;
  printf("found free sector index in %d\n", sectorindex);

  // Put files in root
  files[fileindex] = ROOT;

  // Point the file to the sector
  files[fileindex + 1] = sectorindex; 

  // clear the memory to put name
  clear(files + fileindex + NAME_OFFSET, 14);
  
  // copy the name over
  for (i = 0; i < 12; i++) {
    if (argv[1][i] == 0) break;
    files[fileindex + 2 + i] = argv[1][i];
  }

  // find free sectors and add them to the file
  int sectcount = 0;
  while (!feof(loadFil)) {
    if (sectcount == SECTORS_COLUMNS) {
      printf("Not enough space in directory entry for file\n");
      return;
    }

    // find a free map entry
    for (i = 0; i < MAX_BYTE; i++)
      if (map[i] == EMPTY) break;
    if (i == MAX_BYTE) {
      printf("Not enough room for file\n");
      return;
    }

    // mark the map entry as taken
    map[i] = USED;

    // mark the sector in the sectors entry
    sectors[sectorindex + sectcount] = i;
    sectorindex++;
    sectcount++;

    printf("Loaded %s to sector %d\n", argv[1], i);

    // move to the sector and write to it
    fseek(floppy, i * SECTOR_SIZE, SEEK_SET);
    for (i = 0; i < SECTOR_SIZE; i++) {
      if (feof(loadFil)) {
        fputc(0x0, floppy);
        break;
      } else {
        char c = fgetc(loadFil);
        fputc(c, floppy);
      }
    }
  }

  // write the map and directory back to the floppy image
  writeSector(map, floppy, MAP_SECTOR);
  writeSector(files, floppy, FILES_SECTOR_1);
  writeSector(files + SECTOR_SIZE, floppy, FILES_SECTOR_2);
  writeSector(sectors, floppy, SECTORS_SECTOR);

  fclose(floppy);
  fclose(loadFil);
}
