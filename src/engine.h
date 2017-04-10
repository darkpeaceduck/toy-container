#pragma once

struct aucont_start_args {
    int daemonize;
    int cpu_perc;
    unsigned char net[4];

    char * image_path;
    const char * cmd_filename;

    int forward_argc;
    char **forward_argv;
};

int aucont_start(struct aucont_start_args * args);
