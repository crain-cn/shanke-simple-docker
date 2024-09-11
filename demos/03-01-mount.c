#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/mount.h>

int child(void *argv);

// 直接运行 ./a.out empty /bin/bash
int main(int argc, char *argv[]) {
    // 创建新进程
    int pid = clone(child, malloc(4096) + 4096, SIGCHLD, argv);
    // 等待子进程结束的信号
    waitpid(pid, NULL, 0);
    return 0;
}

int child(void *arg) {
    char **argv = (char **)arg;
    // 设置根目录
    chroot(argv[1]);
    // 把当前目录设置为根目录
    chdir("/");
    // 挂载 proc 文件系统
    mount("proc", "/proc", "proc", 0, "");
    // 替换并执行指定的程序
    execvp(argv[2], argv + 2);
    perror(argv[2]);
    return 1;
}
