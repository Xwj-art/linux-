#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 30
void error_handling(char* message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

int main(int argc,char* argv[]){
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_adr,clnt_adr;
    struct timeval timeout;
    fd_set reads,cpy_reads;

    socklen_t adr_sz;
    int fd_max,str_len,fd_num;
    char buf[BUF_SIZE];
    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }

    serv_sock=socket(PF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))){
        error_handling("bind() error");
    }
    if(listen(serv_sock,5)){
        error_handling("listen() error");
    }

    FD_ZERO(&reads);
    FD_SET(serv_sock,&reads);
    fd_max=serv_sock;

    while(1){
        cpy_reads=reads;//cpy_reads用来存放reads里面的信息
        timeout.tv_sec=5;
        timeout.tv_usec=5000;
        if((fd_num=select(fd_max+1,&cpy_reads,0,0,&timeout))==-1){
            break;
        }
        if(!fd_num)  continue;

        for(int i=0;i<fd_max+1;i++){
            if(FD_ISSET(i,&cpy_reads)){
                if(i==serv_sock){//服务器端的读，是客户端使用connect读时读到的数据
                    adr_sz=sizeof(clnt_adr);
                    clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&adr_sz);
                    FD_SET(clnt_sock,&reads);//将与客户端通信的套接字设置监控
                    if(fd_max<clnt_sock)    fd_max=clnt_sock;
                    printf("connected client: %d\n",clnt_sock);
                } else {
                    str_len=read(i,buf,BUF_SIZE);//与客户端交互的套接字的读是从客户端那里读到数据
                    if(!str_len){
                        FD_CLR(i,&reads);
                        close(i);
                        printf("closed client: %d \n",i);
                    } else {
                        write(i,buf,str_len);
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}
