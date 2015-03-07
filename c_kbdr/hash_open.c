#include <stdio.h>
#include <malloc.h>
#include <string.h>


typedef unsigned char bool;
#define true 1
#define false 0
/* 	author: 	robin1001
 *  date:       2014.07.22    
 */
#define MAX_LEN 128
#define TABLE_SIZE 101
typedef struct cell{
	char key[MAX_LEN];
	int value;
	bool exist;
	struct cell * next;
}Cell;

Cell table[TABLE_SIZE] = {0};


int hash(char * src)
{
	int val=0;
	for(; *src; src++)
		val = val * 31 + *src;
	return val % TABLE_SIZE;
}

int hash2(char * src)
{
	int val=0;
	for(; *src; src++)
		val = val * 13 + *src;
	return val % TABLE_SIZE;
}

int get(char * key)
{
	int i=1;
	int h = hash(key);
	int pos = h;
	while(table[pos].exist && i<TABLE_SIZE)
	{
		if(0 == strcmp(key, table[pos].key))
			return pos;
		pos = h + i * hash2(key);
	   	i++;	
	}
	return -1;	
}

void put(char * key, int value)
{
	int i, h, pos, count;
	if(get(key) >= 0) return;
	else
	{
		pos = h = hash(key);
		i = 1;
		while(table[pos].exist && i<TABLE_SIZE)
		{
			pos = h + i * hash2(key);
			i++;	
		}
		if(i < TABLE_SIZE)
		{
			strcpy(table[pos].key, key);
			table[pos].value = value;
			table[pos].exist = true;
		}	
	}
}

int main()
{
	int n1=5, n2=8;
	int i=0;
	int pos;
	char *strs[] = {"aa", "bb", "cc", "dd", "ee", "robin1001", "susan", "linzi"};
	memset(table, 0, sizeof(table));

	for(i=0; i<n1; i++)
		put(strs[i], i);
	for(i=0; i<n2; i++)
	{
		if((pos = get(strs[i])) >= 0)  printf("%s\t%d\t%d\t%d\n", table[pos].key, table[pos].value, hash(strs[i]),hash2(strs[i]));
		else printf("%s\tnone\t%d\t%d\n", strs[i], hash(strs[i]),hash2(strs[i]));
	}	
	return 0;
}

