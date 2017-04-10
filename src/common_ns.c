#include "utc_ns.h"
#include "common_ns.h"

#define IS_FLAG_SET(a, flag) (a & flag)
#define RUN(f, ...) do { \
    if (f) \
        f(__VA_ARGS__); \
} while(0);

#define CONTAINER_HOSTNAME "container"


void ns_prepare(struct child_ns_arg * arg, int flags) {
    memset(arg, 0, sizeof(*arg));
    arg->hostname = CONTAINER_HOSTNAME;
    arg->ns_flags = flags;
    if (IS_FLAG_SET(flags, CLONE_NEWUTS)) {
        arg->utc_setup = setup_utc_ns;
    }
}

void ns_setup(struct child_ns_arg * arg) {
    RUN(arg->utc_setup, arg->hostname);
}

