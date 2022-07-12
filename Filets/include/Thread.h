#ifndef THREAD_POOL_H__
#define THREAD_POOL_H__

#include <pthread.h>
#include "Utils.h"
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

