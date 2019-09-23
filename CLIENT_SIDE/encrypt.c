

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sha1.h"
#include "aes.h"
#include "act_socket.h"
struct AES_ctx ctx;
#define ENCRYPT_TEMP_FILE "./temp.encrypt.alt"

// hàm tạo password 
void generate_password(char pass[],char *out){
    SHA1( out, pass, strlen(pass) );
}

void print_password_hash(char *hash){
    int offset;
    char hexresult[41];
    for( offset = 0; offset < 20; offset++) {
    sprintf( ( hexresult + (2*offset)), "%02x", hash[offset]&0xff);
  }
  printf("%s",hexresult);
}

// hàm tạo initialization vector cho kiểu AES CBC
void generate_initialization_vector(char * iv){
    int i = 0;
    for (i = 0; i < 16;i++ ){
        iv[i] = rand() & 1;
    }
}

// hàm mã hóa
void encrtpy_block(void *buf,size_t size, void *key){

    AES_CBC_encrypt_buffer(&ctx, buf,size);
}

// hàm tạo thông tin gửi file
void make_package_header(size_t file_length,char name[] ,struct file_packet_header * header){

    memcpy( header->name,name,64);
    generate_initialization_vector(header->iv);
    header->content_size = file_length;
}

// hàm được gọi khi có phản hồi gói kết nối của máy 2
int handshake_handle(int conn,void* buff,size_t  size){

    return 1;
}

int encrypt_file(FILE * fp,struct file_packet_header * header,char key[]){

    char buffer[2048];
    // tạo password
    char hash_password[21];
    generate_password(key,hash_password);

    printf("Password ");
    print_password_hash(hash_password);
    printf("\n");

    size_t total_encrypt_time=0;
 
    int packet_count =0;
    // đọc và mã hóa
    AES_init_ctx_iv(&ctx, hash_password,header->iv);
    FILE *fw = fopen(ENCRYPT_TEMP_FILE,"wb");
    if (fw < 0){
        return -1;
    }
    reset_time();
    while (!feof(fp))  
    { 
        memset(buffer,0,sizeof(buffer));
        // đoc file
        size_t bytes = fread(buffer, 1, sizeof(buffer), fp); 
        size_t encrypt_size = sizeof(buffer);
        if (bytes < encrypt_size)
        {
            encrypt_size = smart_content_size(bytes);
        }
        encrtpy_block(buffer,encrypt_size,hash_password);
        fwrite(buffer,1,encrypt_size,fw);
    } 
    fclose(fw);
    total_encrypt_time = delta_time();
    printf("Done\n\n================\n");
    printf("Tong thoi gian ma hoa: %ld ns\n",total_encrypt_time);
}

int send_encrypt(int conn,char key[],char path[]){
    
    printf("Initializing\n");  
    size_t total_encrypt_time=0;
    size_t total_send_time = 0;
    FILE *fp;
    fp = fopen(path,"rb");
    if (fp < 0 ){
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    // lấy kích thước file
    size_t sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char file_name[64] ;
  
    // lấy tên file
    ex_file_name(path,file_name);
    struct file_packet_header phandshake ;

    // tạo kết nối gửi thông tin file cho máy 2
    make_package_header(sz,file_name,&phandshake);
    phandshake.type = 1;
    act_send_buff(conn,&phandshake,sizeof(struct file_packet_header));
    if (act_wait_rev_buff(conn,1024, handshake_handle) != 1){
        return -2;
    }
    if (encrypt_file(fp,&phandshake,key) < 0 ){
        fclose(fp);
        return -1;
    }

    fclose(fp);

    char buffer[1024];


    fp = fopen(ENCRYPT_TEMP_FILE,"rb");
    if (fp < 0 ){
        return -1;
    }
    while (!feof(fp))  
    { 
        // đoc file
        size_t bytes = fread(buffer, 1, sizeof(buffer), fp); 
        act_send_buff(conn,&(bytes),8);
        act_send_buff(conn,&(bytes),8);
        act_send_buff(conn,buffer,bytes);   
    } 

    fclose(fp);
}
// hàm đọc và gửi file
int send_file(int conn,char key[],char path[]){

    printf("Initializing\n");
    // mở file dọc binary
    FILE *fp;
    fp = fopen(path,"rb");
    if (fp < 0 ){
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    // lấy kích thước file
    size_t sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char file_name[64] ;
  
    // lấy tên file
    ex_file_name(path,file_name);
    struct file_packet_header phandshake ;

    // tạo kết nối gửi thông tin file cho máy 2
    make_package_header(sz,file_name,&phandshake);
    phandshake.type = 0; 
    act_send_buff(conn,&phandshake,sizeof(struct file_packet_header));
    if (act_wait_rev_buff(conn,1024, handshake_handle) != 1){
        return -2;
    }

    char buffer[1024];

    // tạo password
    char hash_password[21];
    generate_password(key,hash_password);

    printf("password ");
    print_password_hash(hash_password);
    printf("\n");

    size_t total_encrypt_time=0;
    size_t total_send_time = 0;
    int packet_count =0;
    // đọc và mã hóa
    AES_init_ctx_iv(&ctx, hash_password,phandshake.iv);
    
    while (!feof(fp))  
    { 
        memset(buffer,0,sizeof(buffer));
        // đoc file
        size_t bytes = fread(buffer, 1, sizeof(buffer), fp); 
        size_t encrypt_size = sizeof(buffer);
        if (bytes < encrypt_size)
        {
            encrypt_size = smart_content_size(bytes);
        }
       
        reset_time();
        encrtpy_block(buffer,encrypt_size,hash_password);
        total_encrypt_time+= delta_time();
        packet_count++;

        // gửi dữ liệu mã hóa
        // dữ liệu gử i đi có cấu trúc: [header][content]
        // header bao gồm: [kích thước phần mã hóa][kích thước thật của dữ liêu]
        // content là dữ liệu đã mã hóa
        reset_time();
        act_send_buff(conn,&(encrypt_size),8);
        act_send_buff(conn,&(bytes),8);
        act_send_buff(conn,buffer,encrypt_size);
        total_send_time+= delta_time();
    } 

    fclose(fp);
    printf("Done\n\n================\n");
    printf("File size %ld bytes\n",sz);
    printf("Num of packet: %d packet\n",packet_count);
    printf("Total encrypt time: %ld ns\n",total_encrypt_time);
    printf("Average encrypt time: %ld ns\n",total_encrypt_time/packet_count);
    printf("Total transfer time: %ld ns\n",total_send_time);
    printf("Average transfer time: %ld ns\n",total_send_time/packet_count);
    
}


