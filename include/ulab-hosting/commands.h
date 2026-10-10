#pragma once

#include <stdio.h>
#include <stdint.h>

/*
    CMD_UNKNOWN = 0,
    CMD_RESERVE, // reserve port
    CMD_RELEASE, // release reservation
    CMD_STATUS, // reservation status i.e. user's port
    CMD_LIST, // list all reserved ports
    CMD_BLAME, // find owner of process taking up user's port
    CMD_KILL // kill process taking up user's port
*/

typedef int (*cmd_handler)(int argc, char * const *argv);

typedef struct {
    const char *name;
    const char *args;
    const char *description;
    cmd_handler handler;
} Command;

void print_help(const char *prog, FILE *out);
const Command *find_command(const char *name);
