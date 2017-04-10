#pragma once
#define _GNU_SOURCE
#include <sched.h>

#define NS_FAIL 1
#define NS_OK 0

//#define ALL_NS (CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWNS | CLONE_NEWNET | CLONE_NEWPID | CLONE_NEWUSER)
#define ALL_NS (CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWUSER)
struct child_ns_arg {
    int (*user_setup)(void);
    int (*utc_setup)(const char *hostname);
    int (*mount_setup)(const char *image_location);
    const char *hostname;
    int ns_flags;
    const char * image_location;
};

void ns_prepare(struct child_ns_arg * arg, int flags, const char *image_location);
void ns_setup(struct child_ns_arg * arg);
void ns_jump(pid_t pid, int flag);
