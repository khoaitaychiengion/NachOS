#include "syscall.h"
#define MAX_LENGTH 32
int main()
{
	int openFileId;
	int fileSize;
	int i;
	char c;
	char *filename = "mota.txt";
	
	openFileId = Open(filename, 1);
	
	if (openFileId != -1)
	{
		fileSize = Seek(-1, openFileId);
		i = 0;
		Seek(0, openFileId);
		
		for (; i < fileSize; i++)
		{
			Read(&c, 1, openFileId); 
			PrintChar(c); 
		}
		Close(openFileId);
	}
	else
	{
		PrintString(" --- Mo file that bai!!\n\n");
	}
}
