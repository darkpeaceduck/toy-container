#define _GNU_SOURCE
#include <sched.h>
#include <string.h>
#include "common_ns.h"
#include "utc_ns.h"
#include "fs.h"
#include "user_ns.h"

#define IS_FLAG_SET(a, flag) (a & flag)
#define RUN(f, ...) do { \
    if (f) \
        f(__VA_ARGS__); \
} while(0);

#define CONTAINER_HOSTNAME "container"


void ns_prepare(struct child_ns_arg * arg, int flags, const char *image_location) {
    memset(arg, 0, sizeof(*arg));
    arg->hostname = CONTAINER_HOSTNAME;
    arg->ns_flags = flags;
    arg->image_location = image_location;
    if (IS_FLAG_SET(flags, CLONE_NEWUTS)) {
        arg->utc_setup = setup_utc_ns;
    }
    if (IS_FLAG_SET(flags, CLONE_NEWNS)) {
        arg->mount_setup = mount_image;
    }
    if (IS_FLAG_SET(flags, CLONE_NEWUSER)) {
        arg->user_setup = user_ns_setup;
    }
}

void ns_setup(struct child_ns_arg * arg) {
    RUN(arg->user_setup);
    RUN(arg->utc_setup, arg->hostname);
    RUN(arg->mount_setup, arg->image_location);
}

void ns_jump(pid_t pid, int flag) {
    if (IS_FLAG_SET(flag, CLONE_NEWUSER)) {
        user_ns_jump(pid);
    }
    if (IS_FLAG_SET(flag, CLONE_NEWUTS)) {
        utc_ns_jump(pid);
    }
    if (IS_FLAG_SET(flag, CLONE_NEWNS)) {
        mount_ns_jump(pid);
    }
}

