#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

#include "lock_threadpool.hpp"

static thread_pool_t *tpool = NULL;

/* 工作者线程函数, 从任务链表中取出任务并执行 */
static void* thread_routine(void *arg)
{
    thread_node_t* work;

    while(1) {
        /* 如果任务队列为空,且线程池未关闭，线程阻塞等待任务 */
        pthread_mutex_lock(&tpool->queue_lock);
        ///等待队列中有任务或者线程池关闭
        while(!tpool->queue_head && !tpool->shutdown) {
            pthread_cond_wait(&tpool->queue_ready, &tpool->queue_lock);
        }

        /*查看线程池开关，如果线程池关闭，线程退出*/
        if (tpool->shutdown) {
            pthread_mutex_unlock(&tpool->queue_lock);
            pthread_exit(NULL);
        }

        /*从任务链表中取出任务，执行任务*/
        work = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        pthread_mutex_unlock(&tpool->queue_lock);
        work->routine(work->arg);///回调

        /*线程完成任务后，释放任务*/
        free(work->arg);
        free(work);
    }
    return NULL;
}

/* 创建线程池 */
thread_pool_t *thread_pool_create(int max_thr_num)
{
    int i;
    /*创建进程池结构体*/
    tpool= (thread_pool_t*)calloc(1, sizeof(thread_pool_t));
    if (!tpool) {
        printf("%s: calloc tpool failed\n", __FUNCTION__);
        exit(1);
    }

    /* 初始化任务链表、互斥量、条件变量 */
    tpool->max_thr_num = max_thr_num;
    tpool->shutdown = 0;
    tpool->queue_head = NULL;
    tpool->queue_tail = NULL;
    if (pthread_mutex_init(&tpool->queue_lock, NULL) !=0) {
        printf("%s: pthread_mutex_init failed, errno:%d, error:%s\n",
               __FUNCTION__, errno, strerror(errno));
        exit(-1);
    }
    if (pthread_cond_init(&tpool->queue_ready, NULL) !=0 ) {
        printf("%s: pthread_cond_init failed, errno:%d, error:%s\n",
               __FUNCTION__, errno, strerror(errno));
        exit(-1);
    }

    /* 创建worker线程 */
    tpool->thr_id = (pthread_t*)calloc(max_thr_num, sizeof(pthread_t));
    if (!tpool->thr_id) {
        printf("%s: calloc thr_id failed\n", __FUNCTION__);
        exit(1);
    }
    for (i = 0; i < max_thr_num; ++i) {
        ///绑定入口函数
        if (pthread_create(&tpool->thr_id[i], NULL, thread_routine, NULL) != 0){
            printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, errno, strerror(errno));
            exit(-1);
        }
    }
    return tpool;
}

/* 销毁线程池 */
void thread_pool_destroy()
{
    int i;
    thread_node_t *member;

    if (tpool->shutdown) {
        return;
    }
    /*关闭线程池开关*/
    tpool->shutdown = 1;

    /* 唤醒所有阻塞的线程 */
    pthread_mutex_lock(&tpool->queue_lock);
    pthread_cond_broadcast(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->queue_lock);

    /*回收结束线程的剩余资源*/
    for (i = 0; i < tpool->max_thr_num; ++i) {
        pthread_join(tpool->thr_id[i], NULL);
    }

    /*释放threadID数组*/
    free(tpool->thr_id);

    /*释放未完成的任务*/
    while(tpool->queue_head) {
        member = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        free(member->arg);
        free(member);
    }

    /*销毁互斥量、条件变量*/
    pthread_mutex_destroy(&tpool->queue_lock);
    pthread_cond_destroy(&tpool->queue_ready);

    /*释放进程池结构体*/
    free(tpool);
}

/* 向线程池添加任务 */
int thread_pool_add_work(void*(*routine)(void*), void *arg)
{
    /*work指向等待加入任务链表的任务*/
    thread_node_t *work;

    if (!routine){
        printf("%s:Invalid argument\n", __FUNCTION__);
        return -1;
    }

    work = (thread_node_t*)malloc(sizeof(thread_node_t));
    if (!work) {
        printf("%s:malloc failed\n", __FUNCTION__);
        return -1;
    }
    work->routine = routine;
    work->arg = arg;
    work->next = NULL;

    /*将任务结点添加到任务链表*/
    pthread_mutex_lock(&tpool->queue_lock);
    /*任务链表为空*/
    if ( !tpool->queue_head ) {
//		printf("first work in work-queue\n");
        tpool->queue_head = work;
        tpool->queue_tail = work;
    }
        /*任务链表非空，查询任务链表末尾*/
    else {
//		printf("not first work in work-queue\n");
        tpool->queue_tail->next=work;
        tpool->queue_tail=work;
    }
    /* 通知工作者线程，有新任务添加 */
    pthread_cond_signal(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->queue_lock);
    return 0;
}
