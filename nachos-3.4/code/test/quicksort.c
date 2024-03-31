#include "syscall.h"

void swap(int* a, int* b)
{
	int t = *a;
	*a = *b;
	*b = t;
}
int partition(int arr[], int lo, int hi)
{
	int pivot = arr[hi];
	int i = (lo - 1);
	int j = lo;
	for(; j <= hi - 1; j++)
	{
		if(arr[j] < pivot)
		{
			i++;
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[hi]);
	return (i + 1);
}
void quicksort(int arr[], int lo, int hi)
{
	int pi;
	if(lo < hi)
	{
		pi = partition(arr, lo, hi);
		quicksort(arr, lo, pi - 1);
		quicksort(arr, pi + 1, hi);
	}
}
int main()
{
	SpaceId idFile;
	int isSuccess;
	int n;
	int num;
	int i;
	int j;
	int start;
	int end;
	int a[100];
	char c;
	char* command = "Input an integer n, n<100: ";
	char* requirement = "Enter an integer ";
	char* success = "Sort done! Check quicksort.txt for the result\n";
	char number[10];
	char space[3] = ", \0";
	PrintString(command);
	n = ReadInt();
	i = 0;
	for (; i < n; i++)
	{
		PrintString(requirement);
		a[i] = ReadInt();
	}
	quicksort(a, 0, n - 1);
	isSuccess = CreateFile("quicksort.txt");
	if(isSuccess == -1)
		return 1;
	idFile = Open("quicksort.txt", 0);
	PrintString(success);
	i = 0;
	for (; i < n; i++)
	{
		num = a[i];
		j = 0;
		for (; num != 0; j++)
		{
			number[j] = '0' + num % 10;
			num /= 10;
		}
		number[j] = '\0';
		start = 0;
		end = j - 1;
		while (start < end)
		{
			c = number[start];
			number[start] = number[end];
			number[end] = c;
			start++;
			end--;
		}
		Write(number, j, idFile);
		if (i != n - 1)
			Write(space, 2, idFile);
	}
	Close(idFile);
}
	
