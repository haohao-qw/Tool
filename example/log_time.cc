#include "timer.hpp"

int main(){
	log_timer time;
	int out=0;
	printf("当前时间：%d\n",time.get_cur_time(&out));
	printf("当前时间：%d\n",out);
	printf("格式化时间:%s\n",time.get_timefmt());
	return 0;
}
