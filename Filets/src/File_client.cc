#include "File_client.hpp"
#include "File_utils.hpp"

char* mbegin;

void Client::Init(){
    Client_init(SERVER_IP);
    InitFile();
}

void Client::InitFile(){
    char filename[FILENAME_MAXLEN] = {0};
    printf("BLOCKSIZE=  %d\n",BLOCKSIZE);
    printf("Input filename : ");
    scanf("%s",filename);
    strcpy(m_filename,filename);
    m_openfd = open(m_filename, O_RDWR);
    if(m_openfd==-1) {
        printf("open file:%s failed,exit\n", m_filename);
        exit(-1);
    }
    fstat(m_openfd,&m_filestat);///初始化m_filestat
}

int  Client::SendFileInfo(){
    int last_bs=0;
    send_fileinfo(m_sockfd, m_filename, &m_filestat, &m_info, &last_bs);///这里完成了info的初始化
    mbegin = (char *)::mmap(NULL, m_filestat.st_size, PROT_WRITE|PROT_READ, MAP_SHARED, m_openfd , 0);
    //close(m_openfd);///映射完成后关闭 是否有必要？
    return last_bs;///最后一个分块是否完整
}

void Client::Transform(int last_bs){///由上面接收
    char id_buf[INT_SIZE] = {0};
    int n=0;
    ///TODO:这里能否在网络环境下快速收到
    for(n=0; n<INT_SIZE; n++){
        read(m_sockfd, &id_buf[n], 1);
    }
    int freeid = *((int *)id_buf);       /*向任务队列添加任务*/
    printf("freeid = %d\n", freeid);

    ///多线程
    int j=0, num=m_info.chunk_num, offset=0;
    pthread_t pid[num];
    memset(pid, 0, num*sizeof(pthread_t));

    struct HeadArg* args=(HeadArg*)malloc(sizeof(HeadArg));
    /*文件可以被标准分块*/
    if(last_bs == 0){
        for(j=0; j<num; j++){
            //TODO:将filehead和m_sockfd封装一下 传入sockfd
            struct filehead * p_fhead = create_file_head(m_filename, freeid, &offset);
            args->head=p_fhead;
            args->sockfd=m_openfd;
            ///创建一系列线程跑send_filedata
            if (pthread_create(&pid[j], NULL, send_filedata, (void *)args) != 0){
                printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, errno, strerror(errno));
                exit(-1);
            }
        }
    }
        /*文件不能被标准分块*/
    else{
        for(j=0; j<num-1; j++){
            struct filehead * p_fhead = create_file_head(m_filename, freeid, &offset);
            args->head=p_fhead;
            args->sockfd=m_openfd;
            if (pthread_create(&pid[j], NULL, send_filedata, (void *)args) != 0){
                printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, errno, strerror(errno));
                exit(-1);
            }
        }
        /*最后一个分块*/
        struct filehead * p_fhead = (struct filehead *)malloc(head_len);
        args->head=p_fhead;
        args->sockfd=m_openfd;
        bzero(p_fhead, head_len);
        strcpy(p_fhead->file_name, m_filename);
        p_fhead->which_con = freeid;
        p_fhead->file_offset = offset;
        p_fhead->chunk_size= last_bs;

        if (pthread_create(&pid[j], NULL, send_filedata, (void *)args) != 0){
            printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, errno, strerror(errno));
            exit(-1);
        }
    }

    /*回收线程*/
    for (j = 0; j < num; ++j) {
        pthread_join(pid[j], NULL);
    }

}




int Client::createfile(char *filename, int size)
{
	int fd = open(filename, O_RDWR | O_CREAT);
	if(fd!=0){
	    return -1;
	}
	//fchmod更改文件权限
	int ret=fchmod(fd, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if(ret!=0){
	    return -1;
	}
	ret=lseek(fd, size-1, SEEK_SET);///移动文件指针
	if(ret==-1){
        return  -1;
	}

	ret=write(fd, "", 1);///TODO:无效操作
	if(ret==-1){
	    return -1;
	}
	close(fd);
	return 0;
}


struct filehead * Client::create_file_head(char *filename, int conid, int* offset)
{
    struct filehead* file_head = (filehead*)malloc(head_len);
    bzero(file_head, head_len);
    strcpy(file_head->file_name, filename);
    file_head->which_con = conid;
    file_head->file_offset =*offset;
    file_head->chunk_size = BLOCKSIZE;
    *offset += BLOCKSIZE;
    return file_head;
}


void Client::send_fileinfo(int sock_fd, char *fname, struct stat* p_fstat, struct fileinfo *file_info, int *p_last_bs)
{
    ///fileinfo:用于发送的文件信息 逻辑：进行初始化并且发送
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
    memcpy(send_buf, &type, INT_SIZE);///发送全0:表示发送的是文件信息 之后的内容就是文件信息
    memcpy(send_buf+INT_SIZE, file_info, fileinfo_len);///追加到buf
    send(sock_fd, send_buf, fileinfo_len+INT_SIZE, 0);//发送数据

	printf("-------- fileinfo -------\n");
    printf("filename= %s\nfilesize= %d\ncount= %d\nblocksize= %d\n", file_info->file_name,file_info->file_size, file_info->chunk_num, file_info->chunk_size);
	printf("-------------------------\n");
    return;
}

void Client::recv_fileinfo(int sockfd){
	///todo：进行交互反馈
}

void* Client::send_filedata(void *args)
{
    ///具体文件由args传入 传入内容是 filehead结构体
    struct HeadArg* arg= (struct HeadArg *)args;
    struct filehead * file_head=arg->head;
    int m_sockfd=arg->sockfd;

    printf("------- blockhead -------\n");
    printf("filename= %s\nThe filedata id= %d\noffset= %d\nchunk_size= %d\n", file_head->file_name, file_head->which_con, file_head->file_offset, file_head->chunk_size);
    printf("-------------------------\n");

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
        if((send(m_sockfd, p, 1, 0) >0)){
            ++sendsize;
            if(sendsize == len)
                break;
            ++p;
        }
    }

	/*发送数据块*/
	printf("Thread : send filedata\n");
	int i=0, send_size=0, num=file_head->chunk_size/SEND_SIZE;
	//todo:这里没对多余数据进行处理 改进：对于多于数据最后发送 
	char *fp=mbegin+file_head->file_offset;///拿到偏移
	for(i=0; i<num; i++){
	    ///连续发送
		if( (send_size = send(m_sockfd, fp, SEND_SIZE, 0)) == SEND_SIZE){
			fp+=SEND_SIZE;
		}
		else{
		}
	}

	printf("### send a fileblock ###\n");
	///socketfd在销毁时关闭
	free(args);
    return NULL;
}


void Client::Client_init(char *ip)
{
    //创建socket
    m_sockfd = socket(AF_INET,SOCK_STREAM, 0);

    //构建地址结构体
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //连接服务器
    socklen_t sockaddr_len=sizeof(sockaddr_in);
    if (connect(m_sockfd, (struct sockaddr *)&server_addr, sockaddr_len) < 0)
    {
        ///已经进行保护 如果这里错误后续就直接退出
        perror("connect");
        exit(-1);
    }
}


void Client::set_fd_noblock(int fd)
{
    int flag=fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag | O_NONBLOCK);
	return;
}

