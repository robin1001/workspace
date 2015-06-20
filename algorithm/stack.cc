#include <stdio.h>

#define N 100
int stack[N];
int *pstack = stack;
#define push(s)  *pstack++=s
#define pop() psitack--

void main()
{
	int i;
	printf("simple stack using macro\n");	
	for(i=0; i<10; i++)
	{
		push(i);
		printf("push %d\n", i);
	}	
}




