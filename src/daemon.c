#include <sys/wait.h>
#include <signal.h>
#include "daemon.h"
pid_t become_daemon() {
    pid_t sid = setsid();
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    return sid;
}

void stop_daemon(pid_t pid, int sig) {
    int status;
    kill(pid, sig);
    waitpid(pid, &status, 0);
}
