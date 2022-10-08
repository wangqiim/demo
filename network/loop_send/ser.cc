#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <atomic>
#include <mutex>

// 创建5G大小的文件用于mmap
// $ dd if=/dev/urandom of=random.img count=1024 bs=5M

const uint64_t MMAP_SIZE = 8 * 2e8;
char *mmap_ptr = nullptr;

void create_mmap_file() {
  int fd = open("random.img", O_RDWR);
  mmap_ptr = (char *)mmap(0, MMAP_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
  if (mmap_ptr == nullptr) {
    perror("mmap fail");
  }
  memset(mmap_ptr, 0xFF, MMAP_SIZE);
  std::cout << "mmap success" << std::endl;
}

int main(){
    create_mmap_file();
    //创建套接字
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    //进入监听状态，等待用户发起请求
    listen(serv_sock, 20);
    //接收客户端请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    
    //向客户端发送数据
    uint64_t writen_len = 0;
    while (writen_len < MMAP_SIZE) {
        ssize_t send_bytes = send(clnt_sock, mmap_ptr + writen_len, MMAP_SIZE - writen_len, 0);
        std::cout << "send_bytes: " << send_bytes << std::endl;
        if (send_bytes <= 0) {
            std::cout << "send fail, errno = " << errno << std::endl;
        }
        std::cout << "writen_len: " << writen_len << std::endl;
        writen_len += send_bytes;
    }

    //关闭套接字
    close(clnt_sock);
    close(serv_sock);
    return 0;
}
