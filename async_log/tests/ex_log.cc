#include"async_log.hpp"
#include<iostream>

int main(){
	
	async_log::getinstance()->set_path("./log","test.txt",INFO);
	
	for(int i=0;i<1e5;i++)
	async_log::getinstance()->Write("ERROR","nihao%s\n","fndos");
	
	async_log::getinstance()->persistent();

	getchar();

	return 0;
}

