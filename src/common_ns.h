#pragma once
#include <sched.h>
#include <unistd.h>

#define NS_FAIL 1
#define NS_OK 0

#define ALL_NS (CLONE_NEWUTS | 0)

struct child_ns_arg {
    int (*utc_setup)(const char *hostname);

    const char *hostname;
    int ns_flags;
};
