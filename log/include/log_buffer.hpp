#ifndef BUFFER_H
#define BUFFER_H
#include<stdlib.h>
#include<stdio.h>
#include<cstdint>
#include<cstring>
#include<pthread.h>
#include<errno.h>

const int FILE_NAME=128;
const int FILE_DIR=128;

enum BUFFER_STATUS{
	INIT=1,
	FREE=2,
	FULL=3
};///分别代表未写  在写未满 未写已满

using buf_t=BUFFER_STATUS;

struct HeadNode{
	buf_t status;  ///当前节点的状态
	int offset;    ///在文件中的偏移 得用全局的变量进行判定
	int size;      ///在buffer节点中的大小
	char file_name[FILE_NAME];///持久化的文件名称
	char file_dir[FILE_DIR];///持久化的文件名称
};

const int headnode_size=sizeof(HeadNode);

/*封装buffer节点 构造成双向链表 通过char*数组保存实际数据 used_len total_len控制*/
class Buffer{
	public:
		Buffer* prev;
		Buffer* next;

		uint32_t total_len;
		uint32_t used_len;
		
		char* data;///实际空间：使用char*进行存储 存储的是字符

        pthread_mutex_t m_mutex;//全局锁
       // pthread_cond_t m_cond;///通知相关的
        // pthread_once_t m_once;///用于通知一次

public:
		///不可复制
		Buffer(const Buffer&)=delete;
		Buffer& operator=(const Buffer&)=delete;
	
		///提供buffer大小和持久化的文件
		Buffer(uint32_t len):
			prev(NULL),
			next(NULL),
			total_len(len),
			used_len(headnode_size){

            m_mutex=PTHREAD_MUTEX_INITIALIZER;
			if(total_len<headnode_size)total_len=headnode_size;
			data=new char[len];///headnode数据由生产者进行初始化
			if(!data){
				fprintf(stderr,"缓冲区分配失败!\n");
				exit(1);
			}
			///问题：struct头部长度是否是固定的
			/* 应该由log进行初始化
			struct HeadNode* headnode=(HeadNode*)malloc(headnode_size);
			bzero(headnode,headnode_size);
			headnode->status=INIT;///未初始化
			headnode->offset=0;
			headnode->size=0;
			strcpy(headnode->file_name,"");///开始赋值为空 后面交给生产者进行赋值
			memcpy(data,headnode,headnode_size);///赋值给节点
            */
		}

		~Buffer(){
            pthread_mutex_destroy(&m_mutex);
            delete[] data;
            data= nullptr;///再次访问时可以显式的以空指针挂掉
		}

		bool try_lock(){
		    int err= pthread_mutex_trylock(&m_mutex);
		    if(EBUSY==err)return false;
		    return true;
		}

		void unlock(){
            pthread_mutex_unlock(&m_mutex);
		}

		uint32_t available_len()const{
			return total_len-used_len;
		}


		int getUsedlen()const{
			return used_len;
		}

		char* getdata()const{
			return data+headnode_size;
		}

		bool empty()const{
			return used_len==headnode_size;
		}

		bool append(const char* append_log,uint32_t len){
			if(available_len()<len){
			    printf("节点可用空间不足 退出\n");
				return false;///没有分配成功返回false
			}
			memcpy(data+used_len,append_log,len);///追加len长度的日志
			used_len+=len;
			printf("节点追加到缓冲区成功，使用长度为%d\n",used_len);
			return true;
		}

		void clear(){
			used_len=headnode_size;
			setINIT();	
		}

		void setINIT(){
			struct HeadNode* headnode=(HeadNode*)malloc(headnode_size);
			bzero(headnode,headnode_size);
			headnode->status=INIT;
			headnode->offset=0;
			headnode->size=0;
			strcpy(headnode->file_name,"");
			strcpy(headnode->file_dir,"");
			memcpy(data,headnode,headnode_size);///赋值给节点
		}

		void setFREE(){
			struct HeadNode* headnode=(HeadNode*)malloc(headnode_size);
			bzero(headnode,headnode_size);
			headnode->status=FREE;
			headnode->offset=0;
			headnode->size=0;
			strcpy(headnode->file_name,"");
			strcpy(headnode->file_dir,"");
			memcpy(data,headnode,headnode_size);///赋值给节点
		}

		void setFULL(int offset,int size,char* dir, char* name ){
			struct HeadNode* headnode=(HeadNode*)malloc(headnode_size);
			bzero(headnode,headnode_size);
			headnode->status=FULL;
			headnode->offset=offset;
			headnode->size=size;
			strcpy(headnode->file_name,name);
			strcpy(headnode->file_dir,dir);
			memcpy(data,headnode,headnode_size);///赋值给节点
		}

		int try_write(FILE* fp){///需要外部进行fp偏移的设定 同时fp由外部进行关闭
			//将data中used_len长度的数据写入fp中
		//	uint32_t try_len=fwrite(data,headnode_size,used_len,fp);
			//int ret=fseek(fp,used_len,SEEK_SET);
			uint32_t try_len=fwrite(data+headnode_size,1,used_len-headnode_size,fp);
			printf("节点写入文件流大小:%d",try_len);
			if(try_len!=used_len-headnode_size){
				fprintf(stderr,"没有写入fp%u长度的数据",try_len);
				return -1;
			}
			return try_len;
		}

};

#endif