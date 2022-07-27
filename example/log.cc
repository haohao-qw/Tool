#include"async_log.hpp"
#include<iostream>

int main(){
    ///大量日志可以设置较大buffer空间 减少锁的调用
    ASYLOG_INIT("../tmp/log","log.txt",INFO);
    for(int i=0;i<10000;i++) {
        ASYLOG_INFO("INFO %s %d","TEST",i);
        ASYLOG_FATAL("FATAL %s %d","TEST",i);
        ASYLOG_DEBUG("DEBUG %s %d","TEST",i);
        ASYLOG_ERROR("ERROR %s %d","TEST",i);
        ASYLOG_TRACE("TRACE %s %d","TEST",i);
        ASYLOG_WARN("WARN %s %d","TEST",i);
    }
	return 0;
}