#include "syscall.h"  

int main (){
    int a = ReadFloat();
   // float* val = (float*) &a;
    PrintFloat(a);
    PrintChar('\n');
    PrintString("fntw\n");
}