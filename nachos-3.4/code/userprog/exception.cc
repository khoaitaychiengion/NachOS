// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MaxFileLength 32
#define fileTableSize 15

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------


char* UserToSystem(int virtAddr, int limit) {
    char* kernelBuff = NULL;
    kernelBuff = new char[limit + 1];

    if (kernelBuff == NULL)
        return kernelBuff;
        
    memset(kernelBuff, 0, limit + 1);
    int oneChar;

    for (int index = 0; index < limit; index++) {
        machine->ReadMem(virtAddr + index, 1, &oneChar);
        kernelBuff[index] = (char)oneChar;
        if (oneChar == 0)
            break;
    }

    return kernelBuff;
}

int SystemToUser(int virtAddr, int len, char* buffer) {
    if (len < 0) return -1;
    if (len == 0) return len;

    int i = 0, oneChar = 0;
    do {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);

    return i;
}

void increasePC() {
    // read register with the register's number is the value of the current program counter
    int counter = machine->ReadRegister(PCReg);

    // store value into the register with the register's number is the value of the previous program counter
    machine->WriteRegister(PrevPCReg, counter);

    // read register with the register's number is the value of the next program counter
    counter = machine->ReadRegister(NextPCReg);

    // store value into the register with the register's number is the value of the current program counter
    machine->WriteRegister(PCReg, counter);

    // read register with the register's number is the value of the second next program counter
    counter += 4;

    // store value into the register with the register's number is the value of the next program counter
    machine->WriteRegister(NextPCReg, counter);
}

void syscallSC_PrintInt()
{
    // Input: mot so integer
    // Output: khong
    // Chuc nang: In so nguyen len man hinh console
    int number = machine->ReadRegister(4);
    //check nếu số đó là số 0
    if(number == 0)
    {
        gSynchConsole->Write("0", 1); // In ra man hinh so 0
        increasePC();
        return;    
    }
            
    // mac dinh la so duong
    bool positiveFlag = TRUE; 
    //luu do dai chuoi cua so
    int digitCount = 0;
    //so bat dau tai vi tri thu may trong chuoi  
    int firstDigitIndex = 0; 
    //check xem số đó có phải là số âm không
    if(number < 0)
    {
        positiveFlag = FALSE;
        // chuyen so am thanh so duong de tinh do dai chuoi
        number = -number; 
        firstDigitIndex = 1; 
    } 	
    //tinh do dai chuoi cua so
    int temp = number;
    while(temp != 0)
    {
        digitCount++;
        temp /= 10;
    }

    // tao buffer de luu chuoi so
    char* buffer;
    int MAX_BUFFER = 255;
    buffer = new char[MAX_BUFFER + 1];
    int i;
    for(i = firstDigitIndex + digitCount - 1; i >= firstDigitIndex; i--)
    {
        buffer[i] = (char)((number % 10) + '0');
        number /= 10;
    }


    if(positiveFlag == FALSE)
    {
        buffer[0] = '-';
        buffer[digitCount + 1] = 0;
        gSynchConsole->Write(buffer, digitCount + 1);
        delete [] buffer;

    }
    else
    {
        buffer[digitCount] = 0;
        gSynchConsole->Write(buffer, digitCount);
        delete [] buffer;
    }
    increasePC();
    return;
}

void syscallSC_ReadChar()
{
    // Input: Khong
    // Output: Ki tu(char)
    // Cong dung: Doc mot ki
    int maxBytes = 255;
    char* buffer = new char[255];
    int numBytes = gSynchConsole->Read(buffer, maxBytes);

    if(numBytes > 1) //Neu nhap nhieu hon 1 ky tu thi khong hop le
    {
        printf(" Chi duoc nhap duy nhat 1 ky tu!\n");
        DEBUG('a', " ERROR: Chi duoc nhap duy nhat 1 ky tu!\n");
        machine->WriteRegister(2, 0);
    }
    else if(numBytes == 0) //Ky tu rong
    {
        printf(" Ky tu rong!\n");
        DEBUG('a', " ERROR: Ky tu rong!\n");
        machine->WriteRegister(2, 0);
    }
    else
    {
        //Chuoi vua lay co dung 1 ky tu, lay ky tu o index = 0, return vao thanh ghi R2
        char c = buffer[0];
        machine->WriteRegister(2, c);
    }

    delete buffer;
    increasePC(); // error system
    return;
}   

