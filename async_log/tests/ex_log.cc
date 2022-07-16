#include"async_log.hpp"
#include<iostream>

int main(){
	
	async_log::getinstance()->set_path("./log","log.txt",INFO);

	for(int i=0;i<100;i++)
	async_log::getinstance()->Write("ERROR","nihao%d%s%d%s\n",111,"nofds",124,"fnsdojfosjf");

	printf("节点个数%d\n",async_log::getinstance()->getcount());
	printf("%d\n",async_log::getinstance()->readme(async_log::getinstance()->getnode()));
	
	async_log::getinstance()->persistent();

	//getchar();

	return 0;
}

