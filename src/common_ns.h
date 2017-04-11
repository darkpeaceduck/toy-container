#pragma once
#define _GNU_SOURCE
#include <sched.h>

#define NS_FAIL 1
#define NS_OK 0

//#define ALL_NS (CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWNS | CLONE_NEWNET | CLONE_NEWPID | CLONE_NEWUSER)
#define ALL_NS (CLONE_NEWUTS | CLONE_NEWUSER | CLONE_NEWNS)
struct child_ns_arg {
    int (*user_setup)(void);
    int (*utc_setup)(const char *hostname);
    int (*mount_setup)(const char *image_location);
    const char *hostname;
    int ns_flags;
    const char * image_location;

    pid_t final_pid; /* host only */
};

void ns_prepare(struct child_ns_arg * arg, int flags, const char *image_location);
int ns_setup(struct child_ns_arg * arg);
int ns_jump(pid_t pid, int flag);
int ns_post_host(struct child_ns_arg * arg);

int ns_common_setns(pid_t pid, const char * label, int flag);