void syscallSC_PrintChar()
{
    // Input: Ki tu(char)
    // Output: Ki tu(char)
    // Cong dung: Xuat mot ki tu la th
    char c = (char)machine->ReadRegister(4); 
    gSynchConsole->Write(&c, 1); 
    increasePC();
    return;
}

void syscallSC_ReadInt()
{
    // Input: Khong
    // Output: So nguyen(int)
    // Cong dung: Doc mot so nguyen tu ban phim
    char* buf;
    const int BUF_SIZE = 255;
    buf = new char[BUF_SIZE + 1];
    int bytesRead = gSynchConsole->Read(buf, BUF_SIZE);
    int num = 0;
    if(bytesRead == 0) //Neu khong nhap gi thi tra ve 0
    {
        num = 0;
        machine->WriteRegister(2, num);
        delete [] buf;
        increasePC();
        return;
    }
    bool isNeg = FALSE;
    int firstDigitPos = 0;
    int lastDigitPos = 0;
    if(buf[0] == '-')
    {
        isNeg = TRUE;
        firstDigitPos = 1;
        lastDigitPos = 1;                        			   		
    }


    //Kiem tra xem chuoi nhap vao co phai la so nguyen khong
    int i;
    for(i = firstDigitPos; i < bytesRead; i++)					
    {
        //Neu gap ki tu '.' thi kiem tra co la so nguyen ko
        //1.00 va 1.0 la so nguyen
        if(buf[i] == '.')
        {
            int ii;
            for(ii = i + 1; ii < bytesRead; ii++)
            {
                //Neu co ki tu khac so  0 thi la so thap phan
                if(buf[ii] != '0')
                {
                    printf(" Invalid integer number\n");
                    DEBUG('a', " Invalid integer number\n");
                    machine->WriteRegister(2, 0);
                    increasePC();
                    delete[] buf;
                    return;
                }
            }
            lastDigitPos = i - 1;				
            break;                           
        }
        //Neu gap ki tu khac so thi tra ve loi
        else if(buf[i] < '0' || buf[i] > '9')
        {
            printf(" Invalid integer number\n");
            DEBUG('a', " Invalid integer number\n");
            machine->WriteRegister(2, 0);
            increasePC();
            delete[] buf;
            return;
        }
        lastDigitPos = i;    
    }			
    
    //Chuyen chuoi thanh so nguyen
    for(i = firstDigitPos; i <= lastDigitPos; i++)
    {
        num = num * 10 + (int)(buf[i] - '0'); 
    }

    if(isNeg)
    {
        num = -num;
    }
    machine->WriteRegister(2, num);
    delete [] buf;
    increasePC();
    return;
}

void syscallSC_PrintString()
{
    // Input: Dia chi cua chuoi
    // Output: Khong
    // Cong dung: In ra
    int virtAddr;
    char* buf;
    // Lay dia chi cua chuoi
    virtAddr = machine->ReadRegister(4);
    // Copy chuoi tu vung nho cua User sang System
    buf = UserToSystem(virtAddr, 255);
    int len = 0;
    // Tinh do dai cua chuoi
    while (buf[len] != 0) len++;
    // In ra
    gSynchConsole->Write(buf, len + 1);
    // Xoa bo nho
    delete[] buf;
    increasePC(); // Tang Program Counter 
    return;
}

