#ifndef CLIENT_H__
#define CLIENT_H__
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <time.h>
#include "File_utils.hpp"

class Client{
private:
    char* mbegin;
public:
    void Run(){
        int sockfd = Client_init(SERVER_IP);

        char filename[FILENAME_MAXLEN] = {0};
        printf("BLOCKSIZE=  %d\n",BLOCKSIZE);
        printf("Input filename : ");
        scanf("%s",filename);
        int fd=0;
        if((fd = open(filename, O_RDWR)) == -1 )
        {
            printf("open erro ！\n");
            exit(-1);
        }

        /*Timer*/
        printf("Timer start!\n");
        time_t t_start, t_end;
        t_start=time(NULL);

        /*发送文件信息*/
        struct stat filestat;
        fstat(fd ,&filestat);
        int last_bs=0;
        struct fileinfo finfo;
        send_fileinfo(sockfd, filename, &filestat, &finfo, &last_bs);

        /*接收Server分配的ID*/
        char id_buf[INT_SIZE] = {0};
        int n=0;
        for(n=0; n<INT_SIZE; n++){
            read(sockfd, &id_buf[n], 1);
        }
        int freeid = *((int *)id_buf);
        printf("freeid = %d\n", freeid);

        /*map，关闭fd*/
        mbegin = (char *)mmap(NULL, filestat.st_size, PROT_WRITE|PROT_READ, MAP_SHARED, fd , 0);
        close(fd);

        /*向任务队列添加任务*/
        int j=0, num=finfo.chunk_num, offset=0;
        pthread_t pid[num];
        memset(pid, 0, num*sizeof(pthread_t));
        int headlen = sizeof(struct filehead);
        ///*文件可以被标准分块*/
        ///offset似乎没变 多个线程发送数据
        if(last_bs == 0){
            for(j=0; j<num; j++){
                struct filehead * p_fhead = create_file_head(filename, freeid, &offset,mbegin);
                if (pthread_create(&pid[j], NULL, send_filedata, (void *)p_fhead) != 0){
                    printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, errno, strerror(errno));
                    exit(-1);
                }
            }
        }
            /*文件不能被标准分块*/
        else{
            for(j=0; j<num-1; j++){
                struct filehead * p_fhead = create_file_head(filename, freeid, &offset,mbegin);
                if (pthread_create(&pid[j], NULL, send_filedata, (void *)p_fhead) != 0){
                    printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, errno, strerror(errno));
                    exit(-1);
                }
            }
            /*最后一个分块*/
            struct filehead * p_fhead = (struct filehead *)malloc(head_len);
            bzero(p_fhead, head_len);
            strcpy(p_fhead->file_name, filename);
            p_fhead->which_con = freeid;
            p_fhead->file_offset = offset;
            p_fhead->chunk_size= last_bs;

            if (pthread_create(&pid[j], NULL, send_filedata, (void *)p_fhead) != 0){
                printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, errno, strerror(errno));
                exit(-1);
            }
        }

        /*回收线程*/
        for (j = 0; j < num; ++j) {
            pthread_join(pid[j], NULL);
        }

        /*计时结束*/
        t_end=time(NULL);
        printf("Master prosess exit!\n");
        printf("共用时%.0fs\n",difftime(t_end,t_start));

    }
/*创建大小为size的文件*/
    static int createfile(char *filename, int size);

/*设置fd非阻塞*/
    static void set_fd_noblock(int fd);

/*初始化Client*/
    static int Client_init( char *ip);

/*发送文件信息，初始化分块头部信息*/
/*last_bs==0:所有分块都是标准分块；flag>0:最后一个分块不是标准分块，last_bs即为最后一块大小*/
    static void send_fileinfo(int sock_fd                  //要发送文件fd
            , char *fname                //filename
            , struct stat* p_fstat       //文件属性结构体
            , struct fileinfo *p_finfo   //返回初始化后的文件信息
            , int *flag);                 //最后一个分块是否时标准分块，0代表是；1代表不是

/*发送文件数据块*/
    static void * send_filedata(void *args);

/*生成文件块头部*/
    static struct filehead * create_file_head(char *filename, int freeid, int *offset,char* begin);

};

#endif
