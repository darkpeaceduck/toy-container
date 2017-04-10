#pragma once
#define _GNU_SOURCE
#include <sched.h>

#define NS_FAIL 1
#define NS_OK 0

#define ALL_NS (CLONE_NEWUTS | 0)

struct child_ns_arg {
    int (*utc_setup)(const char *hostname);

    const char *hostname;
    int ns_flags;
};

void ns_prepare(struct child_ns_arg * arg, int flags);
void ns_setup(struct child_ns_arg * arg);
void ns_jump(pid_t pid, int flag);
