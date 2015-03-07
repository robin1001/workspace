#include <stdio.h>

#define N 1000
int match(char *pat, char *src)
{
	if(*pat == '\0') return 1;
	if(*src == '\0') return 0;
	if(*src == *pat && match(pat+1, src+1)>0) return 1;
	else return match(pat, src+1);
}

int main(int argc, char **argv)
{
	char line[N]={0};
	if(argc != 2)
	{
		printf("Usage: grep str; example: grep \"robin1001\"\n");
		return -1;
	}
	while(gets(line))
	{
		if(match(argv[1], line))
			printf("%s\n", line);
	}

	return 0;
}

