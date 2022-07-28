#ifndef THREAD_POOL_H__
#define THREAD_POOL_H__

#include <pthread.h>

///同步日志 异步日志
//#include "async_log.hpp"
#include "syn_log.hpp"

Logger::ptr g_logger=LOG_ROOT();


////独立的加锁线程池

class Server;

/**
 * @brief 工作线程参数
 */
struct args{
    int sockfd;
    Server* server;
    void (*recv_finfo)(Server*,int fd);
    void (*recv_fdata)(Server*,int fd);
};


/**
 * @brief 线程结构体
 */
typedef struct thread_node {
    /// 任务函数
    void* (*routine)(void*);
    /// 传入任务函数的参数
    void* arg;
    /// 下一个节点 组织成单链表
    struct thread_node* next;
}thread_node_t;


/**
 * @brief c风格线程池
 */
typedef struct thread_pool{
    /// 线程池是否销毁
    int shutdown;
    /// 最大线程数
    int  max_thr_num;
    /// 线程ID数组首地址
    pthread_t  *thr_id;
    /// 任务链表队首
    thread_node_t    *queue_head;
    /// 任务链表队尾
    thread_node_t    *queue_tail;
    /// 队列锁
    pthread_mutex_t  queue_lock;
    /// 条件变量
    pthread_cond_t   queue_ready;
}thread_pool_t;

/**
 * @brief     创建线程池
 * @param     max_thr_num 最大线程数
 * @return    0: 成功 其他: 失败
 */
thread_pool_t* thread_pool_create(int max_thr_num);

/**
 * @brief     销毁线程池
 */
void thread_pool_destroy();

/**
 * @brief     向线程池中添加任务
 * @param     routine 任务函数指针
 * @param     arg 任务函数参数
 * @return    0: 成功 其他:失败
 */
int thread_pool_add_work(void*(*routine)(void*), void *arg);

#endif

