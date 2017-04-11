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
#include <errno.h>

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

static int mount_system_to_image(const char *image_location) {
//    return 0;
    char * proc_location = produce_system_path(image_location, "proc");
    char * sys_location = produce_system_path(image_location, "sys");
    char * dev_location = produce_system_path(image_location, "dev");
    int ret = 0;
    const char * fail_msg = NULL;

    ret = mount("/proc", proc_location, "proc", MS_BIND, NULL);
    if (ret) {
        fail_msg = "mount proc failed";
        goto out;
    }
//
    ret = mount("/sys", sys_location, "sys", MS_REC | MS_BIND, NULL);
    if (ret) {
        fail_msg = "mount sys failed";
        goto out;
    }

    ret = mount("/dev", dev_location, NULL, MS_REC | MS_BIND, NULL);
    if (ret)
        fail_msg = "mount dev failed";
out:
    free(proc_location);
    free(sys_location);
    free(dev_location);
    if (fail_msg)
       LOG(LOG_NULL, "%s errno=%s", fail_msg, strerror(errno));
    return ret;
}

int mount_image(const char *image_location) {
    int ret = 0;
    const char * fail_msg = NULL;

    ret = mount_system_to_image(image_location);
    if (ret) {
        fail_msg = "failed to mount builtin fses";
        goto out;
    }


    char * tmp_old_location = produce_oldroot_path(image_location);
    ret = mount(image_location, image_location, "bind", MS_BIND | MS_REC, NULL);
    if (ret) {
        fail_msg = "failed bind rec mount image itself";
        goto out_free;
    }

    if (syscall(SYS_pivot_root, image_location, tmp_old_location) == -1) {
        ret = 1;
        fail_msg = "pivot root failed";
        goto out_free;
    }


    ret = chdir("/");
    if (ret) {
        fail_msg = "chdir failed (mount  ns)";
        goto out_free;
    }


    ret = chroot("/");
    if (ret) {
        fail_msg = "chroot failed (mount  ns)";
        goto out_free;
    }

    ret = umount2(OLD_ROOT_TMPMNT_PATH, MNT_DETACH);
    if (ret) {
        fail_msg =  "umount failed (mount ns)";
    }

//    ret = mount(NULL, "/proc", "/proc", 0, NULL);
//    if (ret) {
//        fail_msg = "failed mount proc";
//    }
out_free:
    free(tmp_old_location);
out:
    if (fail_msg)
        LOG(LOG_NULL, "%s errno=%s", fail_msg, strerror(errno));
    return ret;
}

int mount_ns_jump(pid_t pid) {
    char buf[100];
    int fd;
    int ret = 0;
    const char * fail_msg = 0;

    sprintf(buf, "/proc/%d/ns/mnt", pid);
    fd = open(buf, O_RDONLY);
    if (fd == -1) {
        ret = 1;
        fail_msg = "failed to open mnt fd";
        goto out;
    }

    ret = setns(fd, 0);
    if (ret) {
        fail_msg = "fail setns mount";
    }
out:
    if (fail_msg)
        LOG(LOG_NULL, "%s errno=%s", fail_msg, strerror(errno));
    return ret;
}
