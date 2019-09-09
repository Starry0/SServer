#include "epoll.h"

#define MAX_FD 65536
#define MAX_EVENT_NUMBER 10000


int createfd(int flags) {
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    return epollfd;
}

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd, bool ont_shot) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if(ont_shot) {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

void removefd(int epollfd, int fd) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

void modfd(int epollfd, int fd, int ev) {
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

int waitfd(int epollfd, epoll_event *event, int max_events, int timeout) {
    int count = epoll_wait(epollfd, event, max_events, timeout);
    return count;
}

void handle_event(int epollfd, int listenfd, threadpool<http_conn>* pool, http_conn* users,
        epoll_event *events, int number, int timeout) {
    if((number < 0) && (errno != EINTR)) {
        printf("epoll failure\n");
        exit(1);
    }
    log(LOG_INFO, __FILE__, __LINE__, "number %d",number);
    for(int i = 0; i < number; ++ i) {
        int sockfd = events[i].data.fd;
        log(LOG_INFO, __FILE__, __LINE__, "%d %d",sockfd, events[i].events);
        if(sockfd == listenfd) {
            struct sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlength);
            log(LOG_INFO, __FILE__, __LINE__, "new client %d", connfd);
            if(connfd < 0) {
                printf("errno is: %d\n",errno);
                continue;
            }
            if(http_conn::m_user_count >= MAX_FD) {
                show_error(connfd, "Internal server busy.");
                continue;
            }
            /* 初始化客户连接*/
            users[connfd].init(connfd, client_address);
        } else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
            /* 如果有异常，直接关闭客户连接*/
            users[sockfd].close_conn();
        } else if(events[i].events & EPOLLIN) {
            /* 根据读的结果，决定是将任务添加到线程池，还是关闭连接*/
            if(users[sockfd].read()) {
                pool->append(users + sockfd);
            } else {
                users[sockfd].close_conn();
            }
        } else if(events[i].events & EPOLLOUT) {
            /* 根据写的结果，决定是否关闭连接*/
            if(!users[sockfd].write()) {
                log(LOG_INFO, __FILE__, __LINE__, "close 3");
                users[sockfd].close_conn();
            }
        } else {

        }
    }
}