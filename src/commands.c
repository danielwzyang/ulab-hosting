#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ulab-hosting/commands.h"
#include "ulab-hosting/reservations.h"

static int cmd_reserve(int argc, char * const *argv);
static int parse_port(const char *arg, uint16_t *port);

const Command commands[] = {
    { "reserve", "[port]", "reserve a port forwarded to hosting.ee.cooper.edu/~<user>/", cmd_reserve },
};

const size_t num_commands = sizeof commands / sizeof commands[0];

void print_help(const char *prog, FILE *out) {
    fprintf(out, "Usage: %s <command> [arguments]\n", prog);
    fputs("\n", out);
    fputs("Commands:\n", out);

    for (size_t i = 0; i < num_commands; i++) {
        char name_and_args[32];
        snprintf(name_and_args, 32, "%s %s", commands[i].name, commands[i].args);
        
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

    if (argc > 2) {
        // port given
        if (parse_port(argv[2], &port) != 0) {
            fprintf(stderr, "Invalid port: '%s'. Expected [%d-%d]\n", argv[2], MIN_PORT, MAX_PORT);
            return EXIT_FAILURE;
        }
    }

    Reservation reservations[NUM_PORTS] = {0};
    get_reservations(reservations);

    return EXIT_SUCCESS;
} 

static int parse_port(const char *arg, uint16_t *port) {
    char *end;
    long value = strtol(arg, &end, 10);

    // 3 fail conditions:
    // no digits found e.g "abc123"
    // number followed by non-digit characters e.g. "123abc"
    // port out of range
    if (end == arg || *end != '\0' || value < MIN_PORT || value > MAX_PORT ) {
        return EXIT_FAILURE;
    }

    *port = (uint16_t) value;
    return EXIT_SUCCESS; 
}
