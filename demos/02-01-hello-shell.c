#include <stdlib.h>
#include <sys/wait.h>

int child();

// 直接运行 ./a.out
int main(int argc, char *argv[]) {
    // 创建新进程
    int pid = clone(child, malloc(4096) + 4096, SIGCHLD, NULL);
    // 等待子进程结束的信号
    waitpid(pid, NULL, 0);
    return 0;
}

int child() {
    char *args[] = {"sh", NULL};
    // 替换并执行指定的程序
    execvp("/bin/sh", args);
    return 1;
}
