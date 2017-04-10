#include "common_ns.h"
#include "utc_ns.h"

int setup_utc_ns(const char * hostname) {
    if (sethostname(hostname, strlen(hostname)) == -1) {
        return NS_FAIL;
    }
    return NS_OK;
}
