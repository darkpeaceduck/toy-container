#pragma once
#include <stdlib.h>

int user_ns_setup();
int user_ns_change_mapping(pid_t pid, uid_t new_uid, gid_t new_gid, uid_t old_uid, gid_t old_gid);
int user_ns_jump(pid_t pid);
