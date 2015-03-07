#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define HASHSIZE 100

struct nlist{
	struct nlist * next;
	char *name;
	char *defn;
};

static struct nlist *hashtab[HASHSIZE];

unsigned hash(char *s)
{
	unsigned hashval;
	for(hashval = 0; *s; s++)
		hashval = *s + 31 * hashval;
	return hashval % HASHSIZE;
}

char *strdup(char *s)
{
	char *p;
	p = (char *)malloc(strlen(s)+1);
	if(p != NULL)
		strcpy(p, s);
	return p;
}

struct nlist *lookup(char *s)
{
	struct nlist *np;
	for(np = hashtab[hash(s)]; np != NULL; np = np->next)
		if(strcmp(s, np->name) == 0)
			return np;
	return NULL;
}

struct nlist *install(char *name, char *defn)
{
	struct nlist *np;
	unsigned	hashval;
	if((np = lookup(name)) == NULL) {
		np = (struct nlist *)malloc(sizeof(*np));
		if(np == NULL || (np->name = strdup(name)) == NULL)
			return NULL;
		hashval = hash(name);
		np->next = hashtab[hashval];
	}
	else
		free(np->defn);
	if((np->defn = strdup(defn)) == NULL)
		return NULL;
	return np;
}


int main()
{
	int i;
	char *ss[100] = {"aa", "bb", "cc", "robin1001", "linzi", "susuan"};
	for(i=0; i<5; i++)
		install(ss[i],ss[i]);

	return 0;
}
