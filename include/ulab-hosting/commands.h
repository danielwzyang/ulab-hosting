#pragma once

#include <stdio.h>
#include <stdint.h>

typedef int (*cmd_handler)(int argc, char * const *argv);

typedef struct {
    const char *name;
    const char *args;
    const char *description;
    cmd_handler handler;
} Command;

void print_help(const char *prog, FILE *out);
const Command *find_command(const char *name);
