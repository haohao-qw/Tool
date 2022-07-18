#include"server.h"
/*
结构体长度
*/

int main(int argc, char **argv)
{
    printf("##################### Server #####################\n");
    Server server;
    server.Init();
    server.RecvTransfrom();
    return 0;
}

