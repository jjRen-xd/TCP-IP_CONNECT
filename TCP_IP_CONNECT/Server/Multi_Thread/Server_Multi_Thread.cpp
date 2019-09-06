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

int startup(const char* _ip,int _port){
    //初始化
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        perror("socket");
        exit(2);
    }
    printf("fd:%d\n",sock);
    //绑定ip，port
    struct sockaddr_in local;
    local.sin_family=AF_INET;
    local.sin_port=htons(_port);
    local.sin_addr.s_addr=inet_addr(_ip);
    if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0){
        perror("bind");
        exit(3);
    }
    //监听
    if(listen(sock,10)<0){
        perror("listen");
        exit(4);
    }
    return sock;
}

void *request(void *arg){
    int new_sock=(int)arg;
    char buf[1024];
    while(1){
        ssize_t s=read(new_sock,buf,sizeof(buf)-1);
        if(s>0){
            buf[s]=0;
            printf("client# %s\n",buf);
            write(new_sock,buf,strlen(buf));
        }
        else if(s==0){
            printf("client quit!\n");
            break;
        }
        else{
            perror("read");
            break;
        }
    }
    close(new_sock);
    return  (void*)0;
}

int main(int argc,char *argv[]){
    if(argc!=3){
        usage(argv[0]);
        return 1;
    }

    int listen_sock=startup(argv[1],atoi(argv[2]));
    while(1){
        struct sockaddr_in client;
        socklen_t len=sizeof(client);
        int new_sock=accept(listen_sock,(struct sockaddr*)&client,&len);
        if(new_sock<0){
            perror("accept");
            continue;
        }

        printf("get a new client:[%s:%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
        //一旦三次握手成功就开启新线程
        pthread_t id;
        pthread_create(&id,NULL,request,(void*)new_sock);
        pthread_detach(id);

    }
}
/*多线程tcp测试可以通过两个主机连接一个局域网完成。

切换到root用户，关闭自己和对方主机的防火墙—-service iptables stop

检查是否有sshd—-ps aux | grep ssh

使用命令查看IP地址是否在一个局域网内—-ifconfig

与对方主机ping一下看能不能通—-ping 192.xxx.xxx.xxx

将tcp_client执行文件发送到对方主机—-scp tcp_client 192.xxx.xxx.xxx:/home

运行tcp_server，对方主机在/home目录下运行tcp_client 
 */