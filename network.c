#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>

//define listen socket(receive msg) and connect socket(send msg)
int listen_fd , conn_fd;
char recvbuf[buffer_size];
char sendbuf[buffer_size];
// define sockaddress 
struct sockaddr_in server_sockaddr, client_addr;
//define socket server address when it represent client
struct sockaddr_in remote_server_sockaddr;
void init_listen(){
	if((listen_fd = socket(AF_INET , SOCK_STREAM , 0)) == -1){
		perror("socket error.\n");
		exit(1);
	}
	server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(recv_port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listen_fd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1){
        perror("bind error.\n");
        exit(1);
    }
    if(listen(listen_fd,connect_pool) == -1){
        perror("listen error.\n");
        exit(1);
    }
    socklen_t length = sizeof(client_addr);
	int conn = accept(listen_fd, (struct sockaddr*)&client_addr, &length);
	if(conn<0){
        perror("connect error.\n");
        exit(1);
    }
    while(1){
    	recv(conn, recvbuf, sizeof(recvbuf),0);
    	if(strcmp(recvbuf,"exit\n")==0)
            break;
        fputs(recvbuf, stdout);
        memset(recvbuf,0,sizeof(recvbuf));
    }
}
void init_conn(){
	if((conn_fd = socket(AF_INET , SOCK_STREAM , 0)) == -1){
		perror("socket error.\n");
		exit(1);
	}
	remote_server_sockaddr.sin_family = AF_INET;
    remote_server_sockaddr.sin_port = htons(send_port); 
    remote_server_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    if (connect(conn_fd, (struct sockaddr *)&remote_server_sockaddr, sizeof(remote_server_sockaddr)) < 0)
    {
        perror("connect");
        exit(1);
    } 
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        send(sock_cli, sendbuf, strlen(sendbuf),0); ///发送
        if(strcmp(sendbuf,"exit\n")==0)
            break;
        memset(sendbuf, 0, sizeof(sendbuf));
    }
}
bool sendmsg(){
	return true;
}
bool 