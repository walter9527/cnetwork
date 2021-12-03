#include "common.h"

#define UNIX_SOCK_FILE_A "./UNIX_SOCK_A"
#define UNIX_SOCK_FILE_B "./UNIX_SOCK_B"

int sockfd = -1;

void *pth_fun(void *pth_arg) {
    int ret;
    char buf[100] = { 0 };

    while (1) {
        memset(buf, 0, sizeof(buf));

        ret = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
        if (ret == -1) print_err("recvfrom fail", __LINE__, errno);
        else if (ret > 0) {
            printf("%s\n", buf);
        }
    }

    return NULL;
}

void sig_fun(int signo) {
    if (SIGINT == signo) {
        remove(UNIX_SOCK_FILE_B);
        exit(0);
    }
}

int main(int argc, char *argv[]) {

    signal(SIGINT, sig_fun);

    sockfd = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if (sockfd == -1) print_err("socket fail", __LINE__, errno);

    struct sockaddr_un addr = { 0 };
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, UNIX_SOCK_FILE_B);

    int ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1) print_err("bind fail", __LINE__, errno);

    pthread_t pid;
    ret = pthread_create(&pid, NULL, pth_fun, NULL);
    if (ret != 0) print_err("pthread_create fail", __LINE__, ret);

    char buf[100] = { 0 };
   
    struct sockaddr_un peer_addr = { 0 };
    peer_addr.sun_family = AF_LOCAL;
    strcpy(peer_addr.sun_path, UNIX_SOCK_FILE_A);

    while (1) {
        memset(buf, 0, sizeof(buf));
    
        scanf("%s", buf);

        ret = sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&peer_addr, sizeof(peer_addr));
        if (ret == -1) print_err("sendto fail", __LINE__, errno);
    }

    return 0; 
}
