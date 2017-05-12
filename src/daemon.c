#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include "daemon.h"
#include "log.h"
#include "common_ns.h"

pid_t become_daemon(pid_t pid) {
    pid_t sid = setsid();
    signal (SIGHUP, SIG_IGN);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    return sid;
}

void stop_daemon(pid_t pid, int sig) {
    int status;
    kill(pid, sig);
    waitpid(pid, &status, 0);

    net_ns_cleanup(pid);
}
