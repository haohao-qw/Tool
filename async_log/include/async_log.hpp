#ifndef ASYNC_LOG_H
#define ASYNC_LOG_H
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<stdint.h>
#include<pthread.h>
#include<sys/syscall.h>

#include "timer.hpp"
#include "buffer.hpp"

enum LOG_LEVEL{
	FATAL=1,
	ERROR,
	WARN,
	INFO,
	DEBUG,
	TRACE
};

///这里先只做单生产者
class async_log{
	private:
		///文件名构造：时间_数值.log
		int m_buf_count;   //缓冲区数量尽量多
		Buffer* m_product;  ///生产节点

		FILE* m_fp;//具体文件位置
		pid_t m_pid;//保存具体线程
		
		//日志内容相关
		int m_year,m_mon,m_day;///日志时间
		int m_log_cnt;///日志条数
		
		///设置输出地
		char m_prog_name[512];//日志输出名称
		char m_log_dir[512];//路径

		bool m_env_ok;//路径是否正确
		int m_level;//日志等级
		uint64_t m_last_error_time;///TODO:日志存在error时最后的时间
		
		log_timer m_tm;///时间

		///TODO:设置静态的原因：静态成员属于类而不属于具体某个类，可以实现多个对象共享
		//变量的同时可以不破坏封装 同时能节省内存

		static pthread_mutex_t m_mutex;//全局锁
		static pthread_cond_t m_cond;///通知相关的
		static uint32_t m_buflen;///一块缓冲区的大小
		
		static async_log* m_instance;///单例对象
		static pthread_once_t m_once;///用于通知一次


		async_log();///单例下私有构造函数 不能构造栈上变量

		bool decis_file(int year,int mon,int day);
	public:
		async_log(const async_log&)=delete;
		async_log& operator=(const async_log&)=delete;

	public:
		static async_log* getinstance(){
			///等待初始化条件满足
			pthread_once(&m_once,async_log::init);
			return m_instance;
		}

		static void init(){
			while(!m_instance)m_instance=new async_log();
		}

		void init_path(const char* log_dir,const char* prog_name,int level);

		int get_level()const{return m_level;}

		//进行持久化操作
		void persist();

		void try_append(const char* lvl,const char* format,...);
};

void* be_thdo(void* args);

/***************************************工具宏********************************************/

///设置m_buflen
#define LOG_MEM_SET(arg)\
	do\
	{\
		if(arg<90*1024*1024)arg=90*1024*1024;\
		else if(arg>1024*1024*1024)arg=1024*1024*1024;\
		async_log::m_buflen=arg;\
	}while(0)

///format:[LEVEL][yy-mm-dd h:m:s[tid][file_name]:line (fun_name):content
//创建线程跑运行函数 挂起跑
#define LOG_INIT(log_dir,prog_name,level)\
	do\
	{\
		async_log::getinstance()->init_path(log_dir,prog_name,level);\
		pthread_t tid;\
		pthread_create(&tid,NULL,be_thdo,NULL);\
		pthread_detach(tid);\
	}while(0)

#define LOG_TRACE(fmt,args...)\
	do{\
		async_log::getinstance()->try_append("[TRACE]","[%u]%s:%d(%s):" fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)

#define LOG_INFO(fmt,args...)\
	do{\
		async_log::getinstance()->try_append("[INFO]","[%u]%s:%d(%s):" fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)

#define LOG_NORMAL(fmt,args...)\
	do{\
		async_log::getinstance()->try_append("[NORMAL]","[%u]%s:%d(%s):" fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)


#define LOG_WARN(fmt,args...)\
	do{\
		async_log::getinstance()->try_append("[WARN]","[%u]%s:%d(%s):" fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)

#define LOG_ERROR(fmt,args...)\
	do{\
		async_log::getinstance()->try_append("[ERROR]","[%u]%s:%d(%s):" fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)

#define LOG_DEBUG(fmt,args...)\
	do{\
		async_log::getinstance()->try_append("[DEBUG]","[%u]%s:%d(%s):" fmt " \n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)


#define LOG_FATAL(fmt,args...)\
	do{\
		async_log::getinstance()->try_append("[FATAL]","[%u]%s:%d(%s):" fmt " \n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)




#define TRACE(fmt,args...)\
	do{\
		if(async_log::getinstance()->get_levle()>=INFO)\
		async_log::getinstance()->try_append("[FATAL]","[%u]%s:%d(%s):" fmt " \n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)


#define DEBUG(fmt,args...)\
	do{\
		if(async_log::getinstance()->get_levle()>=INFO)\
		async_log::getinstance()->try_append("[DEBUG]","[%u]%s:%d(%s):" fmt " \n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)



#define INFO(fmt,args...)\
	do{\
		if(async_log::getinstance()->get_levle()>=INFO)\
		async_log::getinstance()->try_append("[INFO]","[%u]%s:%d(%s):" fmt " \n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)


#define NORMAL(fmt,args...)\
	do{\
		if(async_log::getinstance()->get_levle()>=INFO)\
		async_log::getinstance()->try_append("[NORMAL]","[%u]%s:%d(%s):" fmt " \n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)




#define WARN(fmt,args...)\
	do{\
		if(async_log::getinstance()->get_levle()>=INFO)\
		async_log::getinstance()->try_append("[WARN]","[%u]%s:%d(%s):" fmt " \n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)


#define ERROR(fmt,args...)\
	do{\
		if(async_log::getinstance()->get_levle()>=INFO)\
		async_log::getinstance()->try_append("[EROOR]","[%u]%s:%d(%s):" fmt " \n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)



#define FATAL(fmt,args...)\
	do{\
		if(async_log::getinstance()->get_levle()>=INFO)\
		async_log::getinstance()->try_append("[FATAL]","[%u]%s:%d(%s):"fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)


#endif





