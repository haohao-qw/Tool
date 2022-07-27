#ifndef ASYNC_ASYLOG_H
#define ASYNC_ASYLOG_H
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<sys/types.h>
#include<stdint.h>
#include<pthread.h>
#include<sys/syscall.h>
#include<unordered_map>
#include <stdarg.h>

#include "log_timer.hpp"
#include "log_buffer.hpp"
#include "lockfree_threadpool.hpp"

using std::string;
///致命 错误 警告 初始 调试 跟踪 根据不同的设定输出
enum ASYLOG_LEVEL{
	FATAL=1,
	ERROR,
	WARN,
	INFO,
	DEBUG,
	TRACE
};

#define prog_name_len 128
#define log_dir_path_size 128

class async_freelock_log{
private:
    class BufferPool:public ThreadPoolBase<Buffer*>{
    private:
        async_freelock_log* log;
    public:
        BufferPool(int size): ThreadPoolBase(size){};

        void setLog(async_freelock_log* log){
            this->log=log;
        }

        void Handle(Buffer* &val){
                log->consumer(val);
        }
    };
	private:
        ///文件对应的偏移
		std::unordered_map<string,int>hash;
        ///文件名对应的打开的文件指针
        std::unordered_map<string,FILE*>File;
        ///对应的路径+文件名 可以定位到目录中的文件
		string dirname;
		///用于结合hash校正每一个文件节点的正确偏移
		int m_first_offet;
		///节点缓冲区个数，与性能有关
		int m_buf_count;
		///单个节点缓冲区大小，与效率性能有关
		uint32_t m_buflen;
		///生产者对应的节点
		Buffer* m_product;  ///生产节点
        ///保存线程id
		pid_t m_pid;
		///日志内容相关
		int m_year,m_mon,m_day;///日志时间
		int m_log_cnt;///日志条数 TODO
		
		///设置输出地
		char m_prog_name[prog_name_len];//日志输出名称
		char m_log_dir[log_dir_path_size];//路径
        ///日志等级
		int m_level;
		///UTC时间
		log_timer m_tm;
		///线程池
		BufferPool* m_pool;

		///TODO:设置静态的原因：静态成员属于类而不属于具体某个类，可以实现多个对象共享
		static pthread_mutex_t sm_mutex;//全局锁
		static pthread_cond_t sm_cond;///通知相关的
		static async_freelock_log* sm_instance;///单例对象
		static pthread_once_t sm_once;///用于通知一次

	private:
		async_freelock_log();///单例下私有构造函数 不能构造栈上变量
	public:
		Buffer* getnode(){
			return m_product;
		}

		void Start(){
		    m_pool->Start();
		}

		//读取节点状态 0：FULL 1:INIT 2:FREE
		int readme(Buffer* node){
			char* str=new char[4];
			strncpy(str,node->data,4);
			buf_t ret=*(buf_t*)str;
			if(ret==FULL)return 3;
			else if(ret==FREE)return 2;
			else return 1;
		}

	public:
		async_freelock_log(const async_freelock_log&)=delete;
		async_freelock_log& operator=(const async_freelock_log&)=delete;
        ~async_freelock_log(){
            for(auto file:File){
                fclose(file.second);
            }
            File.clear();
            hash.clear();
        }

	public:

        int getcount()const{
            return m_buf_count;
        }

        ///单例模式
		static async_freelock_log* getinstance(){
			pthread_once(&sm_once,async_freelock_log::init);
			return sm_instance;
		}

		static void init(){
			while(!sm_instance)sm_instance=new async_freelock_log();
		}

		/**
		 * #+@brief 初始化目录 文件名 等级
		 * @param log_dir
		 * @param prog_name
		 * @param level
		 */
		void set_path(const char* log_dir,const char* prog_name,int level);

		/**
		 * @brief 获取日志等级
		 * @return
		 */
		int get_level()const{return m_level;}

		/**
		 *@brief 持久化操作
		 * @param node
		 * @return
		 */
		void consumer(Buffer* node);

