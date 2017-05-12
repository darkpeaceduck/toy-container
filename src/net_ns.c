#define _GNU_SOURCE
#include <sched.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#include "common_ns.h"

#define SCRIPT_PATH "../aucont/scripts/setup_netns.sh"
#define ENV_PATH_PREFIX "/var/run/netns/"


int net_ns_setup(const char * src, const char *dst, pid_t pid) {
    char buf[100];
    sprintf(buf, "%s %d %s %s", SCRIPT_PATH, pid, src, dst);
    return system(buf);
}

int net_ns_jump(pid_t pid, int flag) {
    char path[100];
    int fd;
    int ret = 0;
    const char * fail_msg = 0;

    sprintf(path, "%s%d", ENV_PATH_PREFIX, pid);

    fd = open(path, O_RDONLY);
    if (fd == -1) {
        ret = 1;
        fail_msg = "(common net_ns_jump) failed to open fd";
        goto out;
    }

    ret = setns(fd, flag);
    if (ret) {
        fail_msg = "(commono net_ns_jump) fail setns";
    }
out:
    if (ret)
        LOG(LOG_NULL, "%s errno=%s", fail_msg, strerror(errno));
    return ret;
}

void net_ns_cleanup(pid_t pid) {
    char buf[100];
    sprintf(buf, "ip link delete veth0_%d 2>/dev/null", pid);
    system(buf);
}
