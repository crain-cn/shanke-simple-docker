#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int child(void *argv);

// 直接运行 ./a.out empty /bin/bash
int main(int argc, char *argv[]) {
    // 创建新进程
    int pid = clone(child, malloc(4096) + 4096, CLONE_NEWUTS | SIGCHLD, argv);
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
    mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, "");
    // 替换对应的程序，比如调用者传来的 /bin/bash
    setenv("PATH", "/bin", 1);
    // 替换并执行指定的程序
    execvp(argv[2], argv + 2);
    perror(argv[2]);
    return 1;
}
