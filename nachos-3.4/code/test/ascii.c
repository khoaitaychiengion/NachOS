#include "syscall.h"

int main()
{
	SpaceId idFile;
	int isSuccess;
	int i;
	int i2;
	int j;
	int start;
	int end;
	char c;
	char tab = '\t';
	char nline = '\n';
	char column = '|';
	char num[3];
	char *firstLine = "\t\t------ASCII------\t\t\n";
	char *tabb = "\t";
	char *line = "\n";
	char *col = "|";
	char ascii[2];
	i = 0;
	isSuccess = CreateFile("ascii.txt");
	if(isSuccess == -1)
		return 1;
	idFile = Open("ascii.txt", 0);
	for (; i < 22; i++)
	{
		c = firstLine[i];
		PrintChar(c);
	}
	Write(firstLine, 22, idFile);
	i = 33;
	for (; i < 65; i++)
	{
		i2 = i;
		j = 0;
		for (; i2 != 0; j++)
		{
			num[j] = '0' + i2 % 10;
			i2 /= 10;
		}
		num[j] = '\0';
		start = 0;
		end = j - 1;
		while (start < end)
		{
			c = num[start];
			num[start] = num[end];
			num[end] = c;
			start++;
			end--;
		}
		PrintInt(i);
		Write(num, j, idFile);
		PrintChar(tab);
		Write(tabb, 1, idFile);
		PrintChar((char)i);
		ascii[0] = (char)i;
		ascii[1] = '\0';
		Write(ascii, 1, idFile);
		PrintChar(tab);
		Write(tabb, 1, idFile);
		PrintChar(column);
		Write(col, 1 ,idFile);

		i2 = i + 32;
		j = 0;
		for (; i2 != 0; j++)
		{
			num[j] = '0' + i2 % 10;
			i2 /= 10;
		}
		num[j] = '\0';
		start = 0;
		end = j - 1;
		while (start < end)
		{
			c = num[start];
			num[start] = num[end];
			num[end] = c;
			start++;
			end--;
		}
		PrintInt(i + 32);
		Write(num, j, idFile);
		PrintChar(tab);
		Write(tabb, 1, idFile);
		PrintChar((char)(i+32));
		ascii[0] = (char)(i+32);
		Write(ascii, 1, idFile);
		PrintChar(tab);
		Write(tabb, 1, idFile);
		PrintChar(column);
		Write(col, 1, idFile);

		if (i + 64 > 127)
		{
			PrintChar(tab);
			Write(tabb, 1, idFile);
			PrintChar(tab);
			Write(tabb, 1, idFile);
			PrintChar(column);
			Write(col, 1, idFile);
			PrintChar(nline);
			Write(line, 1, idFile);
			break;
		}
		
		i2 = i + 64;
		j = 0;
		for (; i2 != 0; j++)
		{
			num[j] = '0' + i2 % 10;
			i2 /= 10;
		}
		num[j] = '\0';
		start = 0;
		end = j - 1;
		while (start < end)
		{
			c = num[start];
			num[start] = num[end];
			num[end] = c;
			start++;
			end--;
		}
		PrintInt(i + 64);
		Write(num, j, idFile);
		PrintChar(tab);
		Write(tabb, 1, idFile);
		PrintChar((char)(i + 64));
		ascii[0] = (char)(i + 64);
		Write(ascii, 1, idFile);
		PrintChar(tab);
		Write(tabb, 1, idFile);
		PrintChar(column);
		Write(col, 1, idFile);
		PrintChar(nline);
		Write(line, 1, idFile);
	}		
	
	Close(idFile);
}
