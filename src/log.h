#pragma once

#define LOG_DEBUG 1
#define LOG_NULL 0

void LOG(int level, const char * format, ...);
void set_log_level(int _level);
int get_log_level();
