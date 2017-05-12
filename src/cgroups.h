#pragma once

struct cgroups_args {
    const char * work_dir;
    pid_t pid;
};

int configurate_cgroups( int cpu, pid_t pid);
int jump_cgroups(pid_t cont_pid, pid_t add);
