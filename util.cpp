#include "util.h"


CONFIG read_config(config_t *config, const char* filename) {
    FILE *fp = fopen(filename, "r");
    if(!fp) return CONFIG_ERROR;
    char buf[BUFFERSIZE];
    int pos = 0;
    while (fgets(buf+pos, BUFFERSIZE- pos, fp)) {
        if(strncasecmp(buf+pos, "threadNumber", 12) == 0) {
            config->thread_number = atoi(buf+pos+13);
        }
        if(strncasecmp(buf+pos, "port", 4) == 0) {
            config->port = atoi(buf+pos+5);
        }
        if(strncasecmp(buf+pos, "root", 4) == 0) {
            strncpy(config->root, buf+pos+5, strlen(buf+pos+5)-1);
        }
        if(strncasecmp(buf+pos, "ip", 2) == 0) {
            strncpy(config->ip, buf+pos+3, strlen(buf+pos+3)-1);
        }
        pos += strlen(buf+pos);
    }
}


int socket_bind_listen(config_t *config) {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);
    struct linger tmp = {1, 0};
    setsockopt(listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(config->port);
    inet_pton(AF_INET, config->ip, &address.sin_addr);

    ret = bind(listenfd, (sockaddr*)&address, sizeof(address));
    assert(ret >= 0);

    ret = listen(listenfd, MAXLISTEN);
    assert(ret >= 0);

    return listenfd;
}

void addsig(int sig, void(handler)(int), bool restart) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    if(restart) {
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

void show_error(int connfd, const char* info) {
    printf("%s",info);
    send(connfd, info, strlen(info), 0);
}