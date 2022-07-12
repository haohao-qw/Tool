#include"async_log.hpp"
#include<unistd.h>

int main(){
	LOG_INIT("log","test",3);//
	LOG_ERROR("test is ok");
}
