#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ulab-hosting/commands.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        print_help(argv[0], stderr);
        return EXIT_FAILURE;
    }
    
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_help(argv[0], stdout);
        return EXIT_SUCCESS;
    }

    const Command *command = find_command(argv[1]);

    if (command == NULL) {
        fprintf(stderr, "Command not recognized: '%s'.\nSee available commands with %s -h\n", argv[1], argv[0]);
        return EXIT_FAILURE;
    }

    return command->handler(argc, argv);
}
