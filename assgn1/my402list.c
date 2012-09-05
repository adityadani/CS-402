/* Author: Aditya Dani
   adityada@usc.edu
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "my402list.h"

int  My402ListLength(My402List *aList)
{
	return (aList->num_members);
}

int  My402ListEmpty(My402List *aList)
{
	if(aList->num_members == 0) { //This test is sufficient to check empty list.
		if (aList->anchor.next == &(aList->anchor)) {
			if(aList->anchor.prev == &(aList->anchor))
				return 0;
			else
				return 1; //Failing in this test not possible.
		}
		else
			return 1; //Failing in this test not possible.
	}
	else
		return 1; // There are elements present in the list.
}

int  My402ListPrepend(My402List *aList, void *aObj)
{
	My402ListElem *anchorNext, *newElem;
	
	anchorNext = aList->anchor.next;
	newElem = (My402ListElem *)malloc(sizeof(struct tagMy402ListElem));
	if (newElem == NULL) {
		printf("Error in handling malloc(). Error no = %d",errno);
		return 1; // returning FALSE
	}
	newElem->obj = aObj;
	newElem->prev = &(aList->anchor);
	newElem->next = anchorNext;
	aList->anchor.next = newElem;
	anchorNext->prev = newElem;
	aList->num_members++;

	return 0;
}

int  My402ListAppend(My402List *aList, void *aObj)
{
	My402ListElem *anchorPrev, *newElem;

	anchorPrev = aList->anchor.prev;
	newElem = (My402ListElem *)malloc(sizeof(struct tagMy402ListElem));
	if (newElem == NULL) {
		printf("Error in handling malloc(). Error no = %d",errno);
		return 1; // returning FALSE
	}
	newElem->obj = aObj;
	newElem->prev = anchorPrev;
	newElem->next = &(aList->anchor);
	aList->anchor.prev = newElem;
	anchorPrev->next = newElem;
	aList->num_members++;

	return 0;
	
}

void My402ListUnlink(My402List *aList, My402ListElem *aElem)
{
	My402ListElem *elemNext, *elemPrev;

	elemNext = aElem->next;
	elemPrev = aElem->prev;
	elemPrev->next = elemNext;
	elemNext->prev = elemPrev;
	aList->num_members--;
	free(aElem);
	
}

void My402ListUnlinkAll(My402List *aList)
{
	My402ListElem *elemPtr;

	elemPtr = My402ListNext(aList,&(aList->anchor));
	while(elemPtr != NULL) {
		My402ListUnlink(aList, elemPtr);
		elemPtr = My402ListNext(aList,&(aList->anchor));
	}
}

int  My402ListInsertAfter(My402List *aList, void *aObj, My402ListElem *aElem)
{
	My402ListElem *elemNext, *newElem;

	elemNext = aElem->next;
	
	if (aElem == NULL)
		My402ListAppend(aList, aObj);
	else {
		newElem = (My402ListElem *)malloc(sizeof(struct tagMy402ListElem));
		if (newElem == NULL) {
			printf("Error in handling malloc(). Error no = %d",errno);
			return 1; // returning FALSE
		}
		newElem->obj = aObj;
		elemNext->prev = newElem;
		aElem->next = newElem;
		newElem->prev = aElem;
		newElem->next = elemNext;
		aList->num_members++;
	}

	return 0; // returning TRUE
}

int  My402ListInsertBefore(My402List *aList, void *aObj, My402ListElem *aElem)
{
	My402ListElem *elemPrev, *newElem;

	elemPrev = aElem->prev;
	
	if (aElem == NULL)
		My402ListPrepend(aList, aObj);
	else {
		newElem = (My402ListElem *)malloc(sizeof(struct tagMy402ListElem));
		if (newElem == NULL) {
			printf("Error in handling malloc(). Error no = %d",errno);
			return 1; // returning FALSE
		}
		newElem->obj = aObj;
		elemPrev->next = newElem;
		aElem->prev = newElem;
		newElem->next = aElem;
		newElem->prev = elemPrev;
		aList->num_members++;
	}

	return 0; // returning TRUE
	
}

My402ListElem *My402ListFirst(My402List *aList)
{
	return (aList->anchor.next);
}

My402ListElem *My402ListLast(My402List *aList)
{
	return (aList->anchor.prev);
}

My402ListElem *My402ListNext(My402List *aList , My402ListElem *aElem)
{
	if(aElem->next == &(aList->anchor))
		return NULL;
	else
		return (aElem->next);
}
My402ListElem *My402ListPrev(My402List *aList, My402ListElem *aElem)
{
	if(aElem->prev == &(aList->anchor))
		return NULL;
	else
		return (aElem->prev);
}

My402ListElem *My402ListFind(My402List *aList, void *aObj)
{
	My402ListElem *elemPtr;

	elemPtr = &(aList->anchor);
	while (elemPtr != NULL) {
		if (elemPtr->obj == aObj)
			return (elemPtr);
		My402ListNext(aList,elemPtr);
	}
	return NULL;
}

int My402ListInit(My402List *aList)
{
	if (aList != NULL) {
		aList->num_members=0;
		aList->anchor.next=&(aList->anchor);
		aList->anchor.prev=&(aList->anchor);
		return 0;
	}
	else
		return 1;
}



