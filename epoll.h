#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

#include "threadpool.h"
#include "http_conn.h"
#include "util.h"
#include "time_heap.h"

#define TIMESLOT 10

int setnonblocking(int fd);
int createfd(int flags);
void addfd(int epollfd, int fd, bool ont_shot);
void removefd(int epollfd, int fd);
void modfd(int epollfd, int fd, int ev);
int waitfd(int epollfd, epoll_event *event, int max_events, int timeout);
void handle_event(int epollfd, int listenfd, threadpool<http_conn>* pool, http_conn* users,
                  epoll_event *event, int number, int timeout,time_heap *client_time_heap);
void cb_func(http_conn* user_data);

#endif //EPOLL_H
