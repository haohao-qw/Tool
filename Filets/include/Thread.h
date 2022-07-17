#ifndef THREAD_POOL_H__
#define THREAD_POOL_H__

#include <pthread.h>
////独立的加锁线程池

class Server;
/*线程参数*/
struct args{
    int sockfd;
    Server* server;
    void (*recv_finfo)(Server*,int fd);
    void (*recv_fdata)(Server*,int fd);
};

/**线程相关**/

/* 任务结点 */
typedef struct thread_node {
    void* (*routine)(void*);       /* 任务函数 todo:c++11写法*/
    void* arg;                    /* 传入任务函数的参数 */
    struct thread_node* next;
}thread_node_t;


/*线程池*/
typedef struct thread_pool{
    int shutdown;                    /* 线程池是否销毁 */
    int  max_thr_num;                 /* 最大线程数 */
    pthread_t  *thr_id;                   /* 线程ID数组首地址 */
    thread_node_t    *queue_head;         /* 任务链表队首 */
    thread_node_t    *queue_tail; 	 /* 任务链表队尾 */
    pthread_mutex_t  queue_lock;        /*队列锁*/
    pthread_cond_t   queue_ready;        /*条件变量*/
}thread_pool_t;

/*
 * @brief     创建线程池
 * @param     max_thr_num 最大线程数
 * @return    0: 成功 其他: 失败
 */
int thread_pool_create(int max_thr_num);

/*
 * @brief     销毁线程池
 */
void thread_pool_destroy();

/*
 * @brief     向线程池中添加任务
 * @param     routine 任务函数指针
 * @param     arg 任务函数参数
 * @return    0: 成功 其他:失败
 */
int thread_pool_add_work(void*(*routine)(void*), void *arg);

#endif

