#include "common.h"

int cfd = -1;

void *pth_fun(void *pth_arg) {
    int ret;
    char buf[100] = { 0 };

    while (1) {
        ret = read(cfd, buf, sizeof(buf));
        if (ret == -1) print_err("read fail", __LINE__, errno);
        else if (ret > 0) {
            printf("%s\n", buf);
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    cfd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (cfd == -1) print_err("socket fail", __LINE__, errno);

    // 对比 TCP，这里是最大的不同
    struct sockaddr_un saddr = { 0 };
    saddr.sun_family = AF_UNIX; // AF_LOCAL
    strcpy(saddr.sun_path, "./UNIX_SOCK");

    int ret = connect(cfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) print_err("connnect fail", __LINE__, errno);

    pthread_t tid;
    ret = pthread_create(&tid, NULL, pth_fun, NULL);
    if (ret != 0) print_err("pthread_create fail", __LINE__, ret);

    char buf[100]= { 0 };

    while (1) {
        memset(buf, 0, sizeof(buf));

        scanf("%s", buf);

        // 注意：这里是本机通信，因此不需要大小端序的转换
        ret = write(cfd, buf, sizeof(buf));
        if (ret == -1) print_err("write fail", __LINE__, errno);
    }

    return 0;
}
