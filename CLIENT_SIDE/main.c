
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// các hàm socket
int act_connect(char host[],unsigned int port);
int act_wait_rev_buff(int socketfd,size_t max_size, int (onrev)(int ,void*,size_t ));
int act_send_buff(int socketfd,void *buff,size_t len);
int act_close(int socketfd);

int main(int argc,char * argv[])
{
    int sfd = act_connect("0.0.0.0",2402);
    act_send_buff(sfd,"hello",4);
    return 0;
}