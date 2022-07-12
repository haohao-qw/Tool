#include "Utils.h"
#include "client.h"

/*信息交换sockfd*/
int info_fd;
///写client时定义
extern char *mbegin;
extern int port;

/*结构体长度*/
int fileinfo_len = sizeof(struct fileinfo);
socklen_t sockaddr_len = sizeof(struct sockaddr);
int head_len = sizeof(struct filehead);

int createfile(char *filename, int size)
{
	int fd = open(filename, O_RDWR | O_CREAT);
	fchmod(fd, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	lseek(fd, size-1, SEEK_SET);
	write(fd, "", 1);
	close(fd);
	return 0;
}


//创建文件头部信息
struct filehead * create_file_head(char *filename, int conid, int *offset)
{
	
    struct filehead* file_head = (filehead*)malloc(head_len);
    bzero(file_head, head_len);
    strcpy(file_head->file_name, filename);
    file_head->which_con = conid;
    file_head->file_offset = *offset;
    file_head->chunk_size = BLOCKSIZE;
    *offset += BLOCKSIZE;
    return file_head;
}


//发送文件信息
void send_fileinfo(int sock_fd, char *fname, struct stat* p_fstat, struct fileinfo *file_info, int *p_last_bs)
{	
	/*初始化fileinfo*/
    bzero(file_info, fileinfo_len);
    strcpy(file_info->file_name, fname);
    file_info->file_size = p_fstat->st_size;///st_size是文件的实际大小

    /*最后一个分块可能不足一个标准分块*/
    int count = p_fstat->st_size/BLOCKSIZE;
    if(p_fstat->st_size%BLOCKSIZE == 0){
        file_info->chunk_num = count;
    }
    else{
        file_info->chunk_num = count+1;
        *p_last_bs = p_fstat->st_size - BLOCKSIZE*count;//最后剩余大小
    }
    file_info->chunk_size = BLOCKSIZE;

    /*发送type和文件信息*/
    /*逻辑：前4个字节全0,之后是file_info结构体的信息 解析按照结构体进行解析*/
    char send_buf[200]= {0};
    int type=0;
    memcpy(send_buf, &type, INT_SIZE);///发送全0
    memcpy(send_buf+INT_SIZE, file_info, fileinfo_len);///追加到buf
    send(sock_fd, send_buf, fileinfo_len+INT_SIZE, 0);//发送数据

	printf("-------- fileinfo -------\n");
    printf("filename= %s\nfilesize= %d\ncount= %d\nblocksize= %d\n", file_info->file_name,file_info->file_size, file_info->chunk_num, file_info->chunk_size);
	printf("-------------------------\n");
    return;
}

void recv_fileinfo(int sockfd){
	///todo：进行交互反馈
}

//发送文件数据 作为回调使用 args传filehead结构体
void * send_filedata(void *args)
{
    struct filehead * file_head = (struct filehead *)args;
    printf("------- blockhead -------\n");
    printf("filename= %s\nThe filedata id= %d\noffset= %d\nchunk_size= %d\n", file_head->file_name, file_head->which_con, file_head->file_offset, file_head->chunk_size);
    printf("-------------------------\n");

    int sock_fd = Client_init(SERVER_IP);
	//set_fd_noblock(sock_fd);

	/*发送type和数据块头部*/
    	/*和发送头部逻辑不同：前四个字节是全1 同样发送头部信息 依赖于传入内容决定本次头部信息*/
    char send_buf[200]= {0};
    int type=255;
    memcpy(send_buf, &type, INT_SIZE);
    memcpy(send_buf+INT_SIZE, file_head, head_len);
    int sendsize=0, len = head_len+INT_SIZE;
    char *p=send_buf;
    while(1){
	    ///一个一个字节发送?
        if((send(sock_fd, p, 1, 0) >0)){
            ++sendsize;
            if(sendsize == len)
                break;
            ++p;
        }
    }
//	printf("head_len = %d ; send head: sendsize = %d\n",head_len, sendsize);

	/*发送数据块*/
	printf("Thread : send filedata\n");
	int i=0, send_size=0, num=file_head->chunk_size/SEND_SIZE;
	//todo:这里没对多余数据进行处理 改进：对于多于数据最后发送 
	char *fp=mbegin+file_head->file_offset;///拿到偏移
	for(i=0; i<num; i++){
		if( (send_size = send(sock_fd, fp, SEND_SIZE, 0)) == SEND_SIZE){
			fp+=SEND_SIZE;
//			printf("fp = %p ; a SEND_SIZE ok\n", fp);
		}
		else{
//			printf("send_size = %d ;  a SEND_SIZE erro\n",send_size);
		}
	}

	printf("### send a fileblock ###\n");
    close(sock_fd);
	free(args);
    return NULL;
}


int Client_init(char *ip)
{
    //创建socket
    int sock_fd = socket(AF_INET,SOCK_STREAM, 0);

    //构建地址结构体
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //连接服务器
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sockaddr_len) < 0)
    {
        perror("connect");
        exit(-1);
    }
    return sock_fd;
}


void set_fd_noblock(int fd)
{
    int flag=fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag | O_NONBLOCK);
	return;
}

