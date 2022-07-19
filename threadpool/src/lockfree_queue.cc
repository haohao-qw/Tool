#include "lockfree_queue.hpp"

/*
template<typename T>
LckFreeQueue<T>::LckFreeQueue(): m_head(nullptr), m_tail(nullptr), m_empty(true), m_length(0) {
  m_head = new LinkNode;
  m_head->next = nullptr;
  m_tail = m_head;
}
 
template<typename T>
LckFreeQueue<T>::~LckFreeQueue() {
  LinkNode *p = m_head;
  if (p) {
    LinkNode *q = p->next;
    delete p;
    p = q;
  }
}
 
template<typename T>
T LckFreeQueue<T>::push(const T &val) {
  LinkNode * q = new LinkNode;
  q->data = val;
  q->next = nullptr;
 
  LinkNode * p = m_tail;
  LinkNode * oldp = p;
  do {
    while (p->next != nullptr)
        p = p->next;
  } while( __sync_bool_compare_and_swap(&(p->next), nullptr, q) != true); //如果没有把结点链在尾上，再试
 
  __sync_bool_compare_and_swap(&m_tail, oldp, q); //置尾结点
  return 0;
}
 
template<typename T>
T LckFreeQueue<T>::pop() {
  LinkNode * p;
  do{
    p = m_head;
    if (p->next == nullptr){
      return "";
    }
  } while( __sync_bool_compare_and_swap(&m_head, p, p->next) != true );
  return p->next->data;
}
 
template<typename T>
bool LckFreeQueue<T>::empty() {
  return m_empty;
}
*/
