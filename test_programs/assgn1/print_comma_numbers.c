#include <stdio.h>
#include <stdlib.h>

int main()
{
	char str [14];
	double num;
	char type;
	int temp;
	
	type='-';
	printf("Enter Number : ");
	scanf("%lf",&num);
	printf("Number you entered : %lf",num);
	if(num > 999999) {
		if(type == '+')
			sprintf(str,"  ?,???,???.??");
		else
			sprintf(str,"(?,???,???.?\?)");
	}
	else if(num > 999) {
		temp = (int)(num/1000);
		if(type == '+')
			sprintf(str,"%6d,%5.2f",temp,(num-(temp*1000)));
		else
			sprintf(str,"(%4d,%5.2f)",temp,(num-(temp*1000)));
	}
	else {
		if(type == '+')
			sprintf(str,"%14.2f",num);
		else
			sprintf(str,"(%12.2f)",num);
	}
	printf("\n\t\t*****\n\n The comma string is : %s\n\n\t\t*****\n\n",str);
}
