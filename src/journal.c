#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "journal.h"

static void journal_check_existiting(const char * journal_location) {
    const char *cmd = "mkdir -p ";
    char * buf = malloc(strlen(cmd) + strlen(journal_location) + 10);
    buf[0] = 0;
    strcat(buf, cmd);
    strcat(buf, journal_location);
    system(buf);
    free(buf);
}

void journal_add_id(const char * journal_location, pid_t id) {
    journal_check_existiting(journal_location);
    char * buf = malloc(strlen(journal_location) + 100);
    buf[0] = 0;
    sprintf(buf, "touch %s/%d", journal_location, id);
    system(buf);
    free(buf);
}

void journal_remove_id(const char * journal_location, pid_t id) {
    char * buf = malloc(strlen(journal_location) + 100);
    buf[0] = 0;
    sprintf(buf, "rm -f %s/%d", journal_location, id);
    system(buf);
    free(buf);
}

void journal_print_ids(const char * journal_location) {
    journal_check_existiting(journal_location);
    char * buf = malloc(strlen(journal_location) + 100);
    buf[0] = 0;
    sprintf(buf, "ls -1 %s", journal_location);
    system(buf);
    free(buf);
}
