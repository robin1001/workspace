#include <stdio.h>

int _find(char *src, char *f)
{
	char *p=src, *q=f;
	while(*p && *q && *p++==*q++);
	if(*q) 
		return false;
	else return true;
}

int findstr(char *src, char * f)
{
	for(src; *src; src++)
		if(_find(src, f))
			return true;
	return false;
}


int main()
{
	printf("%d\n", findstr("helloworld", "yes"));
}
