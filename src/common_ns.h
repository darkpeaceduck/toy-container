#pragma once
#define _GNU_SOURCE
#include <sched.h>

#define NS_FAIL 1
#define NS_OK 0

#define ALL_NS (CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWNS | CLONE_NEWNET | CLONE_NEWUSER | CLONE_NEWPID)

struct child_ns_arg {
    int (*user_setup)(void);
    int (*utc_setup)(const char *hostname);
    int (*mount_setup)(const char *image_location);
    const char *hostname;
    int ns_flags;
    const char * image_location;

    int sync_pipe[2];

    int net_set;
    char src_host[100];
    char dst_host[100];
    char net_ns_name[100];
};

int ns_prepare(struct child_ns_arg * arg, int flags, const char *image_location, char * net);
int ns_setup(struct child_ns_arg * arg);
int ns_jump(pid_t pid, int flag);
int ns_post_host(struct child_ns_arg * arg, pid_t pid);

int ns_common_setns(pid_t pid, const char * label, int flag);


int net_ns_setup(const char * src, const char *dst, pid_t pid) ;
int net_ns_jump(pid_t pid, int flag);
void net_ns_cleanup(pid_t pid);

int user_ns_setup();
int user_ns_change_mapping(pid_t pid, uid_t new_uid, gid_t new_gid, uid_t old_uid, gid_t old_gid);
int user_ns_jump(pid_t pid);

int setup_utc_ns(const char * hostname);
int utc_ns_jump(pid_t pid);

int mount_image(const char *image_location);
int mount_ns_jump(pid_t pid);

