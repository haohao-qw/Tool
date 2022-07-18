#include "threadpool.hpp"
 /*
template<typename T>
WorkThreadPool<T>::WorkThreadPool(int size) {
  if (size <= 0) { // 最小也需要有1个线程
    m_size = 1;
  } else {
    m_size = size;
  }
}
template<typename T>
WorkThreadPool<T>::~WorkThreadPool() {
 
}
 
template<typename T>
int WorkThreadPool<T>::Push(const T &val) {
  m_queue.push(val);
  return 0;
}


template<typename T>
void WorkThreadPool<T>::Worker() {
  unsigned int val_count = 0;
  while (1) {
    T val = m_queue.pop();///取出任务

    ///TODO:改下任务处理逻辑
    *//*
    if (val.empty()) {
      printf("no val got, sleep for 0.1 sec\n");
      usleep(100); // 0.1 sec
      continue;
    }
 
    if (static_cast<string>(val) == "__exit__") {
      stringstream ss;
      ss << "exit worker: " << std<T>::this_thread::get_id() << ", processed: " << val_count << "..";
      printf("%s\n", ss.str().c_str());
      return;
    }
*//*

    ///根据内容进行具体处理
    Handle(val);
    val_count++;
    if (val_count % 1000 == 0) {
      printf("every 1000 val count\n");
    }
  }
}

template<typename T>
int WorkThreadPool<T>::Start() {
  for (int i=0; i < m_size; i++) {
    m_threadpool.push_back( thread(&WorkThreadPool<T>::Worker, this) );
  }
  return 0;
}
 
template<typename T>
int WorkThreadPool<T>::Stop() {
  for (int i=0; i < m_size; i++) {
    //SendMessage("__exit__");
    //TODO:更改逻辑
  }
  for (int i=0; i < m_size; i++) {
    m_threadpool[i].join();
  }
  return 0;
}
*/