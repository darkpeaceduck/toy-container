#include <sys/wait.h>
#include <signal.h>
#include "daemon.h"
pid_t become_daemon() {
    return setsid();
}

void stop_daemon(pid_t pid, int sig) {
    int status;
    kill(pid, sig);
    waitpid(pid, &status, 0);
}
