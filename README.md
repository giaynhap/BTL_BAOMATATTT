# Bài tập lớn ATBMTT
Mã hóa file tại pc1 sử dụng socket gửi qua pc2 giải mã.
## Usage
### Build SERVER_SIDE (make)
1. cd SERVER_SIDE 
2. make 
3. Đưa file server lên 1 máy làm máy chủ
4. ./server [port] (ex ./server 50)
## Build CLIENT_SIDE  (make)
1. Cấu hình lại địa chỉ server và port trong file main.c ở CLIENT_SIDE
1. cd CLIENT_SIDE
2. make
3. Copy file client tới máy pc1 và pc2;
## Chạy demo
Pc1 là máy gửi file;
Pc2 là máy nhận file;

tại Pc2 gõ:
```
mkdir rev
./client -r ./ -k "password"
```
tại Pc1 gõ:
```
./client -s ./filegui -k "password"
```
xem kết quả hiện trên terminal.
### CLIENT_SIDE folder
```
file encrypt.c chứa phần đọc và mã hóa file
file decrypt.c chứa phần ghi và giải mã file
file aes.c chứa thuật toán AES (git tiny-aes)
file sha1.c chứa thuật toán mã băm sha1 (tạo password)
file client.c chứa phần xử lý gửi nhận dữ liệu
```
### SERVER_SIDE folder.
```
file main.s server chuyển tiếp dữ liệu 2 máy client
```