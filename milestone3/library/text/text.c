#include "text.h"
#include "../mat/mat.h"

// void Input(char* string, char* ret)
// {
//     int count = len(string);
//     char input;
//     while(1)
//     {
//         /* Call interrupt 0x16 */
//         /* interrupt #, AX, BX, CX, DX */
//         input = interrupt(0x16,0,0,0,0);
//         if (input == '\r')
//         {              
//             *ret = '\r';
//             string[count] = 0x0;
//             interrupt(0x10, 0xe*256+'\n', 0, 0, 0);
//             interrupt(0x10, 0xe*256+'\r', 0, 0, 0);
//             return;
//         }
//         else if (input == '\b')
//         {
//             if (count > 0)
//             {
//                 string[count] = 0x0;
//                 count--;
//                 interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
//                 count++;
//                 interrupt(0x10, 0xe*256+0x0, 0, 0 ,0);
//                 count--;
//                 string[count] = 0x0;
//                 interrupt(0x10, 0xe*256+0x8, 0, 0, 0);
//             }
//         }
//         else if (input == '\x20') // space
//         {
//             string[count] = ' ';
//             interrupt(0x10, 0xe*256+input, 0, 0, 0);
//             count++;
//         }
//         else if (input == '\t')
//         {
//             *ret = '\t';
//             return;
//         }
//         else if (input == 0) // arrow up and down
//         {
//             *ret = '\f';
//             string[count] = input;
//             string[count+1] = 0x0;
//             return;
//         }
//         else
//         {
//             string[count] = input;
//             interrupt(0x10, 0xe*256+input, 0, 0, 0);
//             count++;
//         }     
//     }
// }

void pS(char* string, int newline)
{
    char ch;
    int i = 0;
    while(string[i] != '\0')
    {
        ch = string[i];
        if (ch == '\n') pC('\r', FALSE);
        pC(ch, FALSE);
        i++;
    }
    if (newline) {
		    pC('\r', TRUE);
	  }
}

void pC(char c, int newline)
{
    interrupt(0x10, 0xE00+c, 0, 0, 0);
    if (newline) {
		    interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
		    interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
	  }
}

void pI(int i, int newLine)
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
	  pS(integer + 9 - digit, newLine);
}

int len(char* string)
{
    int i = 0;
    while (string[i] != '\0') i++;
    return i;
}

int lenMax(char* string, int max)
{
    int i = 0;
    while (string[i] != '\0' && i < max) i++;
    return i;
}

void clear(char *buffer, int length)
{
    int i = 0;
    for (i = 0; i < length; i++)
        buffer[i] = 0x0;
}

void copy(char* src, char* dest)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void copyMax(char* src, char* dest, int max)
{
    int i = 0;
    while (src[i] != '\0' && i < max)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0'; 
}

void copyRange(char* src, char* dest, int l, int h)
{
    int i, j;

    i = 0;
    while (src[l + i] != '\0' && i <= h-l)
    {
        dest[i] = src[l+i];
        i++;
    }
    dest[l+i] = '\0';
}

int strCmp(char* str1, char* str2)
{
    int i;
    int length = len(str1);

    if (length != len(str2)){
        return FALSE;
    }
    else
    {
        for (i = 0; i < length; i++)
        {
            if (str1[i] != str2[i]){
                break;
            }
        }
    }
    return (str1[i] == str2[i]);
}

int strCmpMax(char* str1, char* str2, int max)
{
    int i;
    int length = len(str1);

    if (length != len(str2)){
        return FALSE;
    }
    else
    {
        for (i = 0; i < length && i < max; i++)
        {
            if (str1[i] != str2[i]){
                break;
            }
        }
    }
    if (i == max) i--;
    return (str1[i] == str2[i]);
}

int strSubset(char* str1, char* str2)
{
    int i;
    int length;;

    length = len(str1);
    for (i = 0; i < length; i++)
    {
        if (str1[i] != str2[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}

void concat(char* src, char* dest)
{
    int i, j;

    i = 0;
    j = 0;
    while (src[i] != '\0') i++;
    while (dest[j] != '\0')
    {
        src[i + j] = dest[j];
        j++;
    }
}
