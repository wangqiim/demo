#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cassert>

// ref: https://blog.crazyark.me/p/epoll/
// Linux Programmer's Manual

#define MAX_EVENTS 10
#define MAX_LISTEN_SOCKETS 20
#define READ_BUF_SIZE 4096
struct epoll_event ev, events[MAX_EVENTS];
int listen_sock, conn_sock, nfds, epollfd;

void do_use_fd(int fd) {
    // echo
    for (;;) {
        char buf[READ_BUF_SIZE];
        auto read_len = read(fd, buf, READ_BUF_SIZE);
        if (read_len > 0) {
            // todo(wq): assumen not need to re write
            // std::cout << "read_len = " << read_len << std::endl;
            int write_len = write(fd, buf, read_len);
            assert(write_len = read_len);
            continue;
        } else if (read_len < 0) {
            // todo(wq): remove fd from epoll
            return;
        } else {
            if (errno == EAGAIN) {
                return;
            } else {
                // todo(wq): remove fd from epoll
                std::cout << "[do_use_fd] errno = " << errno << std::endl;
            }
        }
    }
}

void setnonblocking(int sock) {
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0) {
        perror("fcntl(sock, GETFL)");
        exit(EXIT_FAILURE);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0) {
        perror("fcntl(sock, SETFL, opts)");
        exit(EXIT_FAILURE);
    }  
}

void ready() {
    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(listen_sock >= 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; // ipv4
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // ip
    serv_addr.sin_port = htons(1234); // port
    int ret = bind(listen_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    assert(ret == 0);

    ret = listen(listen_sock, MAX_LISTEN_SOCKETS);
    assert(ret == 0);

    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

}

void loop() {
    std::cout << "start loop..." << std::endl;
    for (;;) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listen_sock) {
                struct sockaddr_in clnt_addr;
                socklen_t addrlen = sizeof(clnt_addr);
                conn_sock = accept(listen_sock, (struct sockaddr *) &clnt_addr, &addrlen);
                if (conn_sock == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                std::cout << "create connect to " << clnt_addr.sin_addr.s_addr << ": " << clnt_addr.sin_port << std::endl;
                setnonblocking(conn_sock);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            } else {
                do_use_fd(events[n].data.fd);
            }
        }
    }
}

int main() {
    ready();
    loop();
}