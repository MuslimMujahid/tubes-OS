#include "text.h"
#include "../mat/mat.h"

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