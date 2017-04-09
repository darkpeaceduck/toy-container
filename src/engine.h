#pragma once

struct aucont_start_args {
    int daemonize;
    int cpu_perc;
    unsigned char net[4];
    int forward_argc;
    char **forward_argv;
};
