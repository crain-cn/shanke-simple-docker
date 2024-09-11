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
#include <stdarg.h>

int child(void *argv);
void cfcgroup(pid_t pid);
void systemf(const char *format, ...);


// 直接运行 ./a.out empty /bin/bash
int main(int argc, char *argv[]) {
    // 创建新进程的命名空间标识
    int flags = CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET | CLONE_NEWIPC;
    // 创建新进程，新进程在上述独立的命名空间下
    int pid = clone(child, malloc(4096) + 4096, flags | SIGCHLD, argv);
    // 设置 cgroup
    cfcgroup(pid);
    // 等待子进程结束的信号
    waitpid(pid, NULL, 0);
    return 0;
}

int child(void *arg) {
    char **argv = (char **)arg;
    // 把 mount 挂载的目录从共享状态变成私有状态，这样子进程改变 mount 不影响父进程
    mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL);
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

void cfcgroup(pid_t pid) {
    system("mkdir -p /sys/fs/cgroup/cpu/skdocker");
    systemf("mkdir -p /sys/fs/cgroup/cpu/skdocker/%d", pid);
    systemf("echo %d >> /sys/fs/cgroup/cpu/skdocker/%d/tasks", pid, pid);
    systemf("echo 20000 > /sys/fs/cgroup/cpu/skdocker/%d/cpu.cfs_quota_us", pid);
}

void systemf(const char *format, ...) {
    char cmd[128];
    va_list args;
    va_start(args, format);
    vsnprintf(cmd, sizeof(cmd), format, args);
    va_end(args);
    system(cmd);
}