#include "syscall.h"

void merge(int arr[], int l, int m, int r)
{
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;
	int L[n1], R[n2];
	i = 0;
	j = 0;
	for(; i < n1; i++)
		L[i] = arr[l + i];
	for(; j < n2; j++)
		R[j] = arr[m + 1 + j];
	i = 0;
	j = 0;
	k = l;
	while (i < n1 && j < n2)
	{
		if (L[i] <= R[j])
		{
			arr[k] = L[i];
			i++;
		}
		else
		{
			arr[k] = R[j];
			j++;
		}
		k++;
	}
	while (i < n1)
	{
		arr[k] = L[i];
		i++;
		k++;
	}
	while (j < n2)
	{
		arr[k] = R[j];
		j++;
		k++;
	}
}
void mergesort(int arr[], int l, int r)
{
	int m;
	if (l < r)
	{
		m = l + (r - l) / 2;
		mergesort(arr, l, m);
		mergesort(arr, m + 1, r);
		merge(arr, l, m, r);
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
	char* success = "Sort done! Check mergesort.txt for the result\n";
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
	mergesort(a, 0, n - 1);
	isSuccess = CreateFile("mergesort.txt");
	if(isSuccess == -1)
		return 1;
	idFile = Open("mergesort.txt", 0);
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
	
