#include "common.h"

#define S_IP "192.168.1.102"
#define S_PORT 5006

int sockfd;

void *pth_fun(void *pth_arg) {
    int ret = -1;
    char buf[100] = { 0 };

    while (1) {
        memset(buf, 0, sizeof(buf));

        ret = recv(sockfd, buf, sizeof(buf), 0);
        if (ret == -1) print_err("recv fail", __LINE__, errno);
        else if (ret > 0) {
            printf("server say: %s", buf);
        } else if (ret == 0) {
            shutdown(sockfd, SHUT_RDWR);
            break;
        }
    }

    return NULL;
}

void sig_fun(int signo) {
    if (signo == SIGINT) {
        shutdown(sockfd, SHUT_RDWR);
        exit(0);
    }
}

int main(int argc, char *argv[]) {

    signal(SIGINT, sig_fun);
    
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) print_err("socket fail", __LINE__, errno);

    struct sockaddr_in saddr = { 0 };
    saddr.sin_family = AF_INET;    
    saddr.sin_port = htons(S_PORT);
    saddr.sin_addr.s_addr = inet_addr(S_IP);

    int ret = connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) print_err("connect fail", __LINE__, errno);

    pthread_t pid;
    ret = pthread_create(&pid, NULL, pth_fun, NULL);
    if (ret != 0) print_err("pthread_create fail", __LINE__, ret);

    char buf[100] = { 0 };
    while (1) {
        memset(buf, 0, sizeof(buf));

        scanf("%s", buf);

        ret = send(sockfd, buf, sizeof(buf), 0);
        if (ret == -1) print_err("send fail", __LINE__, errno);
    }

    return 0;
}
