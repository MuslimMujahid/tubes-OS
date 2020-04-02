#ifndef TEXT_H
#define TEXT_H

#define TRUE 1
#define FALSE 0

void pS(char* string, int newline);
void pC(char c, int newline);
void pI(int i, int newLine); 
int len(char* string);
void clear(char *buffer, int length); 
void copy(char* src, char* dest);
void copyRange(char* src, char* dest, int l, int h);
int strCmp(char* str1, char* str2);
int strSubset(char* str1, char* str2);
void concat(char* src, char* dest);

#endif