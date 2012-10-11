#include <stdio.h>
#include <stdlib.h>

int main()
{
	char a[6] = {'2' , '3' , '.' , '5' , '6' , '\0'};
	double b;

	b=strtod(a,NULL);
	printf("\nThe value is %f  %s \n",b,a);
}
