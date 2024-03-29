#include "time_heap.h"

time_heap::time_heap(int cap) : capacity(cap), cur_size(0) {
    array = new heap_timer*[cap];
    if(!array) {
        throw std::exception();
    }
    for(int i = 0; i < cap; i ++) {
        array[i] = NULL;
    }
}

time_heap::time_heap(heap_timer** init_array, int size, int capacity) :
cur_size(size), capacity(capacity) {
    if(capacity < size) {
        throw std::exception();
    }
    array = new heap_timer* [capacity];
    if(!array) {
        throw std::exception();
    }
    for(int i = 0; i < capacity; i ++) {
        array[i] = NULL;
    }
    if(size != 0) {
        for(int i = 0; i < size; i ++) {
            array[i] = init_array[i];
        }
        for(int i = (cur_size-1)/2; i >= 0; -- i) {
            percolate_down(i);
        }
    }
}

time_heap::~time_heap() {
    for(int i = 0; i < capacity; i ++) {
        delete array[i];
    }
    delete[] array;
}

int time_heap::add_timer(heap_timer* timer) {
    if(!timer) return -1;
    if(cur_size >= capacity) {
        resize();
    }
    int hole = cur_size++;
    int parent = 0;
    for(; hole > 0; hole = parent) {
        parent = (hole-1)/2;
        if(array[parent]->expire <= timer->expire) {
            break;
        }
        array[hole] = array[parent];
    }
    array[hole] = timer;

    return 0;
}

void time_heap::del_timer(heap_timer* timer) {
    if(!timer) return;
    /* 延迟销毁*/
    timer->cb_fun = NULL;
}

heap_timer* time_heap::top() const {
    if(empty()) {
        return NULL;
    }
    return array[0];
}

void time_heap::pop_timer() {
    if(empty()) {
        return;
    }
    if(array[0]) {
        delete array[0];
        array[0] = array[--cur_size];
        percolate_down(0);
    }
}

void time_heap::tick() {
    heap_timer* tmp = array[0];
    time_t cur = time(NULL);
    while (!empty()) {
        if(!tmp) {
            break;
        }
        if(tmp->expire > cur) {
            break;
        }
        if(array[0]->cb_fun) {
            array[0]->cb_fun(array[0]->user_data);
        }
        pop_timer();
        tmp = array[0];
    }
}

void time_heap::percolate_down(int hole) {
    heap_timer* tmp = array[hole];
    int child = 0;
    for(; ((hole*2+1) <= (cur_size-1)); hole = child) {
        child = hole*2+1;
        if((child+1 < cur_size) && array[child+1]->expire < array[child]->expire) {
            child++;
        }
        if(array[child]->expire < array[hole]->expire) {
            array[hole] = array[child];
        } else {
            break;
        }
    }
    array[hole] = tmp;
}

void time_heap::resize() {
    heap_timer** temp = new heap_timer*[2*capacity];
    for(int i = 0; i < 2*capacity; ++ i) {
        temp[i] = NULL;
    }
    if(!temp) {
        throw std::exception();
    }
    capacity = 2*capacity;
    for(int i = 0; i < cur_size; ++ i) {
        temp[i] = array[i];
    }
    delete[] array;
    array = temp;
}