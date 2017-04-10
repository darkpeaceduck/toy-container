#pragma once

#include <unistd.h>

pid_t become_daemon(pid_t);
void stop_daemon(pid_t pid, int sig);
