#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include "common_ns.h"
#include "log.h"

int setup_utc_ns(const char * hostname) {
    if (sethostname(hostname, strlen(hostname)) == -1) {
        return NS_FAIL;
    }
    return NS_OK;
}

int utc_ns_jump(pid_t pid) {
    return ns_common_setns(pid, "uts", CLONE_NEWUTS);
}
