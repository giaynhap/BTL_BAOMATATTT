#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define BUFFERSIZE 512
#define PROTOPORT 5193
#pragma comment(lib,"ws2_32.lib")
size_t count_time;

void clear_winsock() {
    #if defined WIN32
        WSACleanup();
    #endif
}

int act_connect(char host[],unsigned int port) {
    #if defined WIN32
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
        if (iResult != 0) {
            printf("error at WSASturtup\n");
            return -1;
        }
    #endif

 
	int socketfd;
	socketfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketfd < 0) {
		printf("socket creation failed.\n");
		closesocket(socketfd);
		clear_winsock();
		return -1;
	}
 
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr(host);  
	sad.sin_port = htons(port);  
 
	if (connect(socketfd, (struct sockaddr *) &sad, sizeof(sad)) < 0) {
		printf("Failed to connect.\n");
		closesocket(socketfd);
		clear_winsock();
		return -1;
	}
    return socketfd;
}

int act_wait_rev_buff(int socketfd,size_t max_size, int (onrev)(int ,void*,size_t )){

    void * buf =malloc(max_size);
    size_t bytes;
	int err = 1;
    while (1) {

        if ((bytes= recv(socketfd, buf,max_size, 0)) <= 0) {
			printf("recv() failed or connection closed prematurely");
			closesocket(socketfd);
			clear_winsock();
			return 0;
		}
		err = onrev(socketfd,buf,bytes);
        if (err != 0){
            break;
        }
    }
    free(buf);
    return err;
}

int act_send_buff(int socketfd,void *buff,size_t len){
    if (send(socketfd, buff, len, 0) != len) {
		printf("send() sent a different number of bytes than expected");
		closesocket(socketfd);
		clear_winsock();
		return 0;
	}
    return 1;
}
int act_close(int socketfd){
    closesocket(socketfd);
	clear_winsock();
}


 
void ex_file_name( char path[],char *name )
{
    if( path == NULL )
        return NULL;
	int len = strlen(path);
	int i = 0;
    for( i = len-1;i>0 ; i-- )
    {
        if( path[i] == '/' || path[i] == '\\' ){
         
			break;
		}
    }
	memset(name,0,64);
	memcpy(name,path+i+1,64);
 
}

void smart_content_size(size_t size){
	size_t i = 1;
	while(i < size || i<16){

		i = i<<1;

	}
	return i;
}

clock_t begin;
void reset_time(){
  /*  struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	count_time = ts.tv_nsec;
	*/
	begin = clock();
}

size_t delta_time(){
/*	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
    size_t current =ts.tv_nsec;
    return current - count_time;
	*/
	double time_spent =  0.0;
	clock_t end = clock();
	return end - begin;
}
