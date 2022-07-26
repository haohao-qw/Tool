#include "File_lockfreeserver.hpp"

//*
void LockFreeServer::Init(){
    //开启线程池
    m_threadpool->Start();
    /*初始化server，监听请求*/
    m_listenfd = Server_init(PORT);
    socklen_t sockaddr_len = sizeof(struct sockaddr);

    /*epoll*/
    m_epfd = epoll_create(EPOLL_SIZE);
    m_ev.events = EPOLLIN;
    m_ev.data.fd = m_listenfd;
    epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listenfd, &m_ev);
}


//*
void LockFreeServer::RecvTransfrom() {
    while (1) {
        int events_count = epoll_wait(m_epfd, m_events, EPOLL_SIZE, -1);
        int i = 0;
        /*接受连接，添加work到work-Queue*/
        for (; i < events_count; i++) {
            if (m_events[i].data.fd == m_listenfd) {
                int connfd;
                struct sockaddr_in clientaddr;
                socklen_t sockaddr_len = sizeof(struct sockaddr);
                while ((connfd = ::accept(m_listenfd, (struct sockaddr *) &clientaddr, &sockaddr_len)) > 0) {
                    //线程分发
                    std::function<void(void)>Task=std::bind(&LockFreeServer::worker,this,connfd);
                    m_threadpool->Push(Task);//装进队列
                }
            }
        }
    }

}
int LockFreeServer::createfile(char *filename, int size)
{
	int fd = open(filename, O_RDWR | O_CREAT);
	fchmod(fd, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	lseek(fd, size-1, SEEK_SET);
	write(fd, "", 1);///写一个字节？
	close(fd);
	return 0;
}

//*
/*接收文件信息，添加连接到gobal_con[]数组，创建填充文件，map到内存*/
///只调用一次 也就是只完成一次文件的创建 共享内存的映射
void LockFreeServer::recv_fileinfo(int sockfd)
{
     /*接收文件信息*/
    char fileinfo_buf[100] = {0};
    bzero(fileinfo_buf, fileinfo_len);
    int n=0;
    ///todo：接受多个字节提升效率
    for(n=0; n<fileinfo_len; n++){
        recv(sockfd, &fileinfo_buf[n], 1, 0);
    }

    struct fileinfo finfo;
    memcpy(&finfo, fileinfo_buf, fileinfo_len);

    printf("------- fileinfo -------\n");
    printf("filename = %s\nfilesize = %d\ncount = %d\nbs = %d\n", finfo.file_name, finfo.file_size, finfo.chunk_num, finfo.chunk_size);
    printf("------------------------\n");

    ///接收完文件头信息后立即mmap
    /*创建填充文件，map到虚存*/
    char filepath[100] = {0};
    strcpy(filepath, finfo.file_name);
    createfile(filepath, finfo.file_size);
    int fd=0;
    if((fd = open(filepath, O_RDWR)) == -1 )
    {
		printf("open file erro\n");
		exit(-1);
    }
 //   printf("fd = %d\n", fd);
 //   拿到文件信息后即可创建一个mmap进行映射
    char *map = (char *)mmap(NULL, finfo.file_size, PROT_WRITE|PROT_READ, MAP_SHARED, fd , 0);
 //   printf("mbegin = %p\n", map);
    close(fd);

    /*向gconn[]中添加连接*/
    pthread_mutex_lock(&conn_lock);

    printf("recv_fileinfo(): Lock conn_lock, enter gconn[]\n");
    ///拿到维护的全局可用连接
    while(m_global_con[freeid].used ){
        ++freeid;
        freeid = freeid%CONN_MAX;
    }

    ///初始化一个连接 后续请求通过这个连接练习服务端文件信息和接收到的文件信息
    bzero(&m_global_con[freeid].file_name, FILENAME_MAXLEN);
    m_global_con[freeid].sockfd = sockfd;
    strcpy(m_global_con[freeid].file_name, finfo.file_name);
    m_global_con[freeid].file_size = finfo.file_size;
    m_global_con[freeid].chunk_num = finfo.chunk_num;
    m_global_con[freeid].chunk_size = finfo.chunk_size;
    m_global_con[freeid].mbegin = map;
    m_global_con[freeid].recv_count = 0;
    m_global_con[freeid].used = 1;

    pthread_mutex_unlock(&conn_lock);

    printf("recv_fileinfo(): Unock conn_lock, exit gconn[]\n");

    /*向client发送分配的freeid（gconn[]数组下标），作为确认，每个分块都将携带id*/
    ///确认为0：todo提供更多的交互
    char freeid_buf[INT_SIZE]={0};
    memcpy(freeid_buf, &freeid, INT_SIZE);
    ///回复的内容：指明分配的id
    send(sockfd, freeid_buf, INT_SIZE, 0);
    printf("freeid = %d\n", *(int *)freeid_buf);
    return;
}


//*
/*接收文件块*/
///是在接收文件信息之后 也就是创建了该文件以及创建了共享内存 之后操作创建的文件以及共享内存
void LockFreeServer::recv_filedata(int sockfd)
{
   // set_fd_noblock(sockfd);
    /*读取分块头部信息*/
    int recv_size=0;
    char head_buf[200] = {0};
    char *p=head_buf;
    while(1){
        if( recv(sockfd, p, 1, 0) == 1 ){
            ++recv_size;
            if(recv_size == head_len)
                break;
            ++p;
        }
    }

    struct filehead file_head;
    memcpy(&file_head, head_buf, head_len);
    int recv_id = file_head.which_con;

     /*计算本块在map中起始地址fp*/ 
    ///逻辑：根据id拿到全局接收的连接 通过该连接进行收发信息
    int recv_offset = file_head.file_offset;
    char *fp = m_global_con[recv_id].mbegin+recv_offset;

    printf("------- blockhead -------\n");
    printf("filename = %s\nThe filedata id = %d\noffset=%d\nchunk_size = %d\nstart addr= %p\n", file_head.file_name, file_head.which_con, file_head.file_offset, file_head.chunk_size, fp);
    printf("-------------------------\n");

    /*接受数据，往map内存写*/
    int remain_size = file_head.chunk_size;     //数据块中待接收数据大小
    int size = 0;                   //一次recv接受数据大小
    while(remain_size > 0){
         if((size = recv(sockfd, fp, RECVBUF_SIZE, 0)) >0){
                fp+=size;
                remain_size-=size;
//                printf("recv size = %d      ",size);
//                printf("remain size = %d\n",remain_size);
         }
    }

    printf("----------------- Recv a fileblock ----------------- \n");

    /*增加recv_count，判断是否是最后一个分块，如果是最后一个分块，同步map与文件，释放gconn*/
    pthread_mutex_lock(&conn_lock);
    m_global_con[recv_id].recv_count++;
    if(m_global_con[recv_id].recv_count == m_global_con[recv_id].chunk_num){
	    ///同步map和文件?
        munmap((void *)m_global_con[recv_id].mbegin, m_global_con[recv_id].file_size);

        printf("-----------------  Recv a File ----------------- \n ");

        int fd = m_global_con[recv_id].sockfd;
        close(fd);
	    bzero(&m_global_con[recv_id], conn_len);
    }
    pthread_mutex_unlock(&conn_lock);

    close(sockfd);
    return;
}

/*初始化Server，监听Client*/
int LockFreeServer::Server_init(int port)
{
    int listen_fd;
    struct sockaddr_in server_addr;
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0))== -1)
    {
        fprintf(stderr, "Creating server socket failed.");
        exit(-1);
    }
    set_fd_noblock(listen_fd);
    socklen_t sockaddr_len=sizeof (sockaddr);
    bzero(&server_addr, sockaddr_len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    const int on=1;
    setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    if(bind(listen_fd, (struct sockaddr *) &server_addr, sockaddr_len) == -1)
    {
        fprintf(stderr, "Server bind failed.");
        exit(-1);
    }

    if(listen(listen_fd, LISTEN_QUEUE_LEN) == -1)
    {
        fprintf(stderr, "Server listen failed.");
        exit(-1);
    }
    return listen_fd;
}

void LockFreeServer::set_fd_noblock(int fd)
{
    int flag=fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag | O_NONBLOCK);
	return;
}

void LockFreeServer::worker(int confd) {
    //接受到客户端请求解除阻塞
    char type_buf[INT_SIZE] = {0};
    char *p=type_buf;
    int recv_size=0;
    while(1){
        if( recv(confd, p, 1, 0) == 1 ){
            ++recv_size;
            if(recv_size == INT_SIZE)
                break;
            ++p;
        }
    }

    int type=*((int*)type_buf);///转为int类型
    switch (type){
        /*接收文件信息*/
        ///0接收文件信息   255接受文件块
        case 0:
            printf("## worker ##\nCase %d: the work is recv file-info\n", type);
            ///调用会回调 也就是传入参数中绑定的回调
            recv_fileinfo(confd);
            break;
            /*接收文件块*/
        case 255:
            printf("## worker ##\nCase %d: the work is recv file-data\n", type);
            recv_filedata(confd);
            break;
        default:
            printf("unknown type!");
    }
}

