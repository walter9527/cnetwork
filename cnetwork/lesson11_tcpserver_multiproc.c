#include "common.h"

#define S_IP "192.168.1.102"
#define S_PORT 5006

int main(int argc, char *argv[]) {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) print_err("socket fail", __LINE__, errno);

    struct sockaddr_in saddr = { 0 };
    saddr.sin_family = AF_INET;
    saddr.sin_port   = htons(S_PORT);
    saddr.sin_addr.s_addr = inet_addr(S_IP);


    int ret = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) print_err("bind fail", __LINE__, ret);

    ret = listen(sockfd, 3);
    if (ret == -1) print_err("listen fail", __LINE__, ret);

    struct sockaddr_in caddr = { 0 };
    socklen_t len = sizeof(caddr);
    int cfd = -1;

    while (1) {
        cfd = accept(sockfd, (struct sockaddr *)&caddr, &len);
        if (ret == -1) print_err("accept fail", __LINE__, ret);

        printf("ip: %s, port: %d\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));

        ret = fork();
        if (ret == -1) print_err("fork fail", __LINE__, ret);
        else if (ret > 0) close(cfd); // 由于文件描述符会被子进程继承，因此需要在父进程中关闭该文件描述符，避免互相干扰
        else if (ret == 0) {

//            char buf[10] = { 0 };
//            sprintf(buf, "%d", cfd);
//            char *argv[] = { "./new_pro", buf, NULL};
//
//            execve("./new_pro", argv, environ);
            
//            char buf[20] = { 0 };
//            sprintf(buf, "%s %d", "./new_pro", cfd);
//            system(buf);

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
                    return 0;
                }
            }
        }
    }

    return 0;
}
