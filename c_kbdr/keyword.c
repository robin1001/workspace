#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 128

char token[MAX_LEN];

typedef struct word_info{
	char *word;
    int count;
}Words;

Words keywords[]={
	{"char", 0},
	{"double", 0},
	{"else", 0},
	{"float", 0},
	{"for", 0},
	{"if", 0},
	{"int",0},
	{"while", 0}
};

#define NKEY (sizeof(keywords) / sizeof(keywords[0]))

int bin_search(char * word, int len)
{
	int mid, n;
	int left=0, right=len;
   	while(left <= right)
	{
		mid = (left+right)/2;
		if((n=strcmp(word, keywords[mid].word)) > 0)
			left = mid + 1;
		else if(n < 0)
			right = mid -1;
		else
			return mid;
	}	
	return -1;
}

int main()
{
	int pos, i;
	printf("%d\n", NKEY);
	while(scanf("%s", token) != EOF)
		if((pos = bin_search(token, NKEY)) >= 0)
			keywords[pos].count++;

	for(i=0; i<NKEY; i++)
		printf("%s\t%d\n", keywords[i].word, keywords[i].count);

	return 0;
}

