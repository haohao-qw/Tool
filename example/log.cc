#include"async_log.hpp"
#include<iostream>

int main(){
    ///大量日志可以设置较大buffer空间 减少锁的调用
    LOG_INIT("../tmp/log","log.txt",INFO);
    for(int i=0;i<100000;i++) {
        LOG_INFO("INFO %s %d","TEST",i);
        LOG_FATAL("FATAL %s %d","TEST",i);
        LOG_DEBUG("DEBUG %s %d","TEST",i);
        LOG_ERROR("ERROR %s %d","TEST",i);
        LOG_TRACE("TRACE %s %d","TEST",i);
        LOG_WARN("WARN %s %d","TEST",i);
    }
	return 0;
}