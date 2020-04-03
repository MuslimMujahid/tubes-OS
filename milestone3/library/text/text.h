#ifndef TEXT_H
#define TEXT_H

#define TRUE 1
#define FALSE 0

// Input
// void Input(char* buffer, char* ret);

// Output
void pS(char* string, int newline);
void pC(char c, int newline);
void pI(int i, int newLine); 

int len(char* string);
int lenMax(char* string, int max);
void clear(char *buffer, int length); 
void copy(char* src, char* dest);
void copyMax(char* src, char* dest, int max);
void copyRange(char* src, char* dest, int l, int h);
int strCmp(char* str1, char* str2);
int strCmpMax(char* str1, char* str2, int max);
int strSubset(char* str1, char* str2);
void concat(char* src, char* dest);

#endif