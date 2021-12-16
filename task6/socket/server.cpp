#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define MAX_SIZE 512


int socketfd;
int client_addr;
int clientfd;
int e;
char buf[MAX_SIZE];

struct sockaddr_in server;
struct sockaddr_in client;


socklen_t client_size;
void init(int port);
void handler(int sig);

int main(int argc,char *argv[]){
	if(argc<2){
		printf("usage : ./server <port> [-e]\n");
		exit(0);
	}
	init(atoi(argv[1]));	
	if(argc==3 && !strcmp(argv[2],"-e")){
		e=1;
	}
	puts("[+] Connect success");
	puts("[+] CTRL+C to exit");
	while(1){
		memset(buf,0,sizeof(buf));
		if(!read(clientfd,buf,sizeof(buf))){
			puts("exit");
			break;
		}
		if(e){
			write(clientfd,buf,sizeof(buf));
		}else{
			printf("recv : %s\n",buf);
		}
	}
	close(clientfd);
	close(socketfd);
}
void init(int port){
	signal(SIGINT,handler);
	socketfd=socket(AF_INET,SOCK_STREAM,0);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("0.0.0.0");
	server.sin_port=htons(port);
	
	bind(socketfd,(struct sockaddr*)&server,sizeof(server));
	listen(socketfd,1);
	client_size=sizeof(client_addr);
	clientfd=accept(socketfd,(struct sockaddr*)&client,&client_size);
}
void thread_routine(void *arg){
	
}
void handler(int sig){
	printf("exit\n");
	close(socketfd);
	exit(0);
}

