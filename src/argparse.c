#include "argparse.h"
#include <stdio.h>
#include <unistd.h>

int aucont_parse_args(int *argc, char ***argv,
        struct aucont_parser *parser, void * data) {
    int ch;
    int index;
    int ret = CHECKER_OK;

    if (!parser->checker)
        return CHECKER_FAIL;

    opterr = 0;
    while ((ch = getopt_long(*argc, *argv, parser->optstring, parser->opts, &index)) != -1) {
        ret = parser->checker(parser, data, ch, index);
        if (ret != CHECKER_OK) {
            optind--;
            goto out;
        }
    }

out:
    *argc -= optind;
    *argv += optind;
    return ret;
}
