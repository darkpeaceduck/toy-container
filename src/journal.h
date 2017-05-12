#pragma once
#include <unistd.h>

void journal_add_id(const char * journal_location, pid_t id);
void journal_remove_id(const char * journal_location, pid_t id);
void journal_print_ids(const char * journal_location);
char* journal_produce_workdir(const char * journal_location, pid_t id);
