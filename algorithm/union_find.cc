#include <stdio.h>

#define MAX_N 500

int set[MAX_N];

void init(int n)
{
	for(int i=0; i<n; i++) set[i] = i;
}

int find(int i)
{
	while(set[i] != i)
		i = set[i];
	return set[i];
}

void unionset(int i, int j)
{
	int x = find(i), y = find(j);
	if(x == y) return;
	else set[y] = x;
}


int main()
{
	int n;
	int i, j;
	scanf("%d", &n);
	init(n);
	printf("please input relation num and pair\n");
	scanf("%d", &n);
	for(int k=0; k<n; k++)
	{
		scanf("%d%d", &i, &j);
		unionset(i, j);
	}	

	printf("please input ask pair\n");
	while(scanf("%d%d", &i, &j) != EOF)
	{
		if(find(i) == find(j))
			printf("yes\n");
		else
			printf("no\n");
	}

	return 0;
}
