#include "syscall.h"

void main()
{
    int pingID, pongID;
    PrintString("Ping-pong is running...\n\n");
    pingID = Exec("./test/ping");
    pongID = Exec("./test/pong");
    Join(pingID);
    Join(pongID);
}
