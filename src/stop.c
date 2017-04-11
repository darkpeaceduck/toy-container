#include <stdlib.h>
#include "log.h"
#include "engine.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        LOG(LOG_NULL, "wrong args");
        return 1;
    }
    struct aucont_stop_args args = {
         .pid = atoi(argv[1]),
         .sig = atoi(argv[2])
    };
    return aucont_stop(&args) ? 1 : 0;
}
