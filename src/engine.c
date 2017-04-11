#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include "engine.h"
#include "utc_ns.h"
#include "common_ns.h"
#include "log.h"
#include "daemon.h"
#include "journal.h"
#include "user_ns.h"

#define STACK_SIZE 1024*1024
#define ENGINE_WORKDIR "/etc/aucont"

char child_stack[STACK_SIZE];

struct child_args {
    struct child_ns_arg ns_arg;
    const char *exec_filename;
    char **exec_argv;
    int daemonize;
};

static void container_cleanup(pid_t pid) {
    journal_remove_id(ENGINE_WORKDIR, pid);
}


static int child_body(void * __arg) {
    /*sleep while mapping changing in host */
    sleep(2);


    LOG(LOG_DEBUG, "entered child body with uid %d", getuid());

    struct child_args * arg = __arg;
    const char *filename = arg->exec_filename;
    char **argv = arg->exec_argv;
    pid_t old_pid = getpid();
    int daemon = arg->daemonize;

    ns_setup(&arg->ns_arg);
    if (daemon) {
        LOG(LOG_DEBUG, "becoming daemon");
        become_daemon(old_pid);
    }
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
    return arg->ns_arg.ns_flags | SIGCHLD;
}

static pid_t born_child(struct aucont_start_args * args) {
    struct child_args * child_arg = malloc(sizeof(struct child_args));
    pid_t ret;

    child_arg->exec_filename = args->cmd_filename;
    child_arg->exec_argv = args->forward_argv;
    child_arg->daemonize = args->daemonize;
    ns_prepare(&child_arg->ns_arg, ALL_NS, args->image_path);
    ret = enter_child_body(child_arg, produce_clone_flags(child_arg));
    return ret;
}

static void check_await(struct aucont_start_args * args, pid_t pid) {
    if (!args->daemonize) {
       int status;
       waitpid(pid, &status, 0);
       container_cleanup(pid);
   }
}

static void temporary_host_setup_ns(pid_t pid) {
    if (ALL_NS & CLONE_NEWUSER) {
        uid_t uid = geteuid();
        gid_t gid = getegid();
        user_ns_change_mapping(pid, 0, 0, uid, gid);
    }
}

int aucont_start(struct aucont_start_args * args) {
    pid_t child_pid = born_child(args);
    if (child_pid == -1) {
        LOG(LOG_DEBUG, "failed to clone child with errno = %s", strerror(errno));
        return 1;
    }
    printf("%d\n", child_pid);

    temporary_host_setup_ns(child_pid);
    journal_add_id(ENGINE_WORKDIR, child_pid);
    check_await(args, child_pid);
    return 0;
}

int aucont_stop(struct aucont_stop_args * args) {
    LOG(LOG_DEBUG, "stopping daemon %d with sig %d", args->pid, args->sig);
    stop_daemon(args->pid, args->sig);
    container_cleanup(args->pid);
}

int aucont_list(struct aucont_list_args *args) {
    journal_print_ids(ENGINE_WORKDIR);
}

int aucont_exec(struct aucont_exec_args *args) {
    ns_jump(args->pid, ALL_NS);
    if (execv(args->cmd_filename, args->forward_argv)) {
        fprintf(stderr, "DIE BITCH %s", strerror(errno));
        exit(2);
    }
}
