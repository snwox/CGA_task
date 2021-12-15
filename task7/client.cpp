#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <pthread.h>

#define MAX_SIZE 512

int socketfd;
char buf[MAX_SIZE];
char recv_buf[MAX_SIZE];
struct sockaddr_in client;
pthread_t thread;

void init_thread(void);
void *thread_routine(void *arg);
void handler(int sig);
int init(char *ip,int port);
int main(int argc,char *argv[]){
	if(argc<3){
		printf("usage : ./client <ip addr> <port>");
		exit(0);
	}
	if(!init(argv[1],atoi(argv[2]))){
		init_thread();
		puts("[+] Connect success");
		puts("[+] CTRL+C to exit");
		while(1){
			memset(buf,0,sizeof(buf));
			scanf("%512s",buf);
			write(socketfd,buf,sizeof(buf));
		}
	}else{
		puts("[-] Connect failed..");
	}
}
int init(char *ip,int port){
	signal(SIGINT,handler);
	socketfd=socket(AF_INET,SOCK_STREAM,0);
	client.sin_family=AF_INET;
	client.sin_addr.s_addr=inet_addr(ip);
	client.sin_port=htons(port);
	if(!connect(socketfd,(struct sockaddr*)&client,sizeof(client))){
		return 0;
	}else{
		return 1;
	}
}
void init_thread(void){
	pthread_create(&thread,NULL,thread_routine,NULL);
}
void *thread_routine(void *arg){
	while(1){
		memset(recv_buf,0,sizeof(recv_buf));
		if(!read(socketfd,recv_buf,sizeof(recv_buf))){
			break;
		}
		printf("\r[+] %s\n",recv_buf);
	}
}
void handler(int sig){
	pthread_detach(thread);
	printf("exit\n");
	close(socketfd);
	exit(0);
}