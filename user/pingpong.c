#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define RD 0 //pipe的read端
#define WR 1 //pipe的write端



int main(int argc, char* argv[]) {
    char buf = 'P'; // 用于传送的字符
    int statusMode = 0;

    int p2c[2]; // 父进程->子进程
    int c2p[2]; // 子进程->父进程

    pipe(p2c);
    pipe(c2p);

    int pid = fork(); // 创建一个子进程

    if (pid < 0) {
        fprintf(2, "子线程创建失败\n"); // 此时应该关闭两个通信管道，释放资源
        close(p2c[RD]);
        close(p2c[WR]);
        close(c2p[RD]);
        close(c2p[WR]);
        exit(1);
    } else if (pid == 0) {
        // 现在处于子线程
        // 1. 关闭不必要的管道端口
        close(p2c[WR]);
        close(c2p[RD]);
        // 2. 读取父进程传来的数据
        if (read(p2c[RD], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "子线程读取数据失败\n");
            statusMode = 1;
        } else {
            fprintf(1, "%d: received ping\n", getpid());
        }
        // 3. 向父进程发送消息
        if (write(c2p[WR], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "子线程发送数据失败\n");
            statusMode = 1;
        }
        close(p2c[RD]);
        close(c2p[WR]);

        exit(statusMode);
    } else {
        // 现在处于父进程
        // 1. 关闭不必要的管道端口
        close(c2p[WR]);
        close(p2c[RD]);


        // 2. 向子线程发送消息
        if (write(p2c[WR], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "子线程发送数据失败\n");
            statusMode = 1;
        }
        // 3. 读取子进程发来的消息
        if (read(c2p[RD], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "父线程读取数据失败\n");
            statusMode = 1;
        } else {
            fprintf(1, "%d: received pong\n", getpid());
        }


        close(c2p[RD]);
        close(p2c[WR]);

        exit(statusMode);
    }
    return 0;
}























