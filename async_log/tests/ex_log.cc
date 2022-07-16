#include"async_log.hpp"
#include<iostream>

int main(){
    ///大量日志可以设置较大buffer空间 减少锁的调用
    LOG_INIT("./log","log",INFO);
    for(int i=0;i<100000;i++) {
        LOG_INFO("test is ok %s%d", "nihao", 100);
        LOG_DEBUG("buffer %s%d%f\n","test...",123,12.33);
    }
    getchar();
    printf("成功\n");


	return 0;
}