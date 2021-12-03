#include "common.h"

#define S_IP "192.168.1.102"
#define S_PORT 5006

#define PTH_NUMS 100

int sockfd;

void *pth_fun(void *pth_arg) {
    int cfd = (long)pth_arg;

    int ret = -1;
    char buf[100] = { 0 };

    while (1) {
        memset(buf, 0, sizeof(buf));

        ret = recv(cfd, (void *)buf, sizeof(buf), 0);
        if (ret == -1) print_err("recv fail", __LINE__, errno);
        else if (ret > 0) {
            printf("client say: %s\n", buf);
            ret = send(cfd, "recv ok\n", sizeof("recv ok\n"), 0);
            if (ret == -1) print_err("send fail", __LINE__, errno);
        } else if (ret == 0) {
            shutdown(cfd, SHUT_RDWR);
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

    int ret = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) print_err("bind fail", __LINE__, errno);

    ret = listen(sockfd, 3);
    if (ret == -1) print_err("listen fail", __LINE__, errno);

    int cfd = -1;
    struct sockaddr_in caddr = { 0 };
    socklen_t len = sizeof(caddr);

    pthread_t pid;

    while (1) {
        cfd = accept(sockfd, (struct sockaddr *)&caddr, &len);
        if (cfd == -1) print_err("accept fail", __LINE__, errno);

        printf("ip: %s, port: %d\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));

        ret = pthread_create(&pid, NULL, pth_fun, (void *)((long)cfd));
        if (ret != 0) print_err("pthread_create fail", __LINE__, ret);
    }

    return 0;
}
