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

int BUF_LEN=1024;

static pid_t Getpid(){
	return syscall(SYS_gettid);
}

////初始化静态成员变量，静态成员初始化优先类构造 因此要先进行初始化
pthread_mutex_t async_log::m_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t async_log::m_cond=PTHREAD_COND_INITIALIZER;
pthread_once_t async_log::m_once=PTHREAD_ONCE_INIT;

async_log* async_log::m_instance=nullptr;


/*完成初始化 主要工作室完成buffer初始化 一开始三个buffer指针是指向同一个地方 做成环*/
async_log::async_log(): m_buf_count(5),
			m_buflen(BUF_LEN),
			m_product(nullptr),
			m_log_cnt(0),///保存了多少条日志
			m_level(INFO),
			m_tm(){
	m_product=new Buffer(m_buflen);
	Buffer* node=m_product;
	for(int i=0;i<m_buf_count;i++){
		Buffer* tmp=new Buffer(m_buflen);
		node->next=tmp;
		tmp->prev=node;
		node=node->next;
	}
	node->next=m_product;
	m_product->prev=node;
	m_pid=Getpid();
}

void async_log::set_path(const char* log_dir,const char* prog_name,int level){
	strncpy(m_log_dir,log_dir,strlen(log_dir));//初始化日志文件路径
	strncpy(m_prog_name,prog_name,strlen(prog_name));
	string key_dir=m_log_dir;
	string key_name=m_prog_name;
	string key=key_dir+"_"+key_name;
	dirname=key;
	hash[key]=0;///一开始偏移长度为0
	mkdir(m_log_dir,0777);///创建目录
	if(access(m_log_dir,F_OK|W_OK)==-1){//检查是否具有文件的读取权限
		fprintf(stderr,"logdir:%s error:%s\n",m_log_dir,strerror(errno));
		exit(1);
	}
	if(level>TRACE){
		level=TRACE;
	}
	if(level<FATAL){
		level=FATAL;
	}
	m_level=level;
}

///消费者做的事儿
///逻辑：将m_persist_buf中的内容写入到指定fp中
int async_log::consumer(Buffer* node){
		///持久化操作：读取节点 拿到文件信息后进行读写即可
		struct HeadNode* head=(HeadNode*)malloc(headnode_size);
		bzero(head,headnode_size);
		memcpy(head,node->data,headnode_size);
		FILE* fp=fopen(head->file_name,"w");///不存在就新创建一个 TODO：问题：可否被多个线程打开
		if(fp==NULL){
			printf("fpoen %s error\n",head->file_name);
			return 1;
		}
		int ret=fseek(fp,head->offset,SEEK_SET);
		if(ret!=0){
			printf("fseek error\n");
			return 1;
		}
		node->try_write(fp);///开始写入数据
		fclose(fp);
		return 0;
}

void async_log::persistent(){
	Buffer* node=m_product;
	while(1){
		int ret=readme(node);
		if(ret==1||ret==2){
			node=node->next;
			continue;
		}
		///可以进行持久化 TODO:把锁放到具体某个节点上
		pthread_mutex_lock(&m_mutex);
		ret=consumer(node);
		if(1==ret){
			//TODO:错误处理
			continue;
		}
		///持久化成功后 初始化节点
		node->clear();
	}
}


///第一个字符为等级 后面根据设定更改输出
//这里只完成了对product的写逻辑 TODO:再此基础上封装一下，当写满以及对于当前节点不为INI状态的逻辑
int async_log::try_append(const char*lvl,const char* format,...){
	int ms=0;
	uint64_t cur_sec=m_tm.get_cur_time(&ms);///使用指针传出数据
	char log_line[LOG_LEN];
	///直接拿到时间和写入长度  LOG_LEN表示最大写入大小
	int prev_len=snprintf(log_line,LOG_LEN,"%s[%s.%03d]",lvl,m_tm.time_fmt,ms);
	va_list arg_ptr;
	va_start(arg_ptr,format);
	//第一个：指针指向 第二个：指定大小 第三个：格式化参数 第四个：可变参数列表
	//比如 test(const char* format,...); 
	//test("%d%s",5,"world")
	int len=vsnprintf(log_line+prev_len,LOG_LEN-prev_len,format,arg_ptr);
	va_end(arg_ptr);//完成内容的嵌入
	uint32_t Len=len+prev_len; ///总共写入到大小
	if(m_product->available_len()>=Len){
		///空闲长度够用 写入即可
		m_product->append(log_line,Len);
		hash[dirname]+=Len;
		return 0;
	}else{
		///长度不够返回0 返回前对于数据头进行格式化
		struct HeadNode* head=(HeadNode*)malloc(headnode_size);
		bzero(head,headnode_size);
		head->status=FULL;
		head->offset=hash[dirname];
		head->size=m_product->used_len;///TODO:这个值由成员变量记录
		strcpy(head->file_name,m_prog_name);
		strcpy(head->file_dir,m_log_dir);
		memcpy(m_product->data,head,headnode_size);
		///TODO:进行通知
		return -1;
	}
}



///每个线程不断循环
void* be_thdo(void* args){
	async_log::getinstance()->persistent();
	return NULL;
}
