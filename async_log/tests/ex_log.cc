#include"async_log.hpp"
#include<iostream>

int main(){

	LOG_INIT("./log","ttt",INFO);
	///大量日志才能打印
	int i=1e5;
	while(i--){
		if(i==1234)printf("ok\n");
		LOG_INFO("tfddddddddddddddddddddddddddddddddddddddddest....\n");
	}
	return 0;
}

