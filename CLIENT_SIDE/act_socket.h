
// các hàm socket
int act_connect(char host[],unsigned int port);
int act_wait_rev_buff(int socketfd,size_t max_size, int (onrev)(int ,void*,size_t ));
int act_send_buff(int socketfd,void *buff,size_t len);
int act_close(int socketfd);

typedef struct  file_packet_header{  
    char name[64];
    char iv[16];
    long content_size;
}  __attribute__((packed,aligned(4))) file_packet_header_t ;

void ex_file_name(char[],char *);