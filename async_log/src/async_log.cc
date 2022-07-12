#include<assert.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<stdarg.h>
#include "async_log.hpp"

const int BUFF_WAIT_TIME=1;
const int RELOG_TIME=5;
const int LOG_LEN=512;
const int LOG_USE=1024;

const int MEM_USE=3*1024;


static pid_t Getpid(){
	return syscall(SYS_gettid);
}

////初始化静态成员变量，静态成员初始化优先类构造 因此要先进行初始化
pthread_mutex_t async_log::m_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t async_log::m_cond=PTHREAD_COND_INITIALIZER;
pthread_once_t async_log::m_once=PTHREAD_ONCE_INIT;

async_log* async_log::m_instance=nullptr;

uint32_t async_log::m_buflen=1<<8;///256字节

/*完成初始化 主要工作室完成buffer初始化 一开始三个buffer指针是指向同一个地方 做成环*/
async_log::async_log(): m_buf_count(5),
			m_cur_buf(nullptr),
			m_product(nullptr),
			m_fp(nullptr),
			m_log_cnt(0),///保存了多少条日志
			m_env_ok(false),
			m_level(INFO),
			m_last_error_time(0),
			m_tm(){
	Buffer* node=m_product;
	for(int i=0;i<m_buf_count;i++){
		Buffer*
	}
	m_pid=Getpid();
	printf("async_log sucess\n");
}

void async_log::init_path(const char* log_dir,const char* prog_name,int level){
	pthread_mutex_lock(&m_mutex);///锁住的是整个日志
	
	strncpy(m_log_dir,log_dir,512);//初始化日志文件路径
	
	strncpy(m_prog_name,prog_name,512);
	
	mkdir(m_log_dir,0777);///创建目录
	if(access(m_log_dir,F_OK|W_OK)==-1){//检查是否具有文件的读取权限
		fprintf(stderr,"logdir:%s error:%s\n",m_log_dir,strerror(errno));
	}else{
		///到这里日志目录已经构造成功
		m_env_ok=true;
	}

	if(level>TRACE){
		level=TRACE;
	}
	if(level<FATAL){
		level=FATAL;
	}

	m_level=level;

	pthread_mutex_unlock(&m_mutex);
}

///逻辑：将m_persist_buf中的内容写入到指定fp中
void async_log::persist(){
	while(true){
		///只有一个线程能进行持久化操作：TODO：直接单线程效果不是更好吗？
		pthread_mutex_lock(&m_mutex);

		///当前持久化buffer还有空闲 通过设置等待超时避免无意义的等待
		if(m_persist_buf->m_status==Buffer::FREE){
			struct timespec tsp;///秒 纳秒
			struct timeval now;///秒 微秒

			gettimeofday(&now,NULL);///获取当前时间
			tsp.tv_sec=now.tv_sec;
			tsp.tv_nsec=now.tv_usec*1000;//
			tsp.tv_sec+=BUFF_WAIT_TIME;///等待一秒
			pthread_cond_timedwait(&m_cond,&m_mutex,&tsp);//等待一秒
		}

		if(m_persist_buf->empty()){
			///内容为空 没写 跳过持久化下一个
			pthread_mutex_unlock(&m_mutex);
			continue;
		}

		//经过等待后仍然为空 当前不能写入了
		if(m_persist_buf->m_status==Buffer::FREE){
			assert(m_cur_buf==m_persist_buf);///TODO 当前访问缓冲区
			m_cur_buf->m_status=Buffer::FULL;//设置为满 由其它线程写入或者下一次访问时写入
			m_cur_buf=m_cur_buf->next;
		}

		int year=m_tm.m_year,mon=m_tm.m_mon,day=m_tm.m_day;

		pthread_mutex_unlock(&m_mutex);

		///
		if(!decis_file(year,mon,day)){
			continue;
		}
		
		m_persist_buf->try_write(m_fp);///写入操作
		m_persist_buf->clear();
		//持久化后到下一个
		m_persist_buf=m_persist_buf->next;
		pthread_mutex_unlock(&m_mutex);
		printf("persist sucess\n");
	}
}

