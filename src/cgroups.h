#pragma once

struct cgroups_args {
    const char * work_dir;
    pid_t pid;
};

int configurate_cgroups(const char * work_dir, int cpu);
int jump_cgroups(const char * work_dir);
