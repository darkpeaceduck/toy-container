#include <unistd.h>
#include "engine.h"
#include "common_ns.h"

#define STACK_SIZE 1024*1024
static char child_stack[STACK_SIZE];

struct child_args {
    struct child_ns_arg ns_arg;
    const char *exec_filename;
    char **exec_argv;
};


static int child_body(void * _arg) {
    struct child_args * arg = _arg;
    const char *filename = arg->exec_filename;
    char **argv = arg->exec_argv;

    ns_setup(&arg->ns_arg);
    free(arg);
    execv(filename, argv);
}


static int enter_child_body(struct child_args * arg, int clone_flags) {
    pid_t child_pid = clone(child_body, child_stack + STACK_SIZE, clone_flags, arg);
    return child_pid;
}

static int produce_clone_flags(struct child_args * arg) {
    return arg->ns_arg.ns_flags;
}

static pid_t born_child(struct aucont_start_args * args) {
    struct child_arg * child_arg = malloc(sizeof(struct child_arg));
    pid_t ret;

    ns_prepare(&child_arg->child_arg, ALL_NS);
    ret = enter_child_body(&child_arg, produce_clone_flags(&child_arg));
    return ret;
}

int aucont_start(struct aucont_start_args * args) {

}
