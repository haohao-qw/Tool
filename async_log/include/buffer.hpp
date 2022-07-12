#ifndef BUFFER_H
#define BUFFER_H
#include<stdlib.h>
#include<stdio.h>
#include<cstdint>
#include<cstring>

const int FILE_NAME=128;

//只设置可用FREE 和满FULL 两种状态
enum BUFFER_STATUS{
	INIT=1,
	RUN,
	FREE,
	FULL
};///分别代表未写 在写 未满空闲 已满空闲

using buf_t=BUFFER_STATUS;

struct HeadNode{
	buf_t status;///当前节点的状态
	int offset;///在文件中的偏移 得用全局的变量进行判定
	int size;///在buffer节点中的大小
	char file_name[FILE_NAME];///持久化的文件名称
};

const int headnode_size=sizeof(HeadNode);

/*封装buffer节点 构造成双向链表 通过char*数组保存实际数据 used_len total_len控制*/
class Buffer{
	public:
		Buffer* prev;
		Buffer* next;

	private:
		uint32_t total_len;
		uint32_t used_len;
		char* m_filename;
		char* data;///实际空间：使用char*进行存储 存储的是字符


	public:
		///不可复制
		Buffer(const Buffer&)=delete;
		Buffer& operator=(const Buffer&)=delete;
	
		///提供buffer大小和持久化的文件
		Buffer(uint32_t len,char* filename):
			prev(NULL),
			next(NULL),
			total_len(len),
			used_len(headnode_size),
			m_filename(filename){
				data=new char[len];
				if(!data){
					fprintf(stderr,"缓冲区分配失败!\n");
					exit(1);
				}
				struct HeadNode* headnode=(HeadNode*)malloc(headnode_size);
				bzero(headnode,headnode_size);
				headnode->status=INIT;
				headnode->offset=0;
				headnode->size=0;
				strcpy(headnode->file_name,filename);
				memcpy(data,headnode,headnode_size);///赋值给节点
		}

		uint32_t available_len()const{
			return total_len-used_len;
		}


		char* getdata()const{
			return data+headnode_size;
		}

		bool empty()const{
			return used_len==headnode_size;
		}

		bool append(const char* append_log,uint32_t len){
			if(available_len()<len){
				return false;///没有分配成功返回false
			}
			memcpy(data+used_len,append_log,len);///追加len长度的日志
			used_len+=len;
			return true;
		}

		void clear(){
			used_len=headnode_size;
			struct HeadNode* headnode=(HeadNode*)malloc(headnode_size);
			bzero(headnode,headnode_size);
			headnode->status=INIT;
			headnode->offset=0;
			headnode->size=0;
			strcpy(headnode->file_name,m_filename);
			memcpy(data,headnode,headnode_size);///赋值给节点
		}

		bool try_write(FILE* fp){
			//将data中used_len长度的数据写入fp中
			uint32_t try_len=fwrite(data,headnode_size,used_len,fp);
			if(try_len!=used_len){
				fprintf(stderr,"没有写入fp%u长度的数据",try_len);
				return false;
			}
			return true;
		}

};

#endif
