#ifndef __WORK_THREAD_POOL__
#define __WORK_THREAD_POOL__
 
#include <stdio.h>
#include <thread>
#include <queue>
#include <string>
#include <vector>
#include <math.h>
#include <sstream>
#include "lock_free_queue.hpp"
 
using namespace std;
 
template<typename T>
class ThreadPoolBase {
  public:
  //子类进行实现 可以进行扩展
  virtual void Init() {};
  virtual void Finish() {};
  virtual void Handle(const T &val)=0;///纯虚函数 必须实现
 
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


  ///将任务或者其他数据类型加入队列
  bool Push(const T &val){
      bool flag=m_queue.push(val);
      return flag;
  }
 
  int Start(){
      for (int i=0; i < m_size; i++) {
          m_threadpool.push_back( thread(&ThreadPoolBase<T>::Worker, this));
      }
      return 0;
  }

  int Stop(){
      for (int i=0; i < m_size; i++) {
          m_threadpool[i].join();
      }
      return 0;
  }

  unsigned int getsize()const{
	  return m_queue.getsize();
  }

 
 private:
  void Worker(){
      unsigned int val_count = 0;
      while (!m_queue.empty()) {
          T val = m_queue.pop();///取出任务  这里是阻塞的
          ///根据内容进行具体处理
          Handle(val);///任务处理函数 交给子类自定义进行实现 扩展性质
      }
  }
 
  int m_size;
  CLockFreeQueue<T> m_queue; //无锁队列
  vector<thread> m_threadpool;
};



#endif
