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
#define RAND_SUFFIX_LEN 10
#define ENV_PATH_PREFIX "/var/run/netns/"

static void generate_rand_suffix(char * s) {
    s[RAND_SUFFIX_LEN] = 0;
    static const char * alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < RAND_SUFFIX_LEN; i++) {
        s[i] = alpha[rand() % sizeof(alpha)];
    }
}

int net_ns_setup(char * name, const char * src, const char *dst) {
    char buf[100];
    char suf[100];
    generate_rand_suffix(suf);
    sprintf(name, "aucont-%s", suf);
    sprintf(buf, "%s %s %s %s", SCRIPT_PATH, name, src, dst);
    return system(buf);
}

int net_ns_jump(const char * name, int flag) {
    char path[100];
    int fd;
    int ret = 0;
    const char * fail_msg = 0;

    sprintf(path, "%s%s", ENV_PATH_PREFIX, name);

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
    LOG(LOG_NULL, "%s errno=%s", fail_msg, strerror(errno));
    return ret;
}
