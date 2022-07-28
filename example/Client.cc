#include"Client.hpp"
#include "lock_threadpool.hpp"


int main(int argc, char **argv)
{
    Client client(PORT);
    client.Init();
    client.Transform();
    return 0;
}


