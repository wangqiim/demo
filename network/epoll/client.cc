#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <errno.h>
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
    int ret = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    assert(ret == 0);
    //读取服务器传回的数据
    const char *buffer = "hello world!";
    ret = write(sock, buffer, strlen(buffer));
    assert(ret == strlen(buffer));
    char read_buf[4096];
    int read_len = read(sock, read_buf, sizeof(read_buf));
    assert(read_len == strlen(buffer));
    std::cout << read_buf << std::endl;
   
    //关闭套接字
    close(sock);

    return 0;
}
