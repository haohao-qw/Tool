#ifndef __WORK_THREAD_POOL__
#define __WORK_THREAD_POOL__
 
#include <stdio.h>
#include <thread>
#include <queue>
#include <string>
#include <vector>
#include <math.h>
#include <sstream>
#include "lockfree_queue.hpp"
 

template<typename T>
class ThreadPoolBase {
  public:
    /**
     * @brief TODO 初始化配置
     */
  virtual void Init() {};
  /**
   * @brief TODO 结束时优化
   */
  virtual void Finish() {};
  /**
   * @brief 子类必须实现，val是队列中取出的元素，对其进行一系列操作，或是函数执行，或是基础数据进行运算
   * @param val 泛型数据
   */
  virtual void Handle( T &val)=0;///纯虚函数 必须实现
 
 public:
   ThreadPoolBase(int size){
      if (size <= 0||size>=200) { // 最小也需要有1个线程
           m_size = 1;
          } else {
          m_size = size;
          }
   }

   ///必须是虚函数
   virtual ~ThreadPoolBase(){
   }


   /**
    * @brief 可能插入不成功
    * @param val
    * @return
    */
  bool Push(const T &val){
      bool flag=m_queue.push(val);
      return flag;
  }

  /**
   * @brief 初始化线程池
   */
  void Start(){
      for (int i=0; i < m_size; i++) {
          m_threadpool.push_back(std::thread(&ThreadPoolBase<T>::Worker, this));
      }
      return;
  }

  /**
   * @brief 跑线程
   * @return
   */
  int Run(){
      for (int i=0; i < m_size; i++) {
          m_threadpool[i].join();
      }
      return 0;
  }

  unsigned int getsize()const{
	  return m_queue.getsize();
  }

 
 private:
    /**
     * @brief 工作线程
     */
  void Worker(){
      unsigned int val_count = 0;
      while (1) {
	  if(m_queue.empty())break; //TODO：进行自我销毁 采用某种策略动态扩容
          T val = m_queue.pop();///取出任务 存在不能取出任务的情况 内容为空
          if(!val){
              continue;
          }
          ///根据内容进行具体处理
          Handle(val);///任务处理函数 交给子类自定义进行实现 扩展性质
      }
  }

  ///线程池线程个数
  int m_size;
  ///无锁队列结构
  CLockFreeQueue<T> m_queue;
  ///容器封装的线程池
  std::vector<std::thread> m_threadpool;
};



#endif
