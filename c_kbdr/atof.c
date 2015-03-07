#include <stdio.h>
#include <ctype.h>

#define N 1000
double atof(char *str)
{
	int i=0, sign=1, val, power=1;
	if(str[0] == '-') sign=-1;
   	if(str[0] == '-' || str[0] == '+') i++;
	for(val=0; isdigit(str[i]); i++)
	   val = val*10 + (str[i]-'0');
	if(str[i] == '.') i++; 
	for(power=1; isdigit(str[i]); i++)
	{
	   val = val*10 + (str[i]-'0');
	   power = power*10;
	}	
	return (double)sign*val/power;
}


int main()
{
	char arr[N]={0};
	scanf("%s", arr);
	printf("%lf\n", atof(arr));
	return 0;
}
