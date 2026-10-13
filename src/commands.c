#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ulab-hosting/commands.h"
#include "ulab-hosting/reservations.h"
#include "ulab-hosting/sys.h"

static int cmd_status(int argc, char * const *argv);
static int cmd_reserve(int argc, char * const *argv);

static const Command commands[] = {
    { "status", "[port]", "view all reserved ports or check status on a specific port", cmd_status },
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

static int cmd_status(int argc, char * const *argv) {
    if (argc > 3) {
        fprintf(stderr, "Too many arguments. Expected %s status [port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t port = 0;

    // port given
    if (argc > 2 && parse_port(argv[2], &port) != 0) {
        fprintf(stderr, "Invalid port: '%s'. Expected [%d-%d]\n", argv[2], MIN_PORT, MAX_PORT);
        return EXIT_FAILURE;
    }

    Reservation reservations[NUM_PORTS] = {0};
    if (get_reservations(reservations) != 0) {
        return EXIT_FAILURE;
    }

    // if port == 0 print all ports otherwise only print specific port
    print_reservation_header();
    if (port == 0) {
        // can potentially add a flag to print free ports when using status as well
        print_reservations(reservations, false);
    } else {
        print_reservation(&reservations[port - MIN_PORT], port);
    }

    return EXIT_SUCCESS;
}

static int cmd_reserve(int argc, char * const *argv) {
    if (argc > 3) {
        fprintf(stderr, "Too many arguments. Expected %s reserve [port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t port = 0;

    if (argc > 2 && parse_port(argv[2], &port) != 0) {
        fprintf(stderr, "Invalid port: '%s'. Expected [%d-%d]\n", argv[2], MIN_PORT, MAX_PORT);
        return EXIT_FAILURE;
    }

    int lock;
    if (get_file_lock(&lock) != 0) {
        return EXIT_FAILURE;
    }

    int rescode = EXIT_FAILURE;

    Reservation reservations[NUM_PORTS] = {0};
    if (get_reservations(reservations) != 0) {
        goto done;
    }
    
    if (port == 0) {
        if (find_free_port(reservations, &port) != 0) {
            fputs("No free ports available.\n", stderr);
            goto done;
        }
    } else if (!is_reservation_free(&reservations[port - MIN_PORT])) {
        fprintf(stderr, "Port %u is already reserved.\n", (unsigned int) port);
        goto done;
    }

    char username[MAX_USER_LEN + 1];
    if (get_username(username) != 0) {
        fputs("Failed to get username.\n", stderr);
        goto done;
    }

    for (size_t i = 0; i < NUM_PORTS; i++) {
        if (strcmp(reservations[i].owner, username) == 0) {
            fprintf(stderr, "You already have port %u reserved.\n", (unsigned int) (i + MIN_PORT));
            goto done;
        }
    }

    if (write_reservation_file(username, port) != 0) {
        goto done;
    }

    rescode = EXIT_SUCCESS;

    printf("Successfully reserved port %u for %s.\n", (unsigned int) port, username);

    done:
    release_lock(lock);
    return rescode;
} 

