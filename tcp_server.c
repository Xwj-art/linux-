#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc,char *argv[]){
    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;//地址族结构体
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    char message[]="Hello World!";

    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }

    serv_sock=socket(PF_INET,SOCK_STREAM,0);//调用socket函数创建套接字
    //ipv4互联网协议族，面向连接传输，tcp协议
    if(serv_sock == -1){
        error_handling("socket() error");
    }

    memset(&serv_addr,0,sizeof(serv_addr));
    //地址族结构体中的地址族种类，为了与sockaddr一致
    serv_addr.sin_family=AF_INET;
    //htons中h是host，n是network，分别代表主机字节序和网络字节序
    //s指的是short，剩下的是to，意思就是将short类型数据由主机字节序转化为网络字节序
    //地址族结构体中的ip号，l代表4字节，通常用于ip地址转换
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    //INADDR_ANY可获得运行服务器端的计算机ip地址，不必亲自输入
    //地址族结构体中的端口号，s代表2字节，通常用于端口号转换
    serv_addr.sin_port=htons(atoi(argv[1]));
    //均以网络字节序存储

    //结构体成员中的sin_zero无特殊含义，只是为了让sockaddr_in
    //的大小与sockaddr大小一样，且必须填充0

    //调用bind分配ip地址和端口号
    //sockaddr有两个成员，一个是sa_family
    //另一个就是存储ip和端口号用的数组
    //将sockaddr_in转换为sockaddr
    if(bind(serv_sock,(struct sockaddr*) &serv_addr,sizeof(serv_addr))==-1){
        error_handling("bind() error");
    }

    //将套接字转换为可接受状态
    if(listen(serv_sock,5)==-1){
        //5为接受连接的最大个数，储存在连接请求等待队列中
        error_handling("listen() error");
    }

    clnt_addr_size=sizeof(clnt_addr);
    //调用accept受理连接请求，如果在没有连接请求的情况下调用该函数
    //将不会返回，直到有连接请求为止
    clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
    //将客户端地址信息填充
    if(clnt_sock==-1){
        error_handling("accept() error");
    }

    //传输数据，若程序经过了accept则代表有连接请求
    //传输的数据在被接收会自动转化，不需要程序员手动转换
    write(clnt_sock,message,sizeof(message));
    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void error_handling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}
