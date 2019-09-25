
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "act_socket.h"

// cấu hình server truyền dẫn
#define HOST "124.158.6.221"
#define PORT 8834

int send_file(int conn,char key[],char path[]);
int decrypt_file(int conn,struct  file_packet_header * header, char key[]);

struct REQ rq ; 

// tạo các option cho app
struct option longopts[] =
    {
    { "send",             required_argument, NULL, 's'},
    { "rev",              no_argument, NULL, 'r'},
    { "key",              required_argument, NULL, 'k'},
    { "help",               no_argument,             NULL, 'h'},
    { 0 }
    };

struct REQ{
    char path[512];
    char key[64];
    char type;
};

// in hướng dẫn sử dụng
void usage(){

printf("BTL_ATBMTT_CLIENT Syntax:\n \
	client [Types] [Path] -k [Key] \n\
Types: Client mode \n\
	-r : receiver\n\
	-s : sender\n\
Path: File/folder send/recv path\n\
Key: encrypt/decrypt password\n\
\n\
Example:\n\
    client -s ./abc.txt -k [Key] \n\
    client -r ./ -k [Key] \n\
\n");

exit(0);
}

// chế độ giử file
void sender(){

   int sfd = act_connect(HOST,PORT);
   if ( send_encrypt(sfd,rq.key,rq.path)<0){
       printf("send_file error* ");
   }
   printf ("\n1 giay = %ld ticks\n",CLOCKS_PER_SEC);
}

// hàm nhận thông tin kết nối và xử lý
int handling_handshake(int sock,void *buff,size_t size){
  struct  file_packet_header * header = (struct file_packet_header *)buff;
  printf("Rev file: %s \n",header->name);
  printf("\tFile size: %ld bytes\n",header->content_size);
  
  //trả thông tin handshake
  char  response[32] = "giaynhap";
  act_send_buff(sock,response,sizeof(response));
  
  if (decrypt_file(sock,header,rq.key)<0){
      printf("decrypt_file error* ");
  }
  printf ("\n1 giay = %ld ticks\n",CLOCKS_PER_SEC);
  return 1;

}
// chế độ nhận file
void receiver( ){
    int sfd = act_connect(HOST,PORT);
    act_wait_rev_buff(sfd,1024,handling_handshake);
}


int main(int argc,char * argv[])
{

    // phần option 
    rq.type = -1;
    while (1)
    {
        int opt;

        opt = getopt_long (argc, argv, "r:s:k:h", longopts, 0);

        if (opt == EOF)
            break;
        
        switch (opt)
        {
        case 0:
            break;
    
        case 'k':
            sscanf(optarg,"%s",rq.key);
            
            break;
        case 'r':
            rq.type = 0;
            break;
        case 's':
            rq.type = 1;
            sscanf(optarg,"%s",rq.path);
            
        break;
        case 'h':
            usage ();
            break;
        default:
            usage ();
            break;
        }
    }
    if (rq.type == 1){
        sender();
    }else  if (rq.type == 0){
        receiver();
    }else{
        usage ();
    }

    return 0;
}   