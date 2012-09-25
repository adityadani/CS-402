/* Author: Aditya Dani
   adityada@usc.edu
*/

#ifndef _MY402LISTAPPLN_H
#define _MY402LISTAPPLN_H

#include "cs402.h"

#define PrintChar(num,chr)	     \
	for(i=0;i<num;i++) { \
		printf("%c",chr);			\
	}

typedef struct tagMy402ApplnObj {
	char transType;
	int transAmountCents;
	char transDesc[25];
	char transTime[15];
	long long transTimeInt;
}My402ApplnObj;

extern My402ApplnObj* createApplnObj (char*);
extern int insertObj(My402List*, My402ApplnObj*);
extern int convertTimeFormats(long long , char *);
extern void printPattern();
extern char * formatNumerics(char, double, char *);
extern void printObj(My402List *);


#endif // _MY402LISTAPPLN_H
