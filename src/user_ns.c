#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "user_ns.h"
#include <fcntl.h>

int user_ns_setup() {
    setuid(0);
    setgid(0);
}


int user_ns_change_mapping(pid_t pid, uid_t new_uid, gid_t new_gid, uid_t old_uid, gid_t old_gid) {
    char buf[100];
    sprintf(buf, "echo %d %d 1 > /proc/%d/uid_map", new_uid, old_uid, pid);
    system(buf);
    sprintf(buf, "echo %d %d 1 > /proc/%d/gid_map", new_gid, old_gid, pid);
    system(buf);
}

int user_ns_jump(pid_t pid) {
    char buf[100];
    int fd;

    sprintf(buf, "/proc/%d/ns/user", pid);
    fd = open(buf, O_RDONLY);
    if (fd == -1){
        return 1;
    }
    setns(fd, CLONE_NEWUSER);
}
