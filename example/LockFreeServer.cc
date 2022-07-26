/#include"File_lockfreeserver.hpp"
/*
结构体长度
*/

int main(int argc, char **argv)
{
    printf("##################### Server #####################\n");
    LockServer server;
    server.Init();
    server.RecvTransfrom();
    return 0;
}/

