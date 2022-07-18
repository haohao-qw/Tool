#include"Thread.h"
#include"client.h"


int main(int argc, char **argv)
{
    Client client(PORT);
    client.Init();
    client.InitFile();
    int last= client.SendFileInfo();
    client.Transform(last);

    return 0;
}


