#include "lockfree_threadpool.hpp"
#include<string>
#include <sstream>
#include <math.h>
#include<functional>
#include <stdio.h>

void fun(int i){
	printf("%d  test is ok....\n",i);
}

class work:public ThreadPoolBase<std::function<void(void)>>{
	public:
		work(int size):ThreadPoolBase(size){};

		///val是队列拿到的值
		void Handle(const std::function<void(void)>& val){
			val();
		}
};

class it:public ThreadPoolBase<int>{
	public:
		it(int size):ThreadPoolBase(size){};

		void Handle(const int& val){
			printf("num is:%d\n",val);
		}
};

int main(){
	work pool(8);
	printf("start....\n");
	for(int i=0;i<1000000;i++){
		pool.Push(std::bind(fun,i));
	}
	pool.Start();
	pool.Run();
//	it pool(8);
//	printf("start...\n");
//	for(int i=0;i<1000000;i++){
//		pool.Push(i);
//	}
//	pool.Start();
//	pool.Run();
//	return 0;
}

