#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
 
#include<unistd.h>  //C++中所需要的read()、write()

static void usage(const char* proc){
    printf("Usage:%s [local_ip] [local_port]\n",proc);
}

int main(int argc,char* argv[]){
    if(argc!=3){
        usage(argv[0]);
        return 1;
    }

    int sock=socket(AF_INET,SOCK_STREAM,0);
        if(sock<0){
            perror("socket");
            return(2);
    }

    struct sockaddr_in peer;
    peer.sin_family=AF_INET;
    peer.sin_port=htons(atoi(argv[2]));
    peer.sin_addr.s_addr=inet_addr(argv[1]);

    if(connect(sock,(struct sockaddr*)&peer,sizeof(peer))<0){
        perror("connect");
        return 3;
    }
    char buf[1024];
    while(1){
        printf("Please Enter# ");
        fflush(stdout);     //清空输出缓存区
        fflush(stdin);
        ssize_t s = read(0,buf,sizeof(buf)-1);  //读取用户输入
        printf("Read_Size = %d\n",s);
        if(s>0){        
            buf[s-1]=0;
            if(strlen(buf) == 0)
                continue;

            write(sock,buf,strlen(buf));            //向服务器写数据
            
            //读取服务器回调数据并显示
            ssize_t _s=read(sock,buf,sizeof(buf)-1);
            if(_s>0){
                buf[_s]=0;
                printf("server echo# %s\n",buf);

            }
            else if(s==0){  
                printf("server quit!\n");
                break;
            }
        }
    }
    close(sock);
    return 0;
}
