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
    ///连接的端口
    int m_port;
    ///待打开文件描述符
    int m_openfd;
    ///用于连接服务端的socket
    int m_sockfd;
    ///打开文件信息
    struct stat m_filestat;
    ///要发送文件信息
    struct fileinfo m_info;
    ///统计文件发送结束时间
    time_t m_start,m_end;
    ///文件名称
    char m_filename[FILENAME_MAXLEN];

public:
    /**
     * @param PORT:端口号
     */
    Client(int PORT):m_port(PORT){};

    ~Client(){};

    /**
     * @brief 对外提供初始化接口
     */
    void Init();

    /**
     * @brief 以交互的方式获取文件位置 并且访问该文件
     */
    void InitFile();

    /**
     * @brief 对外暴露的发送接口
     * @param last_bs 传入最后一个分块的大小
     */
    void Transform();

private:

    /**
     * @brief 获取发送fileinfo后的第一个交互内容 具体逻辑 TODO
     * @param sockfd
     */
    void recv_fileinfo(int sockfd);

    /**
     * @brief 根据名称，大小创建文件
     * @param filename 文件名
     * @param size 文件大小
     * @return -1:发送错误 0：成功
     */
    int createfile(char *filename, int size);

    /**
     * @brief 设置fd为非阻塞
     * @param fd 文件描述符
     */
    void set_fd_noblock(int fd);

    /**
     * @brief 根据ip初始化客户端
     * @param ip
     */
    void Client_init( char *ip);

/*发送文件信息，初始化分块头部信息*/
/*last_bs==0:所有分块都是标准分块；flag>0:最后一个分块不是标准分块，last_bs即为最后一块大小*/
    /**
     * @brief 发送文件信息，该信息记录了服务端要创建的内容
     * @param sock_fd  要发送文件fd
     * @param fname    filename
     * @param p_fstat  文件属性结构体
     * @param p_finfo  返回初始化后的文件信息
     * @param flag     最后一个分块是否时标准分块，0代表是；1代表不是
     */
    void send_fileinfo(int sock_f, char *fname,struct stat* p_fstat,struct fileinfo *p_finf,int *flag);

public:
    /**
     * @brief  发送文件数据块
     * @param args 线程传入参数 struct HeadArg
     * @return
     */
    static void* send_filedata(void *args);

    /**
     * @brief 根据传入参数生成文件头部
     * @param filename
     * @param freeid
     * @param offset
     * @return
     */
    struct filehead * create_file_head(char *filename, int freeid, int* offset);
};

#endif

