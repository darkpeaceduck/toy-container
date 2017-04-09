#include "argparse.h"
#include "log.h"
#include "engine.h"

#include <stdio.h>
#include <stdlib.h>


static struct option long_options[] = {
    {"cpu", required_argument, 0, 'c'},
    {"net", required_argument, 0, 'n'},
    {0, 0, 0, 0}
};

static char * short_options = "d";


static int start_opts_parser(struct aucont_parser* parser, void * data, int ch, int index) {
    struct aucont_start_args * args = data;
    switch(ch) {
        case 'c':
            args->cpu_perc = atoi(aucont_next_arg());
            printf("cpu set to %d\n", args->cpu_perc);
            break;
        case 'n':
            sscanf(aucont_next_arg(), "%hhu.%hhu.%hhu.%hhu", args->net, args->net + 1, args->net + 2, args->net + 3);
            printf("ip %d.%d.%d.%d\n", args->net[0], args->net[1], args->net[2], args->net[3]);
            break;
        case 'd':
            args->daemonize = 1;
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
    struct aucont_start_args start_args;

    int ret = aucont_parse_args(&argc, &argv, &parser, &start_args);

    start_args.forward_argv = argv;
    start_args.forward_argc = argc;
}
