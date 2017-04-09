#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "log.h"

static int level = LOG_NULL;

void LOG(int _level, const char * format, ...) {
   if (level >= _level) {
       va_list args;
       time_t now;
       char buffer[26];
       struct tm* tm_info;

       va_start (args, format);
       time(&now);
       tm_info = localtime(&now);
       strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

       fprintf(stderr, "%s: ", buffer);
       vfprintf(stderr, format, args);
       fprintf(stderr, "\n");
       va_end (args);
   }
}

void set_log_level(int _level) {
    level = _level;
}

int get_log_level() {
    return level;
}
