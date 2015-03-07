#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define MAX_LEN 128
#define TABLE_SIZE 101
typedef struct cell{
	char key[MAX_LEN];
	int value;
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

Cell * get(char * key)
{
	Cell * p;
	int i = hash(key);
   	for(p=&table[i]; p!=NULL; p=p->next)
		if(0 == strcmp(key, p->key))
			return p;
	return NULL;	
}

void put(char * key, int value)
{
	int i;
	Cell * cur, *next, *p;
	if(get(key)) return;
	else
	{
		i = hash(key);
		if(table[i].key[0] == '\0')
		{
			strcpy(table[i].key, key);
			table[i].value = value;
		}
		else{
			cur = &table[i];
			while(next = cur->next) cur = next; 
			//add new cell
			Cell * p = (Cell *)malloc(sizeof(Cell));
		   	memset(p, 0, sizeof(Cell));
			strcpy(p->key, key);
			p->value = value;
			cur->next = p;
		}	
	}
}

int main()
{
	int n1=5, n2=8;
	int i=0;
	char *strs[] = {"aa", "bb", "cc", "dd", "ee", "robin1001", "susan", "linzi"};
	Cell *p;
	for(i=0; i<n1; i++)
		put(strs[i], i);
	for(i=0; i<n2; i++)
	{
		if(p = get(strs[i]))  printf("%s\t%d\t%d\n", p->key, p->value, hash(strs[i]));
		else printf("%s\tnone\t%d\n", strs[i], hash(strs[i]));
	}	
	return 0;
}

