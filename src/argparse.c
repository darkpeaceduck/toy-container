#include "argparse.h"
#include <unistd.h>

typedef int (*check_opt_f)(void*, int);

int aucont_parse_args(int argc, char **argv, const char *optstring,
        check_opt_f checker, void *filling_data) {
    int ch;

    if (!checker)
        return 1;

    while ((ch = getopt(argc, argv, optstring)) != -1) {
        if (checker(filling_data, ch)) {
            return -1;
        }
    }

    return 0;
}
