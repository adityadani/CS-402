#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main()
{
	struct timeval tv1,tv2;
	long tie;

	gettimeofday(&tv1,NULL);
	usleep(1231231);
	gettimeofday(&tv2,NULL);
	tie = ((tv2.tv_sec*1000000) + (tv2.tv_usec)) - ((tv1.tv_sec*1000000) + tv1.tv_usec);
	printf("\nTime Difference : %f ",(double)tie/1000);
}
