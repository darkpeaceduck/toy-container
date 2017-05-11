#include "argparse.h"
#include "log.h"
#include "engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
            break;
        case 'n':
            strcpy(args->net, aucont_next_arg());
            args->net_set = 1;
            break;
        case 'd':
            args->daemonize = 1;
            break;
        default:
            return CHECKER_SKIP;
    }
    return CHECKER_OK;
}


int main(int argc, char** argv) {

//    set_log_level(LOG_DEBUG);
    struct aucont_parser parser = {
        .checker = start_opts_parser,
        .opts = long_options,
        .optstring = short_options
    };
    struct aucont_start_args start_args;
    memset(&start_args, 0, sizeof(start_args));
    start_args.cpu_perc = -1;

    int ret = aucont_parse_args(&argc, &argv, &parser, &start_args);
    if (ret == CHECKER_FAIL || argc < 2) {
        LOG(LOG_NULL, "wrong args");
        return 1;
    }

    start_args.image_path = argv[0];
    start_args.cmd_filename = (const char *)argv[1];

    //assume, that argv[0] is passed, argv[1] is first significant
    start_args.forward_argv = argv + 1;
    start_args.forward_argc = argc - 1;

//    LOG(LOG_NULL, "DAEMONIZE %d", start_args.daemonize);
    LOG(LOG_DEBUG, "cmd %s, image %s", start_args.cmd_filename, start_args.image_path);
    ret = aucont_start(&start_args);
    return (ret ? 1 : 0);
}
