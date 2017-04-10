#pragma once

#include <unistd.h>

struct aucont_start_args {
    int daemonize;
    int cpu_perc;
    unsigned char net[4];

    char * image_path;
    const char * cmd_filename;

    int forward_argc;
    char **forward_argv;
};

struct aucont_stop_args {
    pid_t pid;
    int sig;
};

struct aucont_list_args {
};

struct aucont_exec_args {
    pid_t pid;
    const char * cmd_filename;
    int forward_argc;
    char **forward_argv;
};

int aucont_start(struct aucont_start_args * args);
int aucont_stop(struct aucont_stop_args * args);
int aucont_list(struct aucont_list_args *args);
int aucont_exec(struct aucont_exec_args *args);
