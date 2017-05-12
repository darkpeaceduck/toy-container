#define _GNU_SOURCE
#include <sched.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "common_ns.h"
#include "utc_ns.h"
#include "fs.h"
#include "user_ns.h"
#include "log.h"

#define IS_FLAG_SET(a, flag) (a & flag)

#define RUNE(ret, a, b, f, ...) do { \
    if (IS_FLAG_SET(a, b)) \
        ret = f(__VA_ARGS__); \
} while(0);


#define RUN(a, b, f, ...) do { \
    int ret = 0; \
    RUNE(ret, a, b, f, __VA_ARGS__);\
} while(0);

#define CONTAINER_HOSTNAME "container"


static int ns_prepare_setup(struct child_ns_arg * arg) {
//    int ret = 0;
//    RUNE(ret, arg->ns_flags, CLONE_NEWNET, net_ns_setup, arg->net_ns_name, arg->src_host, arg->dst_host);
//    if (ret)
//       goto out;
//    RUNE(ret, arg->ns_flags, CLONE_NEWNET, net_ns_jump, arg->net_ns_name, 0);
//out:
//    return ret;
    return 0;
}

int ns_prepare(struct child_ns_arg * arg, int flags, const char *image_location, char * net) {
    memset(arg, 0, sizeof(*arg));
    arg->hostname = CONTAINER_HOSTNAME;
    arg->ns_flags = flags;
    arg->image_location = image_location;
    arg->utc_setup = setup_utc_ns;
    arg->mount_setup = mount_image;
    arg->user_setup = user_ns_setup;

    if (pipe(arg->sync_pipe) == -1)
        return 1;

    if (net) {
        arg->net_set = 1;
        unsigned char oct[4];
        sscanf(net, "%hhu.%hhu.%hhu.%hhu", oct, oct + 1, oct + 2, oct + 3);

        strcpy(arg->src_host, net);
        sprintf(arg->dst_host, "%hhu.%hhu.%hhu.%hhu", oct[0], oct[1], oct[2], oct[3] + 1);
    }

    return ns_prepare_setup(arg);
}

int ns_setup(struct child_ns_arg * arg) {
    char ch;
    int ret = 0;

    close(arg->sync_pipe[1]);
    if (read(arg->sync_pipe[0], &ch, 1) != 0) {
        LOG(LOG_NULL, "Failure in child: read from pipe returned != 0\n");
        ret = 1;
        goto out;
    }

    net_ns_jump(getpid(), 0);

    RUNE(ret, arg->ns_flags, CLONE_NEWUSER, arg->user_setup);
    if (ret)
       goto out;

    RUNE(ret, arg->ns_flags, CLONE_NEWUTS, arg->utc_setup, arg->hostname);
    if (ret)
       goto out;

    RUNE(ret, arg->ns_flags, CLONE_NEWNS, arg->mount_setup, arg->image_location);
    if (ret)
       goto out;

out:
    return ret;
}

int ns_common_setns(pid_t pid, const char * label, int flag) {
    char buf[100];
    int fd;
    int ret = 0;
    const char * fail_msg = 0;

    sprintf(buf, "/proc/%d/ns/%s", pid, label);
    fd = open(buf, O_RDONLY);
    if (fd == -1) {
        ret = 1;
        fail_msg = "(common setns) failed to open fd";
        goto out;
    }

    ret = setns(fd, flag);
    if (ret) {
        fail_msg = "(commono setns) fail setns";
    }
out:
    if (fail_msg)
        LOG(LOG_NULL, "%s errno=%s", fail_msg, strerror(errno));
    return ret;
}

int ns_jump(pid_t pid, int flag) {
    int ret = 0;
    RUNE(ret, flag, CLONE_NEWUTS, utc_ns_jump, pid);
    if (ret)
        goto out;

    RUNE(ret, flag, CLONE_NEWNS, mount_ns_jump, pid);
    if (ret)
        goto out;

    RUNE(ret, flag, CLONE_NEWUSER, user_ns_jump, pid);
    if (ret)
            goto out;
out:
    return ret;
}

int ns_post_host(struct child_ns_arg * arg, pid_t pid) {
    int ret = 0;
    if (IS_FLAG_SET(arg->ns_flags, CLONE_NEWUSER)) {
        uid_t uid = geteuid();
        gid_t gid = getegid();
        ret = user_ns_change_mapping(pid, 0, 0, uid, gid);
    }

    if (arg->net_set) {
        ret = net_ns_setup(arg->src_host, arg->dst_host, pid);
        net_ns_jump(pid, 0);
    }
    close(arg->sync_pipe[1]);
    return ret;
}

