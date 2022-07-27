#include<assert.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<stdarg.h>

#include "async_freelock_log.hpp"

const int LOG_LEN=512;
int BUF_LEN_=16*1024*1024;

static pid_t Getpid(){
	return syscall(SYS_gettid);
};

////初始化静态成员变量，静态成员初始化优先类构造 因此要先进行初始化
//pthread_mutex_t async_freelock_log::m_mutex=PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t async_freelock_log::m_cond=PTHREAD_COND_INITIALIZER;
//once初始化
pthread_once_t async_freelock_log::sm_once=PTHREAD_ONCE_INIT;
//静态变量类外初始化
async_freelock_log* async_freelock_log::sm_instance=nullptr;


/*完成初始化 主要工作室完成buffer初始化 一开始三个buffer指针是指向同一个地方 做成环*/
async_freelock_log::async_freelock_log():
            m_buf_count(3),
			m_buflen(BUF_LEN_),
			m_product(nullptr),
			m_log_cnt(0),///保存了多少条日志
			m_level(INFO),
			m_tm(){
    m_pool=new BufferPool(3);
	m_product=new Buffer(m_buflen);
	m_product->setFREE();
	m_pid=Getpid();
	m_pool->setLog(this);
}

void async_freelock_log::set_path(const char* log_dir,const char* prog_name,int level){
	strncpy(m_log_dir,log_dir,strlen(log_dir));//初始化日志文件路径
	strncpy(m_prog_name,prog_name,strlen(prog_name));
	string key_dir=m_log_dir;
	string key_name=m_prog_name;
	string key=key_dir+"/"+key_name;
	dirname=key;
  //  printf("dirname%s\n",dirname.c_str());
    hash[key]=0;///一开始偏移长度为0

  //  printf("文件名:%s\n",dirname.c_str());
    File[dirname]=fopen(dirname.c_str(),"w");///不存在就新创建一个 TODO：问题：可否被多个线程打开

    fseek(File[dirname],0,SEEK_SET);
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
    if(hash.count(key)){
        printf("%s已经创建\n",dirname.c_str());
        return;
    }
}

///消费者做的事儿
///逻辑：将m_persist_buf中的内容写入到指定fp中
void async_freelock_log::consumer(Buffer* node){
		///持久化操作：读取节点 拿到文件信息后进行读写即可
		struct HeadNode* head=(HeadNode*)malloc(headnode_size);
		bzero(head,headnode_size);
		memcpy(head,node->data,headnode_size);
		printf("文件内容:%s %s %d %d\n",head->file_name,head->file_dir,head->status,head->offset);
		FILE* fp=File[dirname];

//        printf("消费者逻辑:拿到 dirname:%s\n",dirname.c_str());
		if(fp==NULL){
			printf("fpoen %s error\n",head->file_name);
			return ;
		}
        int ret=fseek(fp,head->offset-m_first_offet,SEEK_SET);
		if(ret==-1){
            printf("fseek error\n");
		    return ;
		}
		int flag=node->try_write(fp);///开始写入数据
		if(flag==-1){
            printf("文件try_write失败\n");
            return ;
		}

        printf("文件try_write成功%d字节\n",flag);
        flag= fseek(File[dirname],flag,SEEK_SET);
		if(flag==-1){
            printf("error:fseek diff\n");
            return ;///失败
		}
		node->clear();
		return;
}
///第一个字符为等级 后面根据设定更改输出
int async_freelock_log::try_append(const char*lvl,const char* format,va_list args){
    printf("tryappend\n");
	int ms=0;
	uint64_t cur_sec=m_tm.get_cur_time(&ms);///使用指针传出数据
	char log_line[LOG_LEN];
	///直接拿到时间和写入长度  LOG_LEN表示最大写入大小
	int prev_len=snprintf(log_line,LOG_LEN,"%s[%s.%03d]",lvl,m_tm.time_fmt,ms);

	int len=vsnprintf(log_line+prev_len,LOG_LEN-prev_len,format,args);
	//va_end(arg_ptr);//完成内容的嵌入
	uint32_t Len=len+prev_len; ///总共写入到大小
	if(m_product->available_len()>=Len){
		///空闲长度够用 写入即可
		m_product->append(log_line,Len);
		hash[dirname]+=Len;
		return 0;
	}else{
		///长度不够返回0 返回前对于数据头进行格式化
		if(m_product->used_len-headnode_size==hash[dirname])m_first_offet=hash[dirname];
		m_product->setFULL(hash[dirname],m_product->getUsedlen(),m_log_dir,m_prog_name);
		///TODO:进行通知
		return -1;
	}
}

void async_freelock_log::Write(const char* lvl,const char* format,...){
    ///注意：可变参数的嵌套使用
	va_list args;
	va_start(args,format);
	int ret=try_append(lvl,format,args);
	if(ret==0)printf("容量足够，追加成功\n");
	va_end(args);
	if(ret==-1){
		printf("节点不能写入，新创建一个\n");
		m_pool->Push(m_product);
		Buffer* node=new Buffer(m_buflen);
		m_product=node;
		m_product->setFREE();
		va_list args;
		va_start(args,format);
		try_append(lvl,format,args);
		va_end(args);
	}
	return ;
}
