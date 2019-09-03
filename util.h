#ifndef UTIL_H
#define UTIL_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>


#define PATHLEN 128
#define BUFFERSIZE 1024
#define ADDRESSLEN  32
#define MAXLISTEN   1024

enum CONFIG{CONFIG_OK = 0, CONFIG_ERROR};

typedef struct config{
    char root[PATHLEN];
    char ip[ADDRESSLEN];
    int port;
    int thread_number;
}config_t;

CONFIG read_config(config_t *config, const char* filename);
int socket_bind_listen(config_t *config);
void addsig(int sig, void(handler)(int), bool restart = true);
void show_error(int connfd, const char* info);
#endif //UTIL_H
