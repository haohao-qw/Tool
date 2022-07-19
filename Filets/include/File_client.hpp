#ifndef CLIENT_H__
#define CLIENT_H__
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <time.h>
#include "File_utils.hpp"

struct HeadArg{
    struct  filehead* head;
    int sockfd;
};

class Client{
private:
    int m_port;///连接的端口
    int m_openfd;///打开的待文件描述符
    int m_sockfd;///用于连接服务端的socket
    struct stat m_filestat;///打开文件信息
    struct fileinfo m_info;///要发送文件信息
    time_t m_start,m_end;///统计文件发送结束时间
    char m_filename[FILENAME_MAXLEN];///文件名称

public:
    Client(int PORT):m_port(PORT){};
    ~Client(){};

    void Init(){
        Client_init(SERVER_IP);
        InitFile();
    }
    /*初始化发送的文件*/
    void InitFile();


    int  SendFileInfo();

    void Transform(int last_bs);

    void recv_fileinfo(int sockfd);

/*创建大小为size的文件*/
    int createfile(char *filename, int size);

/*设置fd非阻塞*/
    void set_fd_noblock(int fd);

/*初始化Client*/
    void Client_init( char *ip);

/*发送文件信息，初始化分块头部信息*/
/*last_bs==0:所有分块都是标准分块；flag>0:最后一个分块不是标准分块，last_bs即为最后一块大小*/
    void send_fileinfo(int sock_fd                  //要发送文件fd
            , char *fname                //filename
            , struct stat* p_fstat       //文件属性结构体
            , struct fileinfo *p_finfo   //返回初始化后的文件信息
            , int *flag);                 //最后一个分块是否时标准分块，0代表是；1代表不是

/*发送文件数据块*/
    static void * send_filedata(void *args);

/*生成文件块头部*/
    struct filehead * create_file_head(char *filename, int freeid, int* offset);
};

#endif
