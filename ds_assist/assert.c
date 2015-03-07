#include <stdio.h>
#define NDEBUG
#include <assert.h>

int main()
{
	int a = 10;
	assert(10 == a);
	a = 20;
	assert(10 == a);
	return 0;
}
