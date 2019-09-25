

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sha1.h"
#include "aes.h"
#include "act_socket.h"
#define HEAD_SIZE 8
size_t file_size = 0;
size_t read_size = 0;
size_t packet_size = 0;
size_t conent_size = 0;
size_t file_read_size = 0;

size_t total_decrypt_time = 0;
int packet_count = 0;

struct AES_ctx ctx;

char cache_buff[4096];
FILE *file ;

// khởi tạo đọc giải mã 
void init_decrypt(size_t f_size){
    file_size = f_size;
    packet_size = 0;
    file_read_size = 0;
    packet_count = 0;
    total_decrypt_time = 0;
   
}

// hàm giải mã  và ghi luôn tới file
void decrypt_block(void *buff,size_t size,size_t content_size){
    reset_time();
    AES_CBC_decrypt_buffer(&ctx,buff,size);
    total_decrypt_time += delta_time();
}
// gọi kết thúc giải mã
void decrypt_end(){
    printf("Fix file size ....\n");
    fseek(file, 0L, SEEK_SET);
    ftruncate(fileno(file),file_size);
    fclose(file);
   
}

// hàm kiểm tra, tách các gói tin giải mã
int check_packet(){
    
    // kiểm tra lấy thông tin header gói tin
    if (packet_size == 0 && read_size >= HEAD_SIZE){
        // kích thước gói mã hóa
        packet_size = *((size_t *)cache_buff);
        conent_size = packet_size;
        packet_count++;
    }

    if (read_size > HEAD_SIZE && read_size - HEAD_SIZE >= packet_size ){
        
        // cắt lấy phần gói tin theo kích thước
        void *packet = malloc(packet_size);
        memcpy(packet,cache_buff+HEAD_SIZE,packet_size);
        read_size -= (HEAD_SIZE + packet_size);
        memcpy(cache_buff,cache_buff+HEAD_SIZE+packet_size,read_size);
         
       
        // gọi giả mã

        decrypt_block (packet,packet_size,conent_size);
        fwrite(packet,1,conent_size,file);

        file_read_size+=packet_size;
        packet_size = 0;

        free(packet);
        return check_packet();
    }
    // kết thúc nếu đã xong
    if (file_read_size>=file_size){
        return 1;
    }
    return 0;
    
}

// hàm nhận dữ liệu từ socket
int recv_packet(int conn,void *buff,int size){
    memcpy(cache_buff+read_size,buff,size);
    read_size+=size;

    return check_packet();
}
// hàm chạy giải mã lưu file
int decrypt_file(int conn,struct  file_packet_header * header, char key[]){
   
   // tạo password
   char hash_password[21];
   generate_password(key,hash_password);
  
    printf("Password ");
    print_password_hash(hash_password);
    printf("\n");

    AES_init_ctx_iv(&ctx, hash_password, header->iv);

    // tạo vị trí lưu file ./rev/tên file
   char save_file[512] ;
   sprintf(save_file,"./rev/%s",header->name);
   file = fopen(save_file,"wb");

   //nhận dữ liệu và giải mã
   init_decrypt(header->content_size);
   act_wait_rev_buff(conn,1024,recv_packet);
   decrypt_end();

    printf("Done\n\n================\n");
    printf("So luong packet: %d packet\n",packet_count);
    printf("Tong thoi gian giai ma: %ld ticks\n",total_decrypt_time);
    printf("Trung binh thoi gian gia ma cua 1 packet: %ld ticks\n",total_decrypt_time/packet_count);

}


