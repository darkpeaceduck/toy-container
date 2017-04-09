#pragma once
#include <getopt.h>

#define CHECKER_FAIL 1
#define CHECKER_SKIP -1
#define CHECKER_OK 0

struct aucont_parser {
    int (*checker)(struct aucont_parser* parser, void * data, int ch, int index);
    struct option *opts;
    char *optstring;
};


static inline char * aucont_next_arg() {
	return optarg;
}

int aucont_parse_args(int *argc, char ***argv,
        struct aucont_parser *parser, void * data);
