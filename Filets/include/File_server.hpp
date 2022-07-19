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

class Server {
public:
    /*gconn[]数组存放连接信息，带互斥锁*/
    struct keep_con m_global_con[CONN_MAX];
    int freeid = 0;
    pthread_mutex_t conn_lock = PTHREAD_MUTEX_INITIALIZER;

    int m_listenfd;
    int m_epfd;
    struct epoll_event m_ev, m_events[EPOLL_SIZE];

public:
    Server(){};
    ~Server(){};


    void Init(){
        /*创建线程池*/
        if (thread_pool_create(THREAD_NUM) != 0) {
            printf("tpool_create failed\n");
            exit(-1);
        }
        printf("--- Thread Pool Strat ---\n");
        /*初始化server，监听请求*/
        m_listenfd = Server_init(PORT);
        socklen_t sockaddr_len = sizeof(struct sockaddr);

        /*epoll*/
        m_epfd = epoll_create(EPOLL_SIZE);
        m_ev.events = EPOLLIN;
        m_ev.data.fd = m_listenfd;
        epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listenfd, &m_ev);
    }

    void RecvTransfrom(){
        while(1){
            int events_count = epoll_wait(m_epfd, m_events, EPOLL_SIZE, -1);
            int i=0;

            /*接受连接，添加work到work-Queue*/
            for(; i<events_count; i++){
                if(m_events[i].data.fd == m_listenfd)
                {
                    int connfd;
                    struct sockaddr_in  clientaddr;
                    socklen_t sockaddr_len = sizeof(struct sockaddr);
                    while( ( connfd = ::accept(m_listenfd, (struct sockaddr *)&clientaddr, &sockaddr_len) ) > 0 )
                    {
                        ///进行线程分发 也就是将任务丢到任务队列中
                        printf("EPOLL: Received New Connection Request---connfd= %d\n",connfd);
                        struct args *p_args = (struct args *)malloc(sizeof(struct args));
                        p_args->sockfd = connfd;
                        p_args->recv_finfo = recv_fileinfo;
                        p_args->recv_fdata = recv_filedata;
                        p_args->server=this;
                        /*添加work到work-Queue*/
                        //worker：绑定的线程函数
                        thread_pool_add_work(worker, (void*)p_args);
                    }
                }
            }
        }
    }

/*创建大小为size的文件*/
    int createfile(char *filename, int size);

/*初始化Server：监听请求，返回listenfd*/
    int Server_init(int port);

/*设置fd非阻塞*/
    void set_fd_noblock(int fd);

/*接收文件信息，向Client返回本次文件传输使用的freeid*/
    static void recv_fileinfo(Server*server,int sockfd);

/*接收文件块*/
    static void recv_filedata(Server*server,int sockfd);

/*线程函数*/
    static void *worker(void *argc);
};
#endif