		/**
		 * @brief 封装一层的写函数
		 * @param lvl
		 * @param format
		 * @param ...
		 */
		void Write(const char* lvl,const char* format,...);

		/**
		 * @brief 里面的写逻辑,采用可变参数
		 * @param lvl
		 * @param format
		 * @param args
		 * @return
		 */
		int try_append(const char* lvl,const char* format,va_list args);//外面逻辑由consumer进行
};

/***************************************工具宏********************************************/

///设置m_buflen
#define ASYLOG_SET_BUFLEN(arg)\
	do\
	{\
		if(arg<1*1024)arg=1*1024;\
		else if(arg>1024*1024)arg=1024*1024;\
		BUF_LEN=arg; 			    \
	}while(0)

///format:[LEVEL][yy-mm-dd h:m:s[tid][file_name]:line (fun_name):content
#define ASYLOG_INIT(log_dir,prog_name,level)\
	do\
	{\
		async_freelock_log::getinstance()->set_path(log_dir,prog_name,level); \
	}while(0)

#define ASYLOG_START()\
	do\
	{\
		async_freelock_log::getinstance()->Start(); \
	}while(0)


#define ASYLOG_FATAL(fmt,args...)\
	do{\
		async_freelock_log::getinstance()->Write("[INFO]","[%u]%s:%d(%s):" fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)

#define ASYLOG_ERROR(fmt,args...)\
	do{\
		async_freelock_log::getinstance()->Write("[ERROR]","[%u]%s:%d(%s):" fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)

#define ASYLOG_WARN(fmt,args...)\
	do{\
		async_freelock_log::getinstance()->Write("[WARN]","[%u]%s:%d(%s):" fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)

#define ASYLOG_INFO(fmt,args...)\
	do{\
		async_freelock_log::getinstance()->Write("[INFO]","[%u]%s:%d(%s):" fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)

#define ASYLOG_DEBUG(fmt,args...)\
	do{\
		async_freelock_log::getinstance()->Write("[DUBUG]","[%u]%s:%d(%s):" fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)

#define ASYLOG_TRACE(fmt,args...)\
	do{\
		async_freelock_log::getinstance()->Write("[TRACE]","[%u]%s:%d(%s):" fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}while(0)


#define FATAL(fmg,args...)\
	do{\
		if(async_freelock_log::getinstance()->get_level()>=FATAL)\
		async_freelock_log::getinstance()->Write("[FATAL]","[%u]%s:%d(%s): " fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}\
		while(0)

#define ERROR(fmg,args...)\
	do{\
		if(async_freelock_log::getinstance()->get_level()>=ERROR)\
		async_freelock_log::getinstance()->Write("[ERROR]","[%u]%s:%d(%s): " fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}\
		while(0)

#define WARN(fmg,args...)\
do{\
	if(async_freelock_log::getinstance()->get_level()>=WARN)\
	async_freelock_log::getinstance()->Write("[WARN]","[%u]%s:%d(%s): " fmt "\n",\
			getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
}\
	while(0)

#define INFO(fmg,args...)\
do{\
	if(async_freelock_log::getinstance()->get_level()>=INFO)\
	async_freelock_log::getinstance()->Write("[INFO]","[%u]%s:%d(%s): " ,"\n",\
			getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
}\
	while(0)




#define DUBUG(fmg,args...)\
	do{\
		if(async_freelock_log::getinstance()->get_level()>=DEBUG)\
		async_freelock_log::getinstance()->Write("[DEBUG]","[%u]%s:%d(%s): " fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}\
		while(0)

#define TRACE(fmg,args...)\
	do{\
		if(async_freelock_log::getinstance()->get_level()>=TRACE)\
		async_freelock_log::getinstance()->Write("[TRACE]","[%u]%s:%d(%s): " fmt "\n",\
				getuid(),__FILE__,__LINE__,__FUNCTION__,##args);\
	}\
		while(0)


#endif

