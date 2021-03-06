/* Author: Aditya Dani
   adityada@usc.edu
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include "my402list.h"
#include "my402listappln.h"

int convertTimeFormats(long long transTimeInt , char *transTime)
{
	time_t tempTime;
	long current_time;
	char *startPtr, *spacePtr, *tempStr;
	int i=0;
	char space[2] = {' ','\0'};
	
	current_time=time(NULL);
	if(current_time < transTimeInt) {
		fprintf(stderr,"\nFuture time detected\n");
		return 1;
	}
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
			if (i != 3) {
				strncat(transTime,startPtr,strlen(startPtr));
				strncat(transTime,space,2);

			// If the date is single digit then there is an extra space.
			// Hence the spacePtr is advanced until all the spaces are consumed/
			// This while loop is basically to handle these dates Apr 6 , or Apr 9 and not Apr 12

				if((*spacePtr)==' ') {
					spacePtr++;
					strncat(transTime," ",1);
				}
				
			}
			i++;
		}
	}
	free(tempStr);
	return 0;
}


My402ApplnObj* createApplnObj (char *buf)
{
	char *startPtr, *tabPtr;
	char *newlinePtr, *decimalPtr;
	int i=0;
	double tempAmount;
	My402ApplnObj *applnObj;
	int my_error_num=0;
	int time_ret=0;
	
	
	applnObj = (My402ApplnObj *)malloc(sizeof(struct tagMy402ApplnObj));
	tabPtr = buf;

	// This part of code adopted from Prof Cheng's
	// Lecture No. 3 - Slides 'Parsing Text Input'

	while(tabPtr != NULL) {
		if(i>=4) {
			fprintf(stderr,"\nToo many fields");
			fprintf(stderr,"\nError in tfile format\n");
			free(applnObj);
			return NULL;			
		}
		startPtr = tabPtr;
		tabPtr = strchr(startPtr,'\t');
		if(tabPtr == NULL) {			
			strncpy(applnObj->transDesc,startPtr,24);
			applnObj->transDesc[24]='\0';
			if(strlen(applnObj->transDesc)==1) {
				fprintf(stderr,"\nDescription field cannot be empty!");
				fprintf(stderr,"\nError in tfile [Description Field] format\n");
				free(applnObj);
				return NULL;
			}

			// Checking if /n character is present
			// If present overwritting it with \0
			newlinePtr = strchr(applnObj->transDesc,'\n');			
			if(newlinePtr!=NULL)
				*newlinePtr=0;
			break;
		}
		*tabPtr++=0;

       // Adopted code part ends		

		switch(i) {
		case 0:
			if(strlen(startPtr)!=1) {
				fprintf(stderr,"\nError in tfile [transaction type] format\n");
				free(applnObj);
				return NULL;
			}
			if(startPtr[0]!='+' && startPtr[0]!='-') {
				fprintf(stderr,"\nError in tfile [transaction type] format\n");
				free(applnObj);
				return NULL;
			}
			applnObj->transType = startPtr[0];
			break;	
		case 1:
			if(strlen(startPtr)>10) {
				fprintf(stderr, "\nError in tfile [time stamp] format\n");
				free(applnObj);
				return NULL;
			}
			else{
				applnObj->transTimeInt = atoll(startPtr);
				time_ret=convertTimeFormats(applnObj->transTimeInt,applnObj->transTime);
				if(time_ret==1) {
					free(applnObj);
					return NULL;
				}
			}
			break;

		case 2:
			if(strlen(startPtr) > 10) {
				fprintf(stderr, "\nError in tfile [amount field] format\n");
				free(applnObj);
				return NULL;
			}
			decimalPtr=strchr(startPtr,'.');
			if(strlen(decimalPtr) > 3){
				printf("\nDecimal : %s",decimalPtr);
				fprintf(stderr,"\nError in tfile [amount field] format\n");
				free(applnObj);
				return NULL;
			}
			tempAmount = strtod(startPtr,NULL);
			my_error_num=errno;
			if(my_error_num!=0) {
				fprintf(stderr, "\nError in the amount field\n");
				fprintf(stderr, "\nAmount field not a number Error Num = %d\n",my_error_num);
				free(applnObj);
				return NULL;
			}
			applnObj->transAmountCents = (tempAmount * 100);

			// This part solves the penny problem.
			// For values like 9413.94 when multiplied by 100 and stored in int
			// they get stored as 941393 thus reducing the value by 1. Also some numbers like
			// 1077.07 get stored correctly as 107707. Hence the if condition and the extra
			// processing.

			if((double)applnObj->transAmountCents/100 != tempAmount)
				applnObj->transAmountCents++;
			break;			
		}
		i++;
	}
	if(i!=3) {
		fprintf(stderr,"\nError in [tfile] format\n");
		free(applnObj);
		return NULL;
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

char * formatNumerics(char type, double num, char str[14])
{
	int temp_th, temp_hu, temp_dc;
	
	if(num > 99999999) {
		if(type == '+')
			sprintf(str," ?,???,???.?? ");
		else
			sprintf(str,"(?,???,???.?\?)");
	}
	else if(num > 99999) {
		temp_th = (int)(num/100000);
		temp_hu = (int)((num - (temp_th*100000))/100);
		temp_dc = (int)((num - (temp_th*100000))-(temp_hu*100));
		
		if(type == '+')
			sprintf(str,"%6d,%03d.%02d ",temp_th,temp_hu,temp_dc);
		else {
			sprintf(str,"(%5d,%03d.%02d)",temp_th,temp_hu,temp_dc);
		}
	}
	else {
		if(type == '+')
			sprintf(str,"%13.2f ",(double)(num/100));
		else
			sprintf(str,"(%12.2f)",(double)(num/100));
	}
	return str;
}

void printObj(My402List *applnList)
{
	My402ListElem *ptrElem;
	My402ApplnObj *applnObj;
	int Balance=0;
	char str[14];
	int i; //This i is required for the macro PrintChar
	ptrElem = My402ListFirst(applnList);
	printPattern();
	printf("|       Date      | Description              |         Amount |        Balance |\n");
	printPattern();
	do {
		applnObj = (struct tagMy402ApplnObj *)ptrElem->obj;
		printf("| %s |",applnObj->transTime);
		printf(" %s",applnObj->transDesc);
		PrintChar(24-strlen(applnObj->transDesc),' ');
		printf(" |");
		if(applnObj->transType == '+') {
			printf(" %s |",formatNumerics('+', (double)(applnObj->transAmountCents), str));
			Balance += applnObj->transAmountCents;
		}
		else {
			printf(" %s |",formatNumerics('-', (double)(applnObj->transAmountCents), str));
			Balance -= applnObj->transAmountCents;
		}
		if(Balance > 0)
			printf(" %s |\n",formatNumerics('+', (double)(Balance), str));
		else
			printf(" %s |\n",formatNumerics('-', (double)(-1*Balance), str));


	}while((ptrElem = My402ListNext(applnList,ptrElem))!=NULL);
	printPattern();
}

int insertObj(My402List *applnList, My402ApplnObj *applnObj)
{
	My402ListElem *ptrElem;
	My402ApplnObj *tempObj;

	// Using the insertion sort logic.

	ptrElem = My402ListFirst(applnList);
	if(ptrElem == NULL)
		My402ListPrepend(applnList,applnObj);
	else {
		while(1) {
			tempObj = (struct tagMy402ApplnObj *)ptrElem->obj;
			if(applnObj->transTimeInt > tempObj->transTimeInt) {
				ptrElem = My402ListNext(applnList , ptrElem);
				if (ptrElem == NULL) {
					My402ListAppend(applnList , applnObj);
					break;
				}
			}
			else if(applnObj->transTimeInt == tempObj->transTimeInt) {
				fprintf(stderr,"\nDuplicate timestamps!\n");
				return 1;
			}
			else {
				My402ListInsertBefore(applnList ,applnObj ,  ptrElem);
				break;
			}
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	FILE *fp;
	char buf[1026];
	My402ApplnObj *applnObj;
	My402List applnList;
	int my_error_num=0, ret_insert=0;
	struct stat statbuf;

	
	My402ListInit(&applnList);
	if (argc == 1 || argc > 4) {
		fprintf(stderr,"Invalid Command!!\n");
		fprintf(stderr,"Usage: ./warmup1 sort [tfile name]\n");
		exit(1);
	}
	else{
		if(strncmp(argv[1],"sort",4)!=0) {
			fprintf(stderr,"Invalid Command!!\n");
			fprintf(stderr,"Usage: ./warmup1 sort [tfile name]\n");
			exit(1);			
		}
		if(argc==2)
			fp=stdin;
		else{
			stat(argv[2],&statbuf);
			if(S_ISDIR(statbuf.st_mode)==1) {
				fprintf(stderr,"\n Input %s is a directory.\n",argv[2]);
				fprintf(stderr,"Usage: ./warmup1 sort [tfile name]\n");
				exit(1);
			}
			fp=fopen(argv[2],"r");
			my_error_num=errno;
			if(fp==NULL) {
				fprintf(stderr,"\nError in opening file: %s ",argv[2]);
				fprintf(stderr,"\nError : %s\n",strerror(my_error_num));
				exit(1);
			}
		}
	
		while(fgets(buf, sizeof(buf) ,fp)!=NULL) {
			applnObj = createApplnObj(buf);
			if(applnObj==NULL) {
				if(fp!=stdin)
					fclose(fp);
				fprintf(stderr,"Exiting!!!\n");
				exit(1);
			}
			ret_insert=insertObj(&applnList,applnObj);
			if(ret_insert!=0) {
				if(fp!=stdin)
					fclose(fp);
				fprintf(stderr,"Exiting!!!\n");
				exit(1);
			}
		}
		printObj(&applnList);
	}
	if(fp!=stdin)
		fclose(fp);
	return 0;
}
