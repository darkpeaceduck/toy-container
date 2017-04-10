#include <stdlib.h>
#include "engine.h"
#include "log.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        LOG(LOG_NULL, "wrong args");
        return 1;
    }
    struct aucont_exec_args args;
    args.pid = atoi(argv[1]);
    args.cmd_filename = argv[2];
    args.forward_argc = argc - 2;
    args.forward_argv = argv + 2;
    aucont_exec(&args);
}
