#include"File_client.hpp"
#include "lock_threadpool.hpp"


int main(int argc, char **argv)
{
    Client client(PORT);
    client.Init();
    client.InitFile();
    int last= client.SendFileInfo();
    client.Transform(last);

    return 0;
}


