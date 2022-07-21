#ifndef TIMER_H
#define TIMER_H

#include<time.h>
#include<sys/time.h>
#include<unistd.h>
#include<stdint.h>
#include<stdio.h>

/*设计原因：涉及大量时间判定时需要进行系统调用获取时间 可以通过是否在同一秒以及在同一分钟进行优化 减少陷入系统调用的次数*/
class log_timer{
	public:
		///年月日时分
		int m_year;
		int m_mon;
		int m_day;
		int m_hour;
		int m_min;
		int m_sec;

		///提供yyyy-mm-dd hh:mm:ss格式
		char time_fmt[20];
		
		///记录上一条日志时分 减少系统调用
		uint64_t m_sys_min;
		uint64_t m_sys_sec;

	private:
		//snprintf将后面整个字符串格式化至time_fmt中 stdio.h
		void reset_log_fmt(){
			snprintf(time_fmt,20,"%d-%02d-%02d %02d:%02d:%02d",m_year,m_mon,m_day,
									m_hour,m_min,m_sec);
		}
		
		///只更新sec 也就是从char字符串中第18个字符开始设置3个
		void reset_log_fmt_sec(){
			snprintf(time_fmt+17,3,"%02d",m_sec);
		}

	public:
		log_timer(){
			///通过系统调用初始化当前时间
			struct timeval tv;//time.h 两个成员：秒数 微秒数
			gettimeofday(&tv,NULL);//获取当前时间
			m_sys_sec=tv.tv_sec;
			m_sys_min=m_sys_sec/60;//分钟
			
			struct tm cur_time;
			localtime_r((time_t*)&m_sys_sec,&cur_time);//线程安全版本 todo:改为c++20风格

			m_year=cur_time.tm_year+1900;
			m_mon=cur_time.tm_mon+1;
			m_day=cur_time.tm_mday;
			m_hour=cur_time.tm_hour;
			m_min=cur_time.tm_min;
			m_sec=cur_time.tm_sec;

			///初始化至缓冲区buf
			reset_log_fmt();
		}


		/**
		 * @brief 返回当前时间
		 * @param out_tsec
		 * @return
		 */
		uint64_t get_cur_time(int*out_tsec=NULL ){
			struct timeval tv;
			gettimeofday(&tv,NULL);

			if(out_tsec)
				*out_tsec=tv.tv_usec/1000;///根据微秒值拿到
			
			///通过比较减少系统调用 提高系统性能
			if((uint32_t)tv.tv_sec!=m_sys_sec){///通过缓存比较当前时间 不在同一秒
				m_sys_min=tv.tv_sec%60;
				m_sys_sec=tv.tv_sec;
				
				///不在同一分钟
				if(m_sys_sec/60!=m_sys_min){
					m_sys_min=m_sys_sec/60;

					struct tm cur_time;
					localtime_r((time_t*)&m_sys_sec,&cur_time);			
					m_year=cur_time.tm_year+1900;
					m_mon=cur_time.tm_mon+1;
					m_day=cur_time.tm_mday;
					m_hour=cur_time.tm_hour;
					m_min=cur_time.tm_min;
					m_sec=cur_time.tm_sec;
					
					reset_log_fmt();
				}else{
					///同一分钟内不同秒数就只更新秒
					reset_log_fmt_sec();
				}
			}
			return tv.tv_sec;
		}

		/**
		 * @brief 返回实际存储的内容
		 * @return
		 */
		const char* get_timefmt()const{
			return time_fmt;
		}
};

#endif
