#ifndef SERVER_H__
#define SERVER_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include "File_utils.hpp"
#include "lock_threadpool.hpp"
#include "File_utils.hpp"


/*创建大小为size的文件*/
int createfile(char *filename, int size);

/*初始化Server：监听请求，返回listenfd*/
int Server_init(int port);

/*设置fd非阻塞*/
void set_fd_noblock(int fd);

/*接收文件信息，向Client返回本次文件传输使用的freeid*/
void recv_fileinfo(int sockfd);

/*接收文件块*/
void recv_filedata(int sockfd);

/*线程函数*/
void * worker(void *argc);
#endif

