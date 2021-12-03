#include "common.h"

#define S_IP "192.168.1.102"
#define S_PORT 10000

#define PEER_IP "47.113.103.203"
#define PEER_PORT 5006

void getaddr(struct sockaddr_in *addr, char *ip, int port) {
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = inet_addr(ip);
}

int sockfd = -1;

void *pth_fun(void *) {
    int ret = -1;
    char buf[100] = {0};
    struct sockaddr_in peer_addr = { 0 };
    socklen_t len = sizeof(peer_addr);

    while (1) {
        memset(buf, 0, sizeof(buf));

        /* 接收对方发送的数据，并保存对方的 ip 和端口，以便回答 */
        ret = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&peer_addr, &len);
        if (ret == -1) print_err("recvfrom fail", __LINE__, errno);
        else if (ret > 0) {
            printf("peer_port = %d, peer_addr = %s\n", ntohs(peer_addr.sin_port), inet_ntoa(peer_addr.sin_addr));
            printf("%s\n", buf);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("./a.out peer_addr peer_port\n");
        exit(0);
    }
    /* 创建套接字文件，指定 UDP 协议 */
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) print_err("socket fail", __LINE__, errno);


    /* 使用 UDP 来通信，如果要接收数据的话，必须绑定固定的 ip 和端口，方便对方发送数据 */
    struct sockaddr_in peer_addr = { 0 };
    getaddr(&peer_addr, S_IP, S_PORT);
    int ret = bind(sockfd, (struct sockaddr *)&peer_addr, sizeof(peer_addr));
    if (ret == -1) print_err("bind fail", __LINE__, errno);

    pthread_t tid;
    ret = pthread_create(&tid, NULL, pth_fun, NULL);
    if (ret != 0) print_err("pthread_create fail", __LINE__, ret);


    char buf[100] = {0};

    /* 主线程发送数据 */
    while (1) {
        /* 设置对方的 IP 和端口 */
        getaddr(&peer_addr, argv[1], atoi(argv[2]));
//        getaddr(&peer_addr, PEER_IP, PEER_PORT);

        memset(buf, 0, sizeof(buf));

        scanf("%s", buf);

        ret = sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&peer_addr, sizeof(peer_addr)); // 以一个字节为单位的数据不需要进行大小端序转换
        if (ret == -1) print_err("sendto fail", __LINE__, errno);
        printf("send %d\n", ret);
    }

    return 0;
}
