#include <string.h>

#include "ulab-hosting/commands.h"

static int cmd_reserve(int argc, char * const * argv);

const Command commands[] = {
    { "reserve", "[port]", "reserve a port forwarded to hosting.ee.cooper.edu/~<user>/", cmd_reserve },
};

const size_t num_commands = sizeof commands / sizeof commands[0];

const Command *find_command(const char *name) {
    for (size_t i = 0; i < num_commands; i++) {
        if (strcmp(name, commands[i].name) == 0) {
            return &commands[i];
        }
    }

    return NULL;
}

static int cmd_reserve(int argc, char * const * argv) {
    return EXIT_SUCCESS;
} 