void clear ()
{
   int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void syscallSC_ReadString()
{
    // Input: Dia chi cua chuoi, so ki tu toi da duoc nhap, dia chi buffer
    // Output: Chuoi
    // Cong dung: Doc chuoi tu ban phim
    int virtAddr, length;
    char* buf;
    //clear();
    // Lay dia chi cua chuoi
    virtAddr = machine->ReadRegister(4);
    // Lay so ki tu toi da duoc nhap
    length = machine->ReadRegister(5);
    // Tao buffer
    buf = new char[length];
    // Doc chuoi tu ban phim
    int numBytes = gSynchConsole->Read(buf, length);
    // Copy chuoi tu System sang User
    SystemToUser(virtAddr, numBytes, buf);
    // Xoa bo nho
    delete[] buf;
    increasePC(); // Tang Program Counter
    return;
}

void syscallSC_PrintFloat()
{
    // Input: mot so float
    // Output: khong
    // Chuc nang: In so ng 
    int a = machine->ReadRegister(4);
    //check nếu số đó là số 0
    float* f = (float*)&a;
    float num = *f;
    char str[255];
    int isNegative = 0;
    if (num < 0) {
        isNegative = 1;
        num = -num; // Make num positive
    }

    int wholePart = (int)num; // Extract the whole part
    float fractionPart = num - wholePart; // Extract the fractional part

    // Count the number of digits in the whole part
    int digitCount = 0;
    int tempWholePart = wholePart;
    do {
        tempWholePart /= 10;
        digitCount++;
    } while (tempWholePart != 0);

    // Convert the fractional part to string
    int i = 0;
    if (isNegative) {
        str[i++] = '-'; // Add negative sign
    }

    // Convert the whole part to string
    int j = i + digitCount - 1; // Start from the last index
    do {
        str[j--] = '0' + wholePart % 10;
        wholePart /= 10;
    } while (wholePart > 0);

    // Add decimal point
    str[i + digitCount] = '.';
    j = i + digitCount + 1;

    // Convert the fractional part to string
    int precision = 0;
    do {
        fractionPart *= 10;
        int digit = (int)fractionPart;
        str[j++] = '0' + digit;
        fractionPart -= digit;
        precision++;
    } while (precision < 6); // Adjust precision to match the original float

    // Null-terminate the string
    str[j] = '\0';
    gSynchConsole->Write(str, j+1);
    increasePC();
    return;

}

bool isFloat(const char *str) {
    bool hasDot = false;
    bool hasDigit = false;

    // check dấu + hoặc -
    if (*str == '+' || *str == '-')
        str++;

    while (*str != '\0') {
        if (*str == '.') {
            if (hasDot || !hasDigit)
                //Có nhiều hơn 1 dấu chấm hoặc không có số trước dấu chấm
                return false; 
            hasDot = true;
        } else if (*str >= '0' && *str <= '9') {
            hasDigit = true;
        } else {
            //Nếu không phải số hoặc dấu chấm
            return false; 
        }
        str++;
    }

    // Neu khong co so nao thi tra ve false
    if (!hasDigit)
        return false;

    return true;
}

float stringToFloat(const char *str) {
    if(!isFloat(str))
        return 0;
    float result = 0.0;
    float fraction = 0.0;
    int sign = 1;

    // Check for leading sign
    if (*str == '+') {
        str++;
    } else if (*str == '-') {
        sign = -1;
        str++;
    }

    // Convert whole part
    while (*str >= '0' && *str <= '9') {
        result = result * 10.0 + (*str - '0');
        str++;
    }

    // Convert fraction part
    if (*str == '.') {
        float divisor = 10.0;
        str++;
        while (*str >= '0' && *str <= '9') {
            fraction += (*str - '0') / divisor;
            divisor *= 10.0;
            str++;
        }
    }

    // Apply sign and return
    return sign * (result + fraction);
}

void syscallSC_ReadFloat()
{
    // Input: mot so float
    // Output: khong
    // Chuc nang: Nhan so float tu user
    const int BUF_SIZE = 255;
    char* str = new char[BUF_SIZE + 1];
    int bytesRead = gSynchConsole->Read(str, BUF_SIZE);

    int num = 0;
    if(bytesRead == 0 || !isFloat(str)) //Neu khong nhap gi thi tra ve 0
    {
        num = 0;
        machine->WriteRegister(2, num);
        delete str;
        increasePC();
        return;
    }
    
    float* f = new float;
    *f = stringToFloat(str);
    int* a;
    a = (int*)f;
    machine->WriteRegister(2, *a);
    delete f;
    delete str;
    increasePC();
    return;

}

void syscallSC_CreateFile() {
    int virtAddr;
    char *fileName;
    DEBUG('a', " SC_CreateFile being called ...\n");
    DEBUG('a', " Reading virtual address of file name\n");

    virtAddr = machine->ReadRegister(4);
    DEBUG('a', " Reading file name...\n");

    fileName = UserToSystem(virtAddr, MaxFileLength + 1);
    // solve situations
    if (strlen(fileName) == 0) {
        printf(" File name is not valid\n");
        DEBUG('a', " File name is not valid\n");
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }
    else if (fileName == NULL) {
        printf(" Not enough memory in system\n");
        DEBUG('a', " Not enough memory in system\n");
        machine->WriteRegister(2, -1);
        delete fileName;
        increasePC();
        return;
    }

    DEBUG('a', "\n Finishing reading file name");

    if (!fileSystem->Create(fileName, 0)) {
        printf(" Error in creating file '%s'.\n", fileName);
        machine->WriteRegister(2, -1);
        delete fileName;
        increasePC();
        return;
    }

    // Create file success!!!
    machine->WriteRegister(2, 0);
    delete fileName;
    increasePC();
    return;
}

void syscallSC_Open() {
    int virtAddr = machine->ReadRegister(4);
    int type = machine->ReadRegister(5);
    char *fileName;
    fileName = UserToSystem(virtAddr, MaxFileLength);

    int freeSlot = fileSystem->FindFreeSlot();
    if (freeSlot != -1) {
        if (type == 0 || type == 1) {
            if ((fileSystem->openf[freeSlot] = fileSystem->Open(fileName, type)) != NULL) {
                // Open File success !!!
                machine->WriteRegister(2, freeSlot);
            }
        }
        else if (type == 2) {
            machine->WriteRegister(2, 0);
        }
        else if (type == 3) {
            machine->WriteRegister(2, 1);
        }
        else {
            printf("\n Error: Type is not valid\n");
            machine->WriteRegister(2, -1);
        }
        delete[] fileName;
        increasePC();
        return;
    }
    // Open File failed!!!
    machine->WriteRegister(2, -1);
    delete[] fileName;

    increasePC();
    return;
}

void syscallSC_Read() {
    // Input: buffer(char*), charcount(int), id of file (OpenFileId)
    // Output: -1: Error, -2 or bytes: success
    // Purpose: Read File and return number of bytes if success.

    // Get data
    int virtAddr = machine->ReadRegister(4); // get buffer(char*)
    int charcount = machine->ReadRegister(5); // get charcount
    int id = machine->ReadRegister(6); // get FileID
    
    int OldPos;
    int NewPos;
    char *buf;

    // check failing situations

    // Out of file table size
    if (id < 0 || id > fileTableSize - 1) {
        printf(" Error: Out of file table.\n");
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }

    // Check file exists
    if (fileSystem->openf[id] == NULL) {
        printf(" Error: File does not exists.\n");
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }

    // Not suitable for read file
    if (fileSystem->openf[id]->type == 3) {
        printf(" Error: File just for Writing.\n");
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }

    OldPos = fileSystem->openf[id]->GetCurrentPos();
    buf = UserToSystem(virtAddr, charcount);

    // For Reading Console
    if (fileSystem->openf[id]->type == 2) {
        // Use Read function from SynchConsole to return number of bytes.
        int NumberOfBytes = gSynchConsole->Read(buf, charcount);
        SystemToUser(virtAddr, NumberOfBytes, buf);
        machine->WriteRegister(2, NumberOfBytes);
        delete buf;
        increasePC();
        return;
    }

    // For Reading File
    if ((fileSystem->openf[id]->Read(buf, charcount)) > 0) {
        // Number of Bytes = New Position - Old Position
        NewPos = fileSystem->openf[id]->GetCurrentPos();
        int NumberOfBytes = NewPos - OldPos;
        SystemToUser(virtAddr, NumberOfBytes, buf);
        machine->WriteRegister(2, NumberOfBytes);
    }
    else {
        machine->WriteRegister(2, -2);
    }
    delete buf;
    increasePC();
    return;
}

void syscallSC_Write() {
    // Input: buffer(char*), charcount(int), id of file (OpenFileId)
    // Output: -1: Error, -2 or bytes: success
    // Purpose: Write File and return number of bytes if success.

    // Get data
    int virtAddr = machine->ReadRegister(4); // get buffer(char*)
    int charcount = machine->ReadRegister(5); // get charcount
    int id = machine->ReadRegister(6); // get FileID
    
    int OldPos;
    int NewPos;
    char *buf;

    // check failing situations

    // Out of file table size
    if (id < 0 || id > fileTableSize - 1) {
        printf(" Error: Out of file table.\n");
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }

    // Check file exists
    if (fileSystem->openf[id] == NULL) {
        printf(" Error: File does not exists.\n");
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }

    // Not suitable for read file
    if (fileSystem->openf[id]->type == 1 || fileSystem->openf[id]->type == 2) {
        printf(" Error: Cannot write file stdin or File just for reading.\n");
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }

    OldPos = fileSystem->openf[id]->GetCurrentPos();
    buf = UserToSystem(virtAddr, charcount);

    // Writing For Console
    if (fileSystem->openf[id]->type == 3) {
        int i = 0;
        while (buf[i] != 0 && buf[i] != '\n') {
            gSynchConsole->Write(buf + i, 1);
            ++i;
        }
        buf[i] = '\n';
        gSynchConsole->Write(buf + i, 1);  
        machine->WriteRegister(2, i - 1);
        delete buf;
        increasePC();
        return;
    }

    // Writing For File
    if (fileSystem->openf[id]->type == 0) {
        if ((fileSystem->openf[id]->Write(buf, charcount)) > 0) {
            int NumberOfBytes = NewPos - OldPos;
            machine->WriteRegister(2, NumberOfBytes);
            delete buf;
            increasePC();
            return;
        }
    }
}

void syscallSC_Close() {
    int fileID = machine->ReadRegister(4);
    if (fileID >= 0 && fileID <= fileTableSize - 1) {
        // if file is opened
        if (fileSystem->openf[fileID]) {
            delete fileSystem->openf[fileID];
            fileSystem->openf[fileID] = NULL;
            machine->WriteRegister(2, 0);
            increasePC();
            return;
        }
    }
    // Fail closing file
    machine->WriteRegister(2, -1);
    increasePC();
    return;
}

void syscallSC_Seek() {
    // Input: Vi tri(int), id cua file(OpenFileId)
    // Output: -1: Loi, Vi tri thuc su: Thanh cong
    // Cong dung: Di chuyen con tro den vi tri thich hop trong file voi tham so la vi tri can chuyen va id cua file
    int pos = machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
    int id = machine->ReadRegister(5);  // Lay id cua file
    // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
    if (id < 0 || id > fileTableSize - 1)
    {
        printf(" Khong the seek vi id nam ngoai bang mo ta file.\n");
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }
    // Kiem tra file co ton tai khong
    if (fileSystem->openf[id] == NULL)
    {
        printf(" Khong the seek vi file nay khong ton tai.\n");
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }
    // Kiem tra co goi Seek tren console khong
    if (id == 0 || id == 1)
    {
        printf(" Khong the seek tren file console.\n");
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }
    // Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
    pos = (pos == -1) ? fileSystem->openf[id]->Length() : pos;
    if (pos > fileSystem->openf[id]->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
    {
        printf(" Khong the seek file den vi tri nay.\n");
        machine->WriteRegister(2, -1);
    }
    else
    {
        // Neu hop le thi tra ve vi tri di chuyen thuc su trong file
        fileSystem->openf[id]->Seek(pos);
        machine->WriteRegister(2, pos);
    }
    increasePC();
    return;
}


void syscallSC_Exec() {
    // Input: Dia chi cua ten chuong trinh
    // Output: -1: Loi, ProcessID: Thanh cong
    // Cong dung: Thuc thi chuong trinh voi ten chuong trinh truyen vao
    int virtAddr = machine->ReadRegister(4); // Lay dia chi cua ten chuong trinh
    char *progName;
    progName = UserToSystem(virtAddr, MaxFileLength);
    // Kiem tra xem co du bo nho de chua ten chuong trinh khong
    if (progName == NULL)
    {
        printf(" Khong du bo nho de chua ten chuong trinh.\n");
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }
    // Kiem tra xem co ton tai chuong trinh nay khong
    OpenFile *prog = fileSystem->Open(progName);
    if (prog == NULL)
    {
        printf(" Khong the mo file %s de thuc thi.\n", progName);
        machine->WriteRegister(2, -1);
        increasePC();
        return;
    }
    delete prog;

    // Return child process id
    int id = pTab->ExecUpdate(progName); 
    machine->WriteRegister(2,id);

    delete[] progName;	
    IncreasePC();
    return;
    
}

void ExceptionHandler(ExceptionType which) {
    int type = machine->ReadRegister(2);
    switch(which) {
        case NoException:
            return;
        case SyscallException:
            switch(type) {
                case SC_Halt:
		    printf(" Shutdown, initiated by user program.\n");
                    interrupt->Halt();
                    break;
                case SC_PrintInt:
                    syscallSC_PrintInt();	
                    break;
                case SC_ReadInt:
                    syscallSC_ReadInt();
                    break;
                case SC_ReadChar:
                    syscallSC_ReadChar();
                    break;
                case SC_PrintChar:
                    syscallSC_PrintChar();
                    break;
                case SC_PrintString:
                    syscallSC_PrintString();
                    break;
                case SC_ReadString:
                    syscallSC_ReadString();
                    break;
                case SC_PrintFloat:
                    syscallSC_PrintFloat();
                    break;
                case SC_ReadFloat:
                    syscallSC_ReadFloat();
                    printf("ReadFloat\n");
                    break;
                case SC_CreateFile:
                    syscallSC_CreateFile();
                    break;
                case SC_Open:
                    syscallSC_Open();
                    break;
                case SC_Read:
                    syscallSC_Read();
                    break;
                case SC_Write:
                    syscallSC_Write();
                    break;
                case SC_Seek:
                    syscallSC_Seek();
                    break;
                case SC_Close:
                    syscallSC_Close();
                    break;
                default:
                    increasePC();
                    break;
            }
            break;

        case PageFaultException:
	    printf(" PageFaultException is happening,\n");
            interrupt->Halt();
            break;

        case ReadOnlyException:
	    printf(" ReadOnlyException is happening,\n");
            interrupt->Halt();
            break;

        case BusErrorException:
            DEBUG('a', " BusError is happening,\n");
            interrupt->Halt();
            break;

        case AddressErrorException:
	    printf(" AddressErrorException is happening,\n");
            interrupt->Halt();
            break;

        case OverflowException:
	    printf(" OverflowException is happening,\n");
            interrupt->Halt();
            break;

        case IllegalInstrException:
	    printf(" IllegalInstrException is happening,\n");
            interrupt->Halt();
            break;

        case NumExceptionTypes:
	    printf(" NumExceptionTypes is happening,\n");
            interrupt->Halt();
            break;

        default:
            interrupt->Halt();
    }
}
