#include <stdio.h>

#define N 10000

int num[N] = {0};


int main()
{
	int n, i;
	int max = -100000000;
	int cur_max = 0;
	scanf("%d", &n);
	for(i=0; i<n; i++) scanf("%d", &num[i]);

	for(i=0; i<n; i++)
	{
		cur_max += num[i];
		if(cur_max > max)
			max = cur_max;
		if(cur_max < 0)
			cur_max = 0;
	}

	printf("%d\n", max);
	return 0;
}