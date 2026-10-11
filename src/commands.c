#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ulab-hosting/commands.h"
#include "ulab-hosting/reservations.h"

static int cmd_reserve(int argc, char * const *argv);

static const Command commands[] = {
    { "reserve", "[port]", "reserve a port forwarded to hosting.ee.cooper.edu/~<user>/", cmd_reserve },
};

static const size_t num_commands = sizeof commands / sizeof commands[0];

void print_help(const char *prog, FILE *out) {
    fprintf(out, "Usage: %s <command> [arguments]\n", prog);
    fputs("\n", out);
    fputs("Commands:\n", out);

    for (size_t i = 0; i < num_commands; i++) {
        char name_and_args[32];
        snprintf(name_and_args, sizeof name_and_args, "%s %s", commands[i].name, commands[i].args);
        
        fprintf(out, "  %-20s %s\n", name_and_args, commands[i].description);
    }
}

const Command *find_command(const char *name) {
    for (size_t i = 0; i < num_commands; i++) {
        if (strcmp(name, commands[i].name) == 0) {
            return &commands[i];
        }
    }

    return NULL;
}

static int cmd_reserve(int argc, char * const *argv) {
    if (argc > 3) {
        fprintf(stderr, "Too many arguments. Expected %s reserve [port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t port = 0;

    // port given
    if (argc > 2 && parse_port(argv[2], &port) != 0) {
        fprintf(stderr, "Invalid port: '%s'. Expected [%d-%d]\n", argv[2], MIN_PORT, MAX_PORT);
        return EXIT_FAILURE;
    }

    Reservation reservations[NUM_PORTS] = {0};
    const char *path = "/etc/caddy/reservations";
    if (get_reservations(reservations, path) != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
} 

