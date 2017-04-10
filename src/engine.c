#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "engine.h"
#include "utc_ns.h"
#include "common_ns.h"
#include "log.h"

#define STACK_SIZE 1024*1024

char child_stack[STACK_SIZE];

struct child_args {
    struct child_ns_arg ns_arg;
    const char *exec_filename;
    char **exec_argv;
};


static int child_body(void * __arg) {
    LOG(LOG_DEBUG, "entered child body");

    struct child_args * arg = __arg;
    const char *filename = arg->exec_filename;
    char **argv = arg->exec_argv;

    ns_setup(&arg->ns_arg);
    free(arg);
    execv(filename, argv);
    return 0;
}


static int enter_child_body(struct child_args * arg, int clone_flags) {
    LOG(LOG_DEBUG, "ready to clone with flags %d", clone_flags);
    pid_t child_pid = clone(child_body, child_stack + STACK_SIZE, clone_flags, arg);
    return child_pid;
}

static int produce_clone_flags(struct child_args * arg) {
    return arg->ns_arg.ns_flags;
}

static pid_t born_child(struct aucont_start_args * args) {
    struct child_args * child_arg = malloc(sizeof(struct child_args));
    pid_t ret;

    child_arg->exec_filename = args->cmd_filename;
    child_arg->exec_argv = args->forward_argv;
    ns_prepare(&child_arg->ns_arg, ALL_NS);
    ret = enter_child_body(child_arg, produce_clone_flags(child_arg));
    return ret;
}

int aucont_start(struct aucont_start_args * args) {
    pid_t child_pid = born_child(args);
    if (child_pid == -1) {
        LOG(LOG_DEBUG, "failed to clone child with errno = %s", strerror(errno));
        return 1;
    }
    printf("%d\n", child_pid);
    return 0;
}
