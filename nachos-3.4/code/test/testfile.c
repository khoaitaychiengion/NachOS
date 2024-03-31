#include "syscall.h"
#include "copyright.h"
#define MAX_LENGTH 255
int main()
{

	int filein;
	int fileout;
	
	char buffer[MAX_LENGTH];
	char fileName[MAX_LENGTH];
	int len; 
	PrintString("\n\t\t===== File Test =====\n\n");
	PrintString(" Enter file name: ");
	ReadString(fileName, MAX_LENGTH);

	filein = CreateFile(fileName);

	if (filein == -1) {
		PrintString("\n Can not create file\n");
		return;
	}
	else
		PrintString("\n Create File success!!!\n");

	filein = Open(fileName, 2);
	if (filein != -1)
	{
		PrintString("\n Enter some thing:");
		len = Read(buffer, MAX_LENGTH, filein);
		
		if (len != -1 && len != -2) 
		{
			fileout = Open(fileName, 0); 
			if (fileout != -1)
			{
				PrintString("\n Writing to file success !!!\n");
				Write(buffer, len, fileout); 
				Close(fileout); 
			}
		}
		Close(filein); 
	}
	PrintString("\n\n");

	return 0;
}
