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
int test=30;
void thread_routine(void *arg){
	printf("%x",arg);
	printf(",%d",((int*)arg)[0]);
	for(int i=0;i<((int*)arg)[0];i++){
		printf("%d\n",test);
	}
}
int main(){
	int a[10]={10,2,3,4,5};
	pthread_t thread;
	pthread_create(&thread,NULL,thread_routine,(void*)a);
	pthread_join(thread,NULL);
	printf("exit hah\n");
	pthread_detach(thread);
}
