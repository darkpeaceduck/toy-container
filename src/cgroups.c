#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "log.h"
#include <errno.h>

//#define CONFIGURATE_SCRIPT_PATH

//int preconfigurate_cgroups(const char * work_dir)

#define SYSTEM_CPU_SGROUP_PREFIX "/sys/fs/cgroup/cpu"

static void make_work_dir(pid_t pid, char * name) {
    sprintf(name, "%s/%d", SYSTEM_CPU_SGROUP_PREFIX, pid);
}

int configurate_cgroups(int cpu, pid_t pid) {
    char buf[100];
    char work_dir[100];
    const char * fail_msg = NULL;

    make_work_dir(pid, work_dir);
    sprintf(buf, "mkdir -p %s", work_dir);
    int ret = system(buf);
    if (ret) {
        fail_msg = "cannot creatre workdir";
        goto out;
    }


//    sprintf(buf, "echo %d > %s/cont_suka/tasks", pid, work_dir);
//    ret = system(buf);
//    if (ret) {
//        LOG(LOG_NULL, "tasks failed %d", ret);
//        goto out;
//    }

    int nproc =   sysconf (_SC_NPROCESSORS_CONF);
    int period = 1000000;
    int quota = period * cpu * nproc / 100;


    sprintf(buf, "echo %d > %s/cpu.cfs_period_us", period, work_dir);
    ret = system(buf);
    if (ret) {
        fail_msg = "failed set fs_period_us %d";
        goto out;
    }


    sprintf(buf, "echo %d > %s/cpu.cfs_quota_us", cpu == -1 ? -1 : quota, work_dir);
    ret = system(buf);
    if (ret) {
        fail_msg = "failed set cfs_quota_us %d";
        goto out;
    }

out:
    if (fail_msg)
        LOG(LOG_NULL, "%s - %s", fail_msg, strerror(errno));
    return ret;
}

int jump_cgroups(pid_t cont_pid, pid_t add) {
    sleep(1);
    char buf[100];
    char work_dir[100];
    make_work_dir(cont_pid, work_dir);
//    LOG(LOG_NULL, "PID %d", pid);
    sprintf(buf, "echo %d >> %s/tasks", add, work_dir);

    int ret = system(buf);
    if (ret) {
       LOG(LOG_NULL, "failed jump to ns %d", ret);
       goto out;
    }

//    sprintf(buf, "cat %s/cont_suka/tasks", work_dir);
//    ret = system(buf);
//    if (ret) {
//       LOG(LOG_NULL, "cat failed %d", ret);
//       goto out;
//    }
//    LOG(LOG_NULL, "JUMP OKKK!!");
out:
    return ret;
}
