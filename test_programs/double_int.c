#include <stdio.h>

int main()
{
	double a=4319.94;
	int b;
	double c=1077.07;
	int d;
	b=a*100;
	/*	if((double)b/100 == a)
		printf("Yes");
	else {
		printf("\nNo");
		b++;
		}*/
	d=c*100;
	/*
	if((double)d/100 == c)
		printf("Yes");
	else {
		printf("\nNo");
	        d++;
		}*/

	printf("\nValue of double : %f",a);
	printf("\n Value of int : %d\n",b);
	printf("\nValue of double : %f",c);
	printf("\n Value of int : %d\n",d);

}
