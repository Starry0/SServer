#ifndef TIME_HEAP_H
#define TIME_HEAP_H

#include <iostream>
#include <netinet/in.h>
#include <time.h>
#include "http_conn.h"
using std::exception;

#define BUFFER_SIZE 64

class http_conn;   /* 前向声明*/

/* 定时器类*/
class heap_timer{
public:
    heap_timer(int delay) {
        expire = time(NULL) + delay;
    }

public:
    time_t expire;      /* 定时器生效时间*/
    void (*cb_fun)(http_conn*);   /* 定时器的回调函数*/
    http_conn* user_data;     /* 用户数据*/
};


/* 时间堆类*/
class time_heap{
public:
    time_heap(int cap) ;
    time_heap(heap_timer** init_array, int size, int capacity) ;
    ~time_heap();

public:
    /* 添加目标定时器*/
    int add_timer(heap_timer* timer) ;
    void del_timer(heap_timer* timer) ;
    heap_timer* top() const;
    void pop_timer() ;
    void tick() ;
    bool empty() const { return cur_size == 0;}
private:
    void percolate_down(int hole);
    void resize()  ;
private:
    heap_timer** array;     /* 堆数组*/
    int capacity;           /* 堆数组的容量*/
    int cur_size;           /* 堆数组当前保护元素的个数*/
};
#endif //TIME_HEAP_H
