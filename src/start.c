#include "argparse.h"
#include <stdio.h>

//struct start_opts {
//    int daemonize;
//    int cpu_perc;
//    unsigned char net[4];
//    int forward_argc;
//    char **forward_argv;
//};

static struct option long_options[] = {
    {"cpu", required_argument, 0, 'c'},
    {"net", required_argument, 0, 'n'},
    {0, 0, 0, 0}
};

static char * short_options = "d";


static int start_opts_parser(struct aucont_parser* parser, void * data, int ch, int index) {
    unsigned char ip[4];
    switch(ch) {
        case 'c':
            printf("cpu set to %d\n", atoi(aucont_next_arg()));
            break;
        case 'n':
            sscanf(aucont_next_arg(), "%d.%d.%d.%d", ip, ip + 1, ip + 2, ip + 3);
            printf("ip %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
            break;
        case 'd':
            printf("will daemon\n");
            break;
        default:
            return CHECKER_SKIP;
    }
    return CHECKER_OK;
}


int main(int argc, char** argv) {
    struct aucont_parser parser = {
        .checker = start_opts_parser,
        .opts = long_options,
        .optstring = short_options
    };
    int ret = aucont_parse_args(&argc, &argv, &parser, NULL);
    for(int i = 0; i < argc; i++) {
        printf("SKIPPED %s\n", argv[i]);
    }
}
