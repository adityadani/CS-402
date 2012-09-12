#include <stdio.h>
#include <time.h>
#include <string.h>



int main()
{
	long long time_input;
	char buf[200];
	time_t tm;
	char *ptr;
	
	printf("\nEnter the time value\n");
	scanf("%lld",&time_input);
	
	tm = time_input;
	strncpy(buf,ctime(&tm),strlen(ctime(&tm)));
	buf[strlen(ctime(&tm))]='\0';

	
	printf("Time is : %s",buf);
}
