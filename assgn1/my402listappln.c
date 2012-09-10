/* Author: Aditya Dani
   adityada@usc.edu
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "my402list.h"
#include "my402listappln.h"

void convertTimeFormats(long long transTimeInt , char *transTime)
{
	time_t tempTime;
	char *startPtr, *spacePtr, *tempStr;
	int i=0;
	char space[2] = {' ','\0'};
	
	tempTime = transTimeInt;
	tempStr = (char *)malloc(strlen(ctime(&tempTime)) + 1);
	strncpy(tempStr,ctime(&tempTime),strlen(ctime(&tempTime)));
	tempStr[strlen(ctime(&tempTime)) - 1]='\0'; // The ctime function also returns a '\n'
	// character at its end hence overwriting it with '\0'. Check on nunki

	spacePtr = tempStr;
	while(spacePtr != NULL) {
		startPtr = spacePtr;
		spacePtr = strchr(startPtr,' ');
		if(spacePtr == NULL)
			strncat(transTime,startPtr,strlen(startPtr));
		else {
			*spacePtr++=0;
			// If the date is single digit then there is an extra space.
			// Hence the spacePtr is advanced until all the spaces are consumed/
			// This while loop is basically to handle these dates Apr 6 , or Apr 9 and not Apr 12
			while((*spacePtr)==' ') {
				spacePtr++;
				strncat(transTime," ",1);
			}
			if (i != 3) {
				strncat(transTime,startPtr,strlen(startPtr));
				strncat(transTime,space,2);
			}
			i++;
		}
	}
}


My402ApplnObj* createApplnObj (char *buf)
{
	char *startPtr, *tabPtr;
	char *newlinePtr;
	int i=0;
	double tempAmount;
	My402ApplnObj *applnObj;
	
	applnObj = (My402ApplnObj *)malloc(sizeof(struct tagMy402ApplnObj));
	// This part of code adopted from Prof Cheng's
	// Lecture No. 3 - Slides 'Parsing Text Input'

	tabPtr = buf;
	while(tabPtr != NULL) {
		startPtr = tabPtr;
		tabPtr = strchr(startPtr,'\t');
		if(tabPtr == NULL) {
			strncpy(applnObj->transDesc,startPtr,23);
			applnObj->transDesc[23]='\0';
			// Checking if /n character is present
			// If present overwritting it with \0
			newlinePtr = strchr(applnObj->transDesc,'\n');
			if(newlinePtr!=NULL)
				*newlinePtr=0;
			break;
		}
		*tabPtr++=0;
		switch(i) {
		case 0:
			if(strlen(startPtr)!=1) {
				fprintf(stderr,"Error in tfile format");				
			}
			else
				applnObj->transType = startPtr[0];
			break;	
		case 1:
			if(strlen(startPtr)!=10) {
				fprintf(stderr, "Error in tfile format");
			}
			else{
				applnObj->transTimeInt = atoll(startPtr);
				convertTimeFormats(applnObj->transTimeInt,applnObj->transTime);
			}
			break;

		case 2:
			tempAmount = strtod(startPtr,NULL);
			applnObj->transAmountCents = (int)(tempAmount * 100);
			break;
			
		}
		i++;
	}
	return applnObj;
}

void printPattern()
{
	int i;
	printf("+");
	PrintChar(17,'-');
	printf("+");
	PrintChar(26,'-');
	printf("+");
	PrintChar(16,'-');
	printf("+");
	PrintChar(16,'-');
	printf("+");	
	printf("\n");
}

void printObj(My402List *applnList)
{
	My402ListElem *ptrElem;
	My402ApplnObj *applnObj;
	int Balance=0;
	int i; //This i is required for the macro PrintChar
	ptrElem = My402ListFirst(applnList);
	printf("\n\n\t\t **** LIST OUTPUT **** \n\n");
	printPattern();
	printf("|       Date      |       Description        |     Amount     |     Balance    |\n");
	printPattern();
	do {
		applnObj = (struct tagMy402ApplnObj *)ptrElem->obj;
		printf("| %s |",applnObj->transTime);
		printf(" %s",applnObj->transDesc);
		PrintChar(24-strlen(applnObj->transDesc),' ');
		printf(" |");
		if(applnObj->transType == '+') {
			printf(" %14.2f |",(double)(applnObj->transAmountCents) / 100);
			Balance += applnObj->transAmountCents;
		}
		else {
			printf(" (%12.2f) |",(double)(applnObj->transAmountCents) / 100);
			Balance -= applnObj->transAmountCents;
		}
		if(Balance > 0)
			printf(" %14.2f |\n",(double)(Balance) / 100);
		else
			printf(" (%12.2f) |\n",(double)(Balance) / 100);
	}while((ptrElem = My402ListNext(applnList,ptrElem))!=NULL);
	printPattern();
}

void insertObj(My402List *applnList, My402ApplnObj *applnObj)
{
	My402ListElem *ptrElem;
	My402ApplnObj *tempObj;
	ptrElem = My402ListFirst(applnList);
	if(ptrElem == NULL)
		My402ListPrepend(applnList,applnObj);
	else {
		while(1) {
			tempObj = (struct tagMy402ApplnObj *)ptrElem->obj;
			if(applnObj->transTimeInt >= tempObj->transTimeInt) {
				ptrElem = My402ListNext(applnList , ptrElem);
				if (ptrElem == NULL) {
					My402ListAppend(applnList , applnObj);
					break;
				}
			}
			else {
				My402ListInsertBefore(applnList ,applnObj ,  ptrElem);
				break;
			}
		}
	}
}

int main(int argc, char **argv)
{
	FILE *fp;
	char buf[1026];
	My402ApplnObj *applnObj;
	My402List applnList;

	
	My402ListInit(&applnList);
	if (argc == 1) {
		fprintf(stderr,"Invalid Command!!\n");
		fprintf(stderr,"Usage: ./applnlist sort [tfile name]\n");
		exit(1);
	}
	else{
		if(strncmp(argv[1],"sort",4)!=0) {
			fprintf(stderr,"Invalid Command!!\n");
			fprintf(stderr,"Usage: ./applnlist sort [tfile name]\n");
			exit(1);			
		}
		if(argc==2)
			fp=stdin;
		else
			fp=fopen(argv[2],"r");
	
		while(fgets(buf, sizeof(buf) ,fp)!=NULL) {
			applnObj = createApplnObj(buf);
			insertObj(&applnList,applnObj);
		}
		printObj(&applnList);
	}
	return 1;
}
