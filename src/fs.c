#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "log.h"

#define OLD_ROOT_TMPMNT_PATH "/tmp"

static char * produce_oldroot_path(const char * image_location) {
    char * buf = malloc(strlen(image_location) + strlen(OLD_ROOT_TMPMNT_PATH) + 10);
    buf[0] = 0;
    strcat(buf, image_location);
    strcat(buf, OLD_ROOT_TMPMNT_PATH);
    return buf;
}

static char * produce_system_path(const char * image_location, const char * type) {
    char * buf = malloc(strlen(image_location) + strlen(type) + 10);
    buf[0] = 0;
    strcat(buf, image_location);
    strcat(buf, "/");
    strcat(buf, type);
    return buf;
}

void mount_system_to_image(const char *image_location) {
    char * proc_location = produce_system_path(image_location, "proc");
    char * sys_location = produce_system_path(image_location, "sys");
    char * dev_location = produce_system_path(image_location, "dev");
    mount("proc", proc_location, "proc", 0, NULL);
    mount("sys", sys_location, "sysfs", 0, NULL);
    mount("/dev", dev_location, NULL, MS_BIND, NULL);
    free(proc_location);
    free(sys_location);
    free(dev_location);
}

int mount_image(const char *image_location) {
    mount_system_to_image(image_location);
    char * tmp_old_location = produce_oldroot_path(image_location);
    syscall(SYS_pivot_root, image_location, tmp_old_location);
    chdir("/");
    chroot("/");
    umount2(tmp_old_location, MNT_DETACH);
    free(tmp_old_location);
}

int mount_ns_jump(pid_t pid) {
    char buf[100];
    int fd;

    sprintf(buf, "/proc/%d/ns/mnt", pid);
    fd = open(buf, O_RDONLY);
    if (fd == -1)
        return 1;
    setns(fd, CLONE_NEWNS);
    return 0;
}
