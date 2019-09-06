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

//初始化，返回服务器描述字
int startup(const char* _ip,int _port){ 
    int sock=socket(AF_INET,SOCK_STREAM,0); //socket():初始化
    if(sock<0){                             //成功后返回服务器描述字，一直存在
        perror("socket");
        exit(2);
    }
    printf("fd:%d\n",sock);
    struct sockaddr_in local;               //用sockaddr_in存储local(Server) addr和port信息
    local.sin_family=AF_INET;               //协议域
    local.sin_port=htons(_port);            //local port
    local.sin_addr.s_addr=inet_addr(_ip);   //local ip
    if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0){    //bind():绑定local(Server) port和ip到sock
        perror("bind");
        exit(3);
    }
    if(listen(sock,10)<0){  //listen():接受一个网络请求，最多10个client等待
        perror("listen");
        exit(4);
    }
    return sock;
}
  
int main(int argc,char *argv[]){    //用sockaddr_in存储Client addr和port信息
    if(argc!=3){    //输入参数不合法
        usage(argv[0]);
        return 1;
    }
    int listen_sock=startup(argv[1],atoi(argv[2])); //初始化，生成服务器描述字
    while(1){
        struct sockaddr_in client;
        socklen_t len=sizeof(client);
        int new_sock=accept(listen_sock,(struct sockaddr*)&client,&len);    //接受一个Client的网络请求，成功后返回已连接的Scoket描述字
        if(new_sock<0){
            perror("accept");
            continue;
        }

        printf("get a new client:[%s:%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));

        char buf[1024];
        while(1){
            ssize_t s=read(new_sock,buf,sizeof(buf)-1); //从Client读取数据
            printf("Get_Size =  %d\n",s);
            if(s>0){    //读取成功
                buf[s]=0;
                printf("client# %s\n",buf);
                write(new_sock,buf,strlen(buf));
            }
            else if(s==0){  //Client离线
                printf("client quit!\n");
                break;
            }
            else{   //读取失败
                perror("read");
                break;
            }
        }
        close(new_sock);    //读取失败就退出
    }
}