#pragma once

#include <stdlib.h>

#define MIN_PORT 10000
#define MAX_PORT 10999
#define MAX_USER_LEN 32 // doesn't include \0

/*
    CMD_UNKNOWN = 0,
    CMD_RESERVE, // reserve port
    CMD_RELEASE, // release reservation
    CMD_STATUS, // reservation status i.e. user's port
    CMD_LIST, // list all reserved ports
    CMD_BLAME, // find owner of process taking up user's port
    CMD_KILL // kill process taking up user's port
*/

typedef int (*cmd_handler)(int argc, char * const * argv);

typedef struct {
    const char *name;
    const char *args;
    const char *description;
    cmd_handler handler;
} Command;

extern const Command commands[];
extern const size_t num_commands;

const Command *find_command(const char *name);
