#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include "engine.h"
#include "utc_ns.h"
#include "common_ns.h"
#include "log.h"
#include "daemon.h"
#include "journal.h"
#include "user_ns.h"
#include "cgroups.h"

#define STACK_SIZE 1024*1024
#define ENGINE_WORKDIR "/etc/aucont"

char child_stack[STACK_SIZE];

struct child_args {
    struct child_ns_arg ns_arg;
    int (*configurate_cgroups)(const char * work_dir);
    const char *exec_filename;
    char **exec_argv;
    int daemonize;
    int cpu;
};

static void container_cleanup(pid_t pid) {
    journal_remove_id(ENGINE_WORKDIR, pid);
}


static int child_body(void * __arg) {
    LOG(LOG_DEBUG, "entered child body with uid %d", getuid());

    struct child_args * arg = __arg;
    const char *filename = arg->exec_filename;
    char **argv = arg->exec_argv;
    pid_t old_pid = getpid();
    int daemon = arg->daemonize;

//    int fd = open("/var/run/my_netns", O_RDONLY);
//    setns(fd, 0);

    net_ns_jump(arg->ns_arg.net_ns_name, 0);
    if (ns_setup(&arg->ns_arg))
        return 1;

//    configurate_cgroups("/tmp/huy", arg->cpu);

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

static pid_t born_child(struct child_args * child_arg, struct aucont_start_args * args) {
    pid_t ret;

    child_arg->exec_filename = args->cmd_filename;
    child_arg->exec_argv = args->forward_argv;
    child_arg->daemonize = args->daemonize;
    child_arg->cpu = args->cpu_perc;
//    child_arg->configurate_cgroups = configurate_cgroups;

//
//    if (system("chown 1000:1000 -R /sys/fs/cgroup/cpu/")) {
//        LOG(LOG_NULL, "failed chown");
//        return -1;
//    }

//    LOG(LOG_NULL, "USER UID %d", getuid());
//    setuid(0);
//    system("mkdir -p ../rootfs/tmp/huy");
//    system("mount -t cgroup -o cpu,cpuacct test ../rootfs/tmp/huy");
//
//    if (system("chown 1000:1000 -R ../rootfs/tmp/huy")) {
//            LOG(LOG_NULL, "failed chown");
//            return -1;
//        }

//    seteuid(1000);

//    system("../aucont/scripts/setup_cgroups.sh");

    if (ns_prepare(&child_arg->ns_arg, ALL_NS, args->image_path, args->net_set ? args->net : NULL))
        return -1;

    ret = enter_child_body(child_arg, produce_clone_flags(child_arg));

    if (ret != -1) {
        ns_post_host(&child_arg->ns_arg, ret);
    }

    return ret;
}

static void check_await(struct aucont_start_args * args, pid_t pid) {
    if (!args->daemonize) {
       int status;
       waitpid(pid, &status, 0);
       container_cleanup(pid);
   }
}

int aucont_start(struct aucont_start_args * args) {
    struct child_args * child_arg = malloc(sizeof(struct child_args));
    memset(child_arg, 0, sizeof(*child_arg));
    pid_t child_pid = born_child(child_arg, args);
    if (child_pid == -1) {
        LOG(LOG_NULL, "failed to start(born child) with errno = %s", strerror(errno));
        return 1;
    }
    journal_add_id(ENGINE_WORKDIR, child_pid);
    journal_put_netns_name(journal_produce_workdir(ENGINE_WORKDIR, child_pid), child_arg->ns_arg.net_ns_name);

    printf("%d\n", child_pid);

    check_await(args, child_pid);
    return 0;
}

int aucont_stop(struct aucont_stop_args * args) {
    LOG(LOG_DEBUG, "stopping daemon %d with sig %d", args->pid, args->sig);
    stop_daemon(args->pid, args->sig);
    container_cleanup(args->pid);
    return 0;
}

int aucont_list(struct aucont_list_args *args) {
    journal_print_ids(ENGINE_WORKDIR);
    return 0;
}

int aucont_exec(struct aucont_exec_args *args) {
    char net_ns[100];
    journal_get_netns_name(journal_produce_workdir(ENGINE_WORKDIR, args->pid), net_ns);
//    LOG(LOG_NULL, "NET NS NAME %s", net_ns);
//    int fd = open("/var/run/netns/my_netns", O_RDONLY);
//    setns(fd, CLONE_NEWNET);
//
    ns_common_setns(args->pid, "net", CLONE_NEWNET);
    if (ns_jump(args->pid, ALL_NS))
        return 1;
//    net_ns_jump(net_ns, 0);
//    if (jump_cgroups("/tmp/huy"))
//        return 1;

//    system("ping 10.0.0.2");
//    LOG(LOG_NULL, "ls -l /bin/ping");
    if (execv(args->cmd_filename, args->forward_argv)) {
        return 1;
    }
    return 0;
}
