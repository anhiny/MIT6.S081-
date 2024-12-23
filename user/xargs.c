#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/fs.h"

#define MSGSIZE 16


// echo hello too | xargs echo bye

int main(int argc, char* argv[]) {
    sleep(10);

    char buf[MSGSIZE];
    read(0, &buf, MSGSIZE); // 文件描述符0代表标准化输入，用一个buf去接上一个命令的输入

    char* xargv[MSGSIZE]; // xargv存放xargs的命令行参数
    int xargc = 0;
    for (int i = 1; i < argc; ++i) {
        xargv[xargc] = argv[i];
        xargc++;
    }
    char* p = buf; // 将之前保存的标准化输入组装成xargs的命令行参数，放在先执行的指令后面，当出现"\n"，就应该fork子线程来执行命令
    for (int i = 0; i < MSGSIZE; ++i) {
        if (buf[i] == '\n') {
            int pid = fork();
            if (pid > 0) {
                p = &buf[i + 1];
                wait(0);
            } else {
                buf[i] = 0;
                xargv[xargc] = p;
                xargc++;
                xargv[xargc] = 0;
                xargc++;

                exec(xargv[0], xargv);
                exit(0);
            }
        }
    }
    wait(0);
    exit(0);
}