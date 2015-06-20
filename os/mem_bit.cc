#include <stdio.h>

typedef unsigned char byte;
#define PAGE_SIZE  64
#define PAGE_COUNT 1024 

byte mem[PAGE_SIZE * PAGE_COUNT];

byte bitmap[PAGE_COUNT / 8] = {0};

void setbit(byte * bitmap, int i)  	{	bitmap[i >> 3] |= (1 << (i & 7)); }
int getbit(byte * bitmap, int i)  	{	return bitmap[i >> 3] &  (1 << (i & 7)); }
void clearbit(byte * bitmap, int i)	{	bitmap[i >> 3] | ~(1 << (i & 7));	}


void * mem_alloc(int size)
{
	if(size <= 0)
		return NULL;
	int n_block =  (size % PAGE_SIZE == 0) ? (size / PAGE_SIZE : size / PAGE_SIZE + 1);
	//find n continous block
	int n;
	for(int i=0; i<PAGE_COUNT; )
	{
		for(n=0; n<n_block; n++)
		{
			if(getbit(bitmap, i+n) != 0)
			{
				i += (n + 1);
				break;
			}	
		}
		if(n == n_block)
			break;	
	}

	return NULL;
}

int main()
{
	return 0;
}

