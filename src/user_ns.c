#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "log.h"
#include "common_ns.h"

int user_ns_setup() {
    return 0;
}


int user_ns_change_mapping(pid_t pid, uid_t new_uid, gid_t new_gid, uid_t old_uid, gid_t old_gid) {
    char buf[100];
    int ret = 0;

    sprintf(buf, "echo %d %d 1 >> /proc/%d/uid_map", new_uid, old_uid, pid);

    ret = system(buf);
    if (ret)
        goto out;

    sprintf(buf, "echo %d %d 1 >> /proc/%d/gid_map", new_gid, old_gid, pid);

    ret = system(buf);
out:
    if (ret)
        LOG(LOG_NULL, "write mapping failed %s", strerror(errno));
    return ret;
}

int user_ns_jump(pid_t pid) {
    int ret = 0;
    ret = ns_common_setns(pid, "user", CLONE_NEWUSER);
    if (ret)
        goto out;

    ret = user_ns_setup();
out:
    return ret;
}
