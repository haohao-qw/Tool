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

class LockServer {
public:
    struct keep_con m_global_con[CONN_MAX];

private:

    ///m_global_con遍历下标
    int freeid = 0;
    ///监听socket
    int m_listenfd;
    ///epoll创建返回句柄
    int m_epfd;
    ///用于epoll的结构体数组
    struct epoll_event m_ev, m_events[EPOLL_SIZE];
    ///用于访问m_global_con的锁
    pthread_mutex_t conn_lock = PTHREAD_MUTEX_INITIALIZER;

public:
    LockServer(){};
    ~LockServer(){};


    /**
     * @brief 完成socket epoll threadpool的初始化
     */
    void Init();

    /**
     * @brief 采用reactor模型等待连接
     */
    void RecvTransfrom();

    /**
     * @brief 创建大小为size的文件
     * @param filename
     * @param size
     * @return
     */
    int createfile(char *filename, int size);

    /**
     * @brief 初始化Server：监听请求，返回listenfd
     * @param port
     * @return
     */
    int Server_init(int port);

    /**
     * @brief 设置fd非阻塞
     * @param fd
     */
    void set_fd_noblock(int fd);

    /**
     * @brief 接收文件信息，向Client返回本次文件传输使用的freeid
     * @param server
     * @param sockfd
     */
    static void recv_fileinfo(LockServer* server,int sockfd);

    /**
     * @brief 接收文件块
     * @param server
     * @param sockfd
     */
    static void recv_filedata(LockServer* server,int sockfd);

    /**
     * @brief 线程函数
     * @param argc  封装的arg结构体
     * @return
     */
    static void *worker(void *argc);
};
#endif

