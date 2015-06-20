#include <stdio.h>

void swap(int *a, int *b)
{
	int tmp = *a;
	*a = * b;
	*b = tmp;
}

void qsort(int *arr, int left, int right)
{
	int cur=left, i=left, j=right;
	if(left >= right)
		return;
	while(i<j)
	{
		while(arr[j] > arr[cur] && i<j) j--;
		swap(&arr[j], &arr[cur]);
		cur = j;
		while(arr[i] < arr[cur] && i<j) i++;
		swap(&arr[i], &arr[cur]);
		cur = i;
	}

	qsort(arr, left, cur);
	qsort(arr, cur+1, right);
}

int main()
{
	int arr[]={1, 3, 5, 7, 9, 2, 4, 6, 8, 10};
	int i;
	qsort(arr, 0, 9);

	for(i=0; i<10; i++)
		printf("%d ", arr[i]);

	printf("\n");
	return 0;
}
