#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "log.h"

//#define CONFIGURATE_SCRIPT_PATH

//int preconfigurate_cgroups(const char * work_dir)

int configurate_cgroups(const char * work_dir, int cpu, pid_t pid) {
    char buf[100];
    sprintf(buf, "mkdir -p %s/cont_suka", work_dir);
//    sprintf(buf, "../aucont/scripts/setup_cgroups.sh %s", work_dir);
    int ret = system(buf);
    if (ret) {
        LOG(LOG_NULL, "system ret %d", ret);
        goto out;
    }
//    pid_t pid = getpid();
    sprintf(buf, "echo %d > %s/cont_suka/tasks", pid, work_dir);

    ret = system(buf);
    if (ret) {
        LOG(LOG_NULL, "tasks failed %d", ret);
        goto out;
    }

//    system("cat /sys/fs/cgroup/cpu/cont_suka/tasks");

//    sprintf(buf, "cat %s/cont_suka/tasks", work_dir);
//    ret = system(buf);
//    if (ret) {
//       LOG(LOG_NULL, "cat failed %d", ret);
//       goto out;
//    }

    sprintf(buf, "echo 1000000 > %s/cont_suka/cpu.cfs_period_us", work_dir);
    ret = system(buf);
    if (ret) {
        LOG(LOG_NULL, "failed set fs_period_us %d", ret);
        goto out;
    }


    sprintf(buf, "echo %d > %s/cont_suka/cpu.cfs_quota_us", cpu == -1 ? -1 : 1600000, work_dir);
    ret = system(buf);
    if (ret) {
        LOG(LOG_NULL, "failed set cfs_quota_us %d", ret);
        goto out;
    }

    LOG(LOG_NULL, "CGROUPS OK!!");
out:
    return ret;
}

int jump_cgroups(const char * work_dir, pid_t pid) {
    sleep(1);
    char buf[100];
//    LOG(LOG_NULL, "PID %d", pid);
    sprintf(buf, "echo %d >> %s/cont_suka/tasks", pid, work_dir);

    int ret = system(buf);
    if (ret) {
       LOG(LOG_NULL, "jump failed %d", ret);
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
