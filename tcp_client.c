#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc,char *argv[]){
    int sock;
    struct sockaddr_in serv_addr;//表示地址族协议的结构体
    char message[30];
    int str_len;
    int idx=0,read_len=0;

    if(argc!=3){
        printf("Usage:%s <IP> <port>\n",argv[0]);
        exit(1);
    }

    sock=socket(PF_INET,SOCK_STREAM,0);//创建套接字,tcp协议
    //如果有bind,listen则会成为服务器端的套接字
    //如果有connect则会是客户端套接字
    if(sock==-1){
        error_handling("socket() error");
    }

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    //127.0.0.1代表本机地址
    //inet_addr可以将点分十进制转换为32位整形，且满足网络字节序
    //而且inet_addr还可以检查ip是否有效
    //serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    inet_aton(argv[1],&serv_addr.sin_addr);
    //inet_aton可以直接将转换后的结果填入结构体中
    //9190代表端口号
    serv_addr.sin_port=htons(atoi(argv[2]));
    ///atoi将字符串类型转换为整形
    //htons中h是host，n是network，分别代表主机字节序和网络字节序
    //s指的是short，剩下的是to，意思就是将short类型数据由主机字节序转化为网络字节序

    //向服务器发送请求
    if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
        error_handling("connect() error");
    }

    while(read_len=read(sock,&message[idx++],1)){
        if(read_len==-1){
            error_handling("read() error");
        }
        str_len+=read_len;
    }

    printf("Message from serve : %s \n",message);
    printf("Function read call count: %d\n",str_len);
    close(sock);
    return 0;
}

void error_handling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}
