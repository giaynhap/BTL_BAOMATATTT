
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "sha1.h"
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


void ex_file_name( char path[],char *name )
{
    if( path == NULL )
        return ;
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

size_t smart_content_size(size_t size){
	size_t i = 1;
	while(i < size || i<16){

		i = i<<1;

	}
	return i;
}

clock_t begin;
void reset_time(){
	begin = clock();
}

size_t delta_time(){
	clock_t end = clock();
	return end - begin;
}
