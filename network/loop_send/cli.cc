#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <cassert>

int main(){
    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
   
    //读取服务器传回的数据
    const int buf_size = 4096 * 10;
    char buf[buf_size];

    int has_read_bytes = 0;
    while (true) {
        if (has_read_bytes == buf_size) {
            has_read_bytes = 0;
        }
        ssize_t read_bytes = read(sock, buf + has_read_bytes, buf_size - has_read_bytes);
        if (read_bytes <= 0) {
            std::cout << "read fail, errno = " << errno << std::endl;
            return 0;
        }
        for (ssize_t i = 0; i < read_bytes; i++) {
            assert(*(uint8_t *)(buf + has_read_bytes + i) == 0xFF);
        }
        has_read_bytes += read_bytes;
        std::cout << "has_read_bytes = " << has_read_bytes << std::endl;

    }
    printf("Message form server: %s\n", buf);
   
    //关闭套接字
    close(sock);
    return 0;
}