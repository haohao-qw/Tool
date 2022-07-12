#include"async_log.hpp"
#include<iostream>

int main(){

	LOG_INIT("./log","t",INFO);
	///大量日志才能打印
	int i=1e6;
	while(i--){
		LOG_INFO("tfddddddddddddddddddddddddddddddddddddddddest....\n");
	}
	return 0;
}

