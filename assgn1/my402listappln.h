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
extern void insertObj(My402List*, My402ApplnObj*);
extern void convertTimeFormats(long long , char *);

#endif // _MY402LISTAPPLN_H
