#include <stdio.h>
#include <unistd.h>

int main() 
{
    pid_t pid = getpid();
    printf("Current process id: %d\n", pid);

    pid_t ppid = getppid();
    printf("Parent process id: %d\n", ppid);

    pid_t pgid = getpgid(0);
    printf("Calling process group id: %d\n", pgid);

    uid_t uid = getuid();
    printf("Real user id: %d\n", uid);

    gid_t gid = getgid();
    printf("Real user group id: %d\n", gid);

    uid_t euid = geteuid();
    gid_t egid = getegid();
    printf("Effective user id: %d\n", euid);
    printf("Effective user group id: %d\n", egid);

    return 0;
}