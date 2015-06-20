#include <stdio.h>
#define COL 4

bool findYang(const int array[][COL],int search)
{
	int i=0, j=COL-1;
	int tmp=array[i][j];

	while(true)
	{
		if(array[i][j]==search)
			return false;
		else if(search<tmp && j>0)
			tmp=array[i][j-1];
		else if(search>tmp && i<COL-1)
			tmp=array[i+1][j];
		else //expand the matrix 
			return false;
	}
}
int main()
{
	return 0;
}