void async_log::try_append(const char*lvl,const char* format,...){
	int ms=0;
	uint64_t cur_sec=m_tm.get_cur_time(&ms);///使用指针传出数据
	
	if(m_last_error_time&&(cur_sec-m_last_error_time)<RELOG_TIME)return; ///错误
	
	char log_line[LOG_LEN];

	int prev_len=snprintf(log_line,LOG_LEN,"%s[%s.%03d]",lvl,m_tm.time_fmt,ms);

	va_list arg_ptr;
	va_start(arg_ptr,format);

	int len=vsnprintf(log_line+prev_len,LOG_LEN-prev_len,format,arg_ptr);

	va_end(arg_ptr);//完成内容的嵌入

	uint32_t Len=len+prev_len;

	m_last_error_time=0;
	bool flag=false;

	pthread_mutex_lock(&m_mutex);
	if(m_cur_buf->m_status==Buffer::FREE&&m_cur_buf->available_len()>=Len){
		///空闲长度够用
		m_cur_buf->append(log_line,Len);
	}else{
		if(m_cur_buf->m_status==Buffer::FREE){
			m_cur_buf->m_status=Buffer::FULL;//一条都写不下了 后面直接持久化
			Buffer* next=m_cur_buf->next;

			flag=true;

			if(next->m_status==Buffer::FULL){
				///说明下一个到末尾了并且不能开辟新的空间
				if(m_buflen*(m_buf_count+1)>MEM_USE){
					///一个都写不下了
					fprintf(stderr,"no more log space can use\n");
					m_last_error_time=cur_sec;///记录最后一次错误时间
				}else{
					///到末尾但是可以开辟新的空间存放
					Buffer* newbuf=new Buffer(m_buflen);
					++m_buf_count;
					newbuf->prev=m_cur_buf;
					m_cur_buf->next=newbuf;
					newbuf->next=next;///对应于正在持久化的下一个
					next->prev=newbuf;
					m_cur_buf=newbuf;
				}
			}else{
				m_cur_buf=next;
			}
			if(!m_last_error_time){
				m_cur_buf->append(log_line,Len);///没问题就追加日志
			}
		}else{
			m_last_error_time=cur_sec;///当前空间不能继续缓存数据
		}
	}
	pthread_mutex_unlock(&m_mutex);
	if(flag)pthread_cond_signal(&m_cond);///有数据可以进行持久化了 进行通知
	printf("try_append sucess\n");
}////TODO:改进 通知到具体编号



///对于目录如果文件日志过载 重新生成
bool async_log::decis_file(int year,int mon,int day){
	///
	if(!m_env_ok){
		if(m_fp)fclose(m_fp);
		m_fp=fopen("/dev/null","w");///用于丢弃不用的文件流
		return m_fp!=NULL;
	}

	if(!m_fp){
		///持久化到文件中 如果文件流关闭就新建一个
		m_year=year,m_mon=mon,m_day=day;
		char log_path[512]={};
		sprintf(log_path,"%s/%s.%d%02d%02d.%u.log",m_log_dir,m_prog_name,m_year
				,m_mon,m_day,m_pid);
		///文件夹名称根据初始化参数创建
		m_fp=fopen(log_path,"w");
		if(m_fp){
			m_log_cnt++;//写的日志数目
		}
	}
	else if(m_day!=day){///如果日期变更
		///关闭重新生成 并且从1开始生成
		fclose(m_fp);
		char log_path[1024]={};
		m_year=year,m_mon=mon,m_day=day;
		sprintf(log_path,"%s/%s.%d%02d%02d.%u.log",m_log_dir,m_prog_name,m_year
				,m_mon,m_day,m_pid);
		m_fp=fopen(log_path,"w");
		if(m_fp)m_log_cnt=1;

	}else if(ftell(m_fp)>=LOG_USE){
		////文件流已经超过最大使用量
		fclose(m_fp);
		char old_path[1024]={};
		char new_path[1024]={};

		///拆开
		for(int i=m_log_cnt-1;i>=0;i--){
			sprintf(old_path,"%s%s.%d%02d%02d.%u.log.%d",m_log_dir,m_prog_name,
					m_year,m_mon,m_day,m_pid,i);
			sprintf(new_path,"%s%s.%d%02d%02d.%u.log.%d",m_log_dir,m_prog_name,
					m_year,m_mon,m_day,m_pid,i+1);
			rename(old_path,new_path);///重新命名目录
		}
			sprintf(old_path,"%s%s.%d%02d%02d.%u.log.%d",m_log_dir,m_prog_name,
					m_year,m_mon,m_day,m_pid);
			sprintf(new_path,"%s%s.%d%02d%02d.%u.log.%d",m_log_dir,m_prog_name,
					m_year,m_mon,m_day,m_pid);
			rename(old_path,new_path);
			m_fp=fopen(old_path,"w");
			if(m_fp)m_log_cnt+=1;
	}
	return m_fp!=NULL;
}

///每个线程不断循环
void *be_thdo(void* args){
	async_log::getinstance()->persist();
	printf("betodo sucess\n");
	return NULL;
}
