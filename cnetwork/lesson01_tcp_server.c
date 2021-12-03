#include "common.h"

#define S_IP "192.168.1.102"
#define S_PORT 8000

/* 封装应用层数据，目前要传输的是学生数据 */
// 学生信息
typedef struct data {
    int stu_num;
    char stu_name[50];
} Data;

int cfd = -1;

/* 从客户端获取数据 */
void *pth_fun(void *pth_arg) {
    int ret;
    Data stu_data = {0};
    while (1) {
        memset(&stu_data, 0, sizeof(stu_data));

        // ret = read((int)*pth_arg, (void *)&stu_data, sizeof(stu_data));
        ret = recv(cfd, (void *)&stu_data, sizeof(stu_data), 0);
        if (ret > 0) {
            printf("student number: %d\n", ntohl(stu_data.stu_num));
            printf("student name: %s\n", stu_data.stu_name);
        } else if (ret == -1) {
            print_err("recv fail", __LINE__, errno);
        }
    }
    
}

void sig_fun(int signo) {
    if (signo == SIGINT) {
        // 服务器端断开连接
        //close(cfd);
        shutdown(cfd, SHUT_RDWR);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sig_fun);

    /* 创建使用 TCP 协议通信的套接字文件 */
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) print_err("socket fail", __LINE__, errno);

    /* 调用 Bind 绑定套接字文件 ip 端口 */
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET; // 指定 IP 地址格式（地址族）
    saddr.sin_port = htons(S_PORT); // 服务器端口
    saddr.sin_addr.s_addr = inet_addr(S_IP); // 服务器 IP

    /* 调用 Bind 绑定套接字文件，ip 和端口 */
    int ret = bind(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1) print_err("bind fail", __LINE__, errno);

    /* 将主动的“套接字文件描述符”转为被动描述符，用于被动监听客户连接 */
    ret = listen(sockfd, 3);
    if (ret == -1) print_err("listen fail", __LINE__, errno);

    /* 调用 accept 函数，被动监听客户的连接 */
    struct sockaddr_in clnaddr = {0};
    socklen_t len = sizeof(clnaddr);

    cfd = accept(sockfd, (struct sockaddr*)&clnaddr, &len);
    if (cfd == -1) print_err("accept fail", __LINE__, errno);

    // 打印客户的端口和 ip，一定要记得端序的转换
    printf("client_port = %d, client_addr = %s\n", ntohs(clnaddr.sin_port), inet_ntoa(clnaddr.sin_addr));

    pthread_t pid;
    ret = pthread_create(&pid, NULL, pth_fun, NULL);
    if (ret != 0) print_err("pthread_create fail", __LINE__, ret);

    Data stu_data = {0};
    while (1) {
        /* 获取学生学号，但是需要进行端序转换 */
        printf("input student number：");
        scanf("%d", &stu_data.stu_num);
        /* char 不需要进行端序转换 */
        printf("input student name：");
        scanf("%s", stu_data.stu_name);

        stu_data.stu_num = htonl(stu_data.stu_num);
        
        /* 向客户端发送数据 */
        //ret = write(cfd, (void *)&stu_data, sizeof(stu_data));
        ret = send(cfd, (void *)&stu_data, sizeof(stu_data), 0);
        if (ret == -1) print_err("send fail", __LINE__, errno);
    }

    return 0;
}
