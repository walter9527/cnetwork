#include "common.h"

#define S_IP "47.113.103.203"
//#define S_IP "192.168.1.102"
#define S_PORT 5006

/* 封装应用层数据，目前要传输的是学生数据 */
// 学生信息
typedef struct data {
    int stu_num;
    char stu_name[50];
} Data;

int sockfd = -1;

void *pth_fun(void *pth_arg) {
    int ret;
    Data stu_data = { 0 };
    while (1) {
        memset(&stu_data, 0, sizeof(stu_data));

        ret = recv(sockfd, (void *)&stu_data, sizeof(stu_data), 0);
        if (ret > 0) {
            printf("学号：%d\n", ntohl(stu_data.stu_num));
            printf("姓名：%s\n", stu_data.stu_name);
        } else if (ret == -1) {
            print_err("recv fail", __LINE__, errno);
        }
    }
}

void sig_fun(int signo) {
    if (signo == SIGINT) {
//        close(sockfd);
        shutdown(sockfd, SHUT_RDWR);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sig_fun);

    /* 创建套接字文件，并指定 TCP 协议 
     * 客户端的套接字文件描述符直接用于通信
     * */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) print_err("socket fail", __LINE__, errno);

    struct sockaddr_in addr = {0}; // 用于存放你要连接的服务器 ip 和端口

    addr.sin_family      = AF_INET; // 地址
    addr.sin_port        = htons(S_PORT); // 服务器程序的端口
    addr.sin_addr.s_addr = inet_addr(S_IP); // 服务器的 ip，如果是跨网通信，就是服务器的公网 IP

    /* 调用 connect，向服务端发起请求连接 */
    int ret =  connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) print_err("connect fail", __LINE__, errno);

    pthread_t pid;
    ret = pthread_create(&pid, NULL, pth_fun, NULL);
    if (ret != 0) print_err("pthread_create fail", __LINE__, ret);


    Data stu_data = { 0 };
    while (1) {
        memset(&stu_data, 0, sizeof(stu_data));

        printf("请输入学生学号：");
        scanf("%d", &stu_data.stu_num);
        printf("请输入学生姓名：");
        scanf("%s", stu_data.stu_name);

        stu_data.stu_num = htonl(stu_data.stu_num);

        /* 向服务端发送数据 */
        ret = send(sockfd, (void *)&stu_data, sizeof(stu_data), 0);
        if (ret == -1) print_err("send fail", __LINE__, errno);
    }
    return 0;
}
