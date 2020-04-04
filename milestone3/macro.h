#ifndef MACRO_H
#define MACRO_H

#define TRUE 1
#define FALSE 0
#define SEPARATOR ' '
#define PATH_DIVIDER '/'
#define ARROW '\f'

#define SECTOR_SIZE 512
#define MAP_SECTOR 256
#define FILES_SECTOR_1 257
#define FILES_SECTOR_2 258
#define SECTORS_SECTOR 259
#define ARGS_SECTOR 512
#define MAX_BYTE 256
#define MAX_FILES 32
#define FILE_SIZE 8192
#define NAME_OFFSET 2
#define SECTORS_COLUMNS 16
#define FILES_COLUMNS 16
#define MAX_FILENAME_LENGTH 14
#define ROOT 0xFF
#define USED 0xFF
#define EMPTY 0x0
#define DIR 0xFF
#define BIN_INDEX 0x0
#define HISTORY_LENGTH 64
#define ARGC_LENGTH 10
#define ARGV_LENGTH 64

// File exception
#define FILE_NOT_FOUND -1
#define FILE_EXIST -1
#define FILES_FULL -2
#define SECTORS_FULL -3
#define FOLDER_UNVALID -4

#define ROOT 0xFF
#define USED 0xFF
#define EMPTY 0x0
#define DIR 0xFF

// command type
#define cd 1
#define ls 2
#define mkdir 3
#define run 4
#define bin 5
#define cat 6 
#define cp 7
#define rm 8
#define mv 9

#endif