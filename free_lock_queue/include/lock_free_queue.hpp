#ifndef __LCKFREE_H__
#define __LCKFREE_H__

using namespace std;
 
template<typename T>
class CLockFreeQueue {
  private:
   struct LinkNode {
   T m_val;
   LinkNode* next=nullptr;
   LinkNode* prev=nullptr;
   
   LinkNode():m_val(T()),prev(nullptr),next(nullptr){};

   LinkNode(T& val):m_val(val),prev(nullptr),next(nullptr){};
};

 public:
  using LinkNode_t=struct LinkNode;

  CLockFreeQueue():m_length(0){
      ///做成链表
      m_head=new LinkNode_t;
      m_tail=new LinkNode_t;
      m_head->next=m_tail;
      m_tail->prev=m_head;

  }

  ~CLockFreeQueue(){
       ///释放内存空间
      LinkNode_t* node=m_head;
      while(m_head->next!=m_tail){
	      node=m_head;
	      m_head=m_head->next;
	      m_head->prev=nullptr;
	      node->next=nullptr;
	      delete node;
      }
      m_head->next=nullptr;
      m_tail->prev=nullptr;
      delete m_head;
      delete m_tail;
  }
 
  bool push(const T &val){
      LinkNode_t * node = new LinkNode(val);

      do {
	///插到队尾 原子操作
	while(__sync_bool_compare_and_swap(&(node->prev),nullptr,m_tail->prev)!=true);
	while(__sync_bool_compare_and_swap(&(m_tail->prev->next),m_tail,node)!=true);
	while(__sync_bool_compare_and_swap(&(node->next),nullptr,m_tail)!=true);
        while(__sync_bool_compare_and_swap(&(m_tail->prev),node->prev,node) != true);
	m_length++;
	}while(0);

      ///返回false的情况 TODO
      return true;
  }

  T pop(){
      LinkNode_t * node=nullptr;
      do{
	      ///需要考虑为空的情况
	   if(m_head->next==m_tail)return T();
	   while(__sync_bool_compare_and_swap(&(node),nullptr,m_tail->prev)!=true);
	   while(__sync_bool_compare_and_swap(&(m_tail->prev),node,node->prev)!=true);
	   while(__sync_bool_compare_and_swap(&(node->prev->next),node,m_tail)!=true);
	   m_length--;
	   node->prev=nullptr;
	   node->next=nullptr;
      } while(0);

      return node->m_val;
  }

unsigned int getsize()const{
	return m_length.load();
}

  bool empty()const{
      return m_length.load()==0;
  }


 private:
  LinkNode_t * m_head;
  LinkNode_t * m_tail;
  std::atomic<unsigned int>m_length;
};
 
#endif
