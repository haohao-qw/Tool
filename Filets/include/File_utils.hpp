#ifndef UTILS_H
#define UTILS_H

#include<pthread.h>
#include<sys/socket.h>
//---------------client
#define SERVER_IP   "127.0.0.1"     //server IP

/*客户端指定服务端IP*/
//std::string SERVER_IP="127.0.0.1";

/*指定一次发送大小*/

const int SEND_SIZE4=1<<12;
const int SEND_SIZE8=1<<13;
const int SEND_SIZE16=1<<14;
const int SEND_SIZE32=1<<15;
const int SEND_SIZE64=1<<16;//64k
const int SEND_SIZE128=1<<17;
const int SEND_SIZE256=1<<18;
const int SEND_SIZE512=1<<19;
const int SEND_SIZE1024=1<<20;
//#define SEND_SIZE    32768       	//32K
#define SEND_SIZE    65536       	//64K
//#define SEND_SIZE	131072			//128K
//#define SEND_SIZE	262144			//256K

/*分块大小*/
const int BLOCKSIZE4=1<<12;
const int BLOCKSIZE8=1<<13;
const int BLOCKSIZE16=1<<14;
const int BLOCKSIZE32=1<<15;
const int BLOCKSIZE64=1<<16;
const int BLOCKSIZE128=1<<17;
const int BLOCKSIZE256=1<<18;
const int BLOCKSIZE512=1<<19;
const int BLOCKSIZE1024=1<<20;
//#define BLOCKSIZE   134217728		//128M
//#define BLOCKSIZE   268435456		//256M
#define BLOCKSIZE  	BLOCKSIZE64
//#define BLOCKSIZE	1073741824		//1G


////------------server
/*监听端口*/
const int PORT=8080;
//#define PORT 10000                  //监听端口
/*监听队列大小*/
const int LISTEN_QUEUE_LEN=100;
//#define LISTEN_QUEUE_LEN 100        //listen队列长度
/*线程池大小*/
const int THREAD_NUM=8;
//#define THREAD_NUM  8               //线程池大小
/*支持最大连接数*/
const int CONN_MAX=10;
//#define CONN_MAX  10                //支持最大连接数，一个连接包含多个socket连接（多线程）
/*epool最大监听数目*/
const int EPOLL_SIZE=50;
//#define EPOLL_SIZE  50              //epoll最大监听fd数量
/*文件名最大长度*/
const int FILENAME_MAXLEN=50;
//#define FILENAME_MAXLEN   30        //文件名最大长度
/*inttype长度*/
const int INT_SIZE=4;
//#define INT_SIZE    4               //int类型长度

/*接收数据范围*/
const int RECVBUF_SIZE4=1<<12;//4k
const int RECVBUF_SIZE8=1<<13;//8k
const int RECVBUF_SIZE16=1<<14;//16k
const int RECVBUF_SIZE32=1<<15;//32k
const int RECVBUF_SIZE64=1<<16;//46k
const int RECVBUF_SIZE128=1<<17;//128k


/*一次rece接收数据大小*/
//#define RECVBUF_SIZE    4096        //4K
//#define RECVBUF_SIZE    32768       //32K
//#define RECVBUF_SIZE    131072      //128K
//#define RECVBUF_SIZE    262144      //256K
#define RECVBUF_SIZE    65536       //64K



/*文件信息*/
struct fileinfo{
    int file_size;                       //文件大小
    int chunk_num;                       //分块数量 决定发送次数
    int chunk_size;                      //标准分块大小 一个分块大小
    char file_name[FILENAME_MAXLEN];     //文件名
};
///chunk_num*chunk_size+?=file_size

/*分块头部信息*/
struct filehead{
    int which_con;                             //分块所属文件的id，gconn[CONN_MAX]数组的下标
    int file_offset;                //分块在原文件中偏移
    int chunk_size;         //本文件块实际大小 正常情况和fileinfo中一致 最后一个可能不同
    char file_name[FILENAME_MAXLEN];     //文件名
};


//与客户端关联的连接，每次传输建立一个，在多线程之间共享
//接收到文件信息后应该需要保存在此 todo:
struct keep_con{
    int sockfd;                      //信息交换socket：接收文件信息、文件传送通知client
    int file_size;                     //文件大小
    int chunk_size;                           //分块大小
    int chunk_num;                        //分块数量
    int recv_count;                    //已接收块数量，recv_count == count表示传输完毕
    int used;                         //使用标记，1代表使用，0代表可用
    char *mbegin;                     //mmap起始地址
    char file_name[FILENAME_MAXLEN];   //文件名
};
#define  fileinfo_len  sizeof(struct fileinfo)
#define  head_len      sizeof(struct filehead)
#define  conn_len       sizeof(struct keep_con)
#endif
