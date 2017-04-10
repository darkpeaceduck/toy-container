#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "common_ns.h"
#include "utc_ns.h"

int setup_utc_ns(const char * hostname) {
    if (sethostname(hostname, strlen(hostname)) == -1) {
        return NS_FAIL;
    }
    return NS_OK;
}

int utc_ns_jump(pid_t pid) {
    char buf[100];
    int fd;

    sprintf(buf, "/proc/%d/ns/uts", pid);
    fd = open(buf, O_RDONLY);
    if (fd == -1)
        return 1;
    setns(fd, CLONE_NEWUTS);
    return 0;
}
