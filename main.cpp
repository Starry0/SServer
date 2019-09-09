#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "threadpool.h"
#include "http_conn.h"
#include "locker.h"
#include "util.h"
#include "epoll.h"
#include "log.h"

#define MAX_FD 65536
#define MAX_EVENT_NUMBER 10000

int main(int argc, char* argv[]) {
    config_t config;
    const char* config_path = "WebConfig.conf";
    if(read_config(&config, config_path) == CONFIG_ERROR) {
        fprintf(stderr, "read config error!!!");
        return -1;
    }

    /* 忽略SIGPIPE信号*/
    addsig(SIGPIPE, SIG_IGN);

    /* 创建线程池*/
    threadpool<http_conn>* pool = NULL;
    try {
        pool = new threadpool<http_conn>(config.thread_number);
    } catch (...) {
        return 1;
    }

    /* 预先为每个可能的客户连接分配一个http_conn对象*/
    http_conn* users = new http_conn[MAX_FD];
    assert(users);
    int user_count = 0;

    int listenfd = socket_bind_listen(&config);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = createfd(5);
    addfd(epollfd, listenfd, false);
    http_conn::m_epollfd = epollfd;
    int num = 0;
    while (true) {
        int number = waitfd(epollfd, events, MAXLISTEN, -1);
//
//        num += number;
//        printf("%d\n",num);
        handle_event(epollfd, listenfd, pool, users, events, number, -1);
    }
    close(epollfd);
    close(listenfd);
    delete[] users;
    delete pool;
    return 0;
}