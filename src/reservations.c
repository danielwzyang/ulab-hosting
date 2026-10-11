#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#include "ulab-hosting/reservations.h"

static int parse_reservation(int dir_fd, const char *filename, uint16_t *port, Reservation *reservation);

int get_reservations(Reservation *reservations, const char *path) {
    DIR *dir;
    dir = opendir(path);

    if (dir == NULL) {
        perror(path);
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    int rescode = EXIT_FAILURE;
    
    while ((entry = readdir(dir)) != NULL) {
        const char *filename = entry->d_name; 

        // skips ./ , ../ , and hidden files
        if (filename[0] == '.') continue;

        // all valid reservations end in .conf
        size_t length = strlen(filename);
        if (length <= 5 || length > MAX_USER_LEN + 5 || strcmp(filename + length - 5, ".conf") != 0) continue;
        
        uint16_t port = 0;
        Reservation reservation;
        if (parse_reservation(dirfd(dir), filename, &port, &reservation) != 0) {
            goto done;
        }

        size_t index = port - MIN_PORT;
        if (reservations[index].owner[0] != '\0') {
            fprintf(stderr, "%s: Port %u is already claimed by %s\n", filename, (unsigned int) port, reservations[index].owner);
            goto done;
        }

        reservations[index] = reservation;
    }

    rescode = EXIT_SUCCESS;

    done:
    closedir(dir);
    return rescode;
}

static int parse_reservation(int dir_fd, const char *filename, uint16_t *port, Reservation *reservation) {
    int fd = openat(dir_fd, filename, O_RDONLY);
    if (fd == -1) {
        // file couldn't open
        perror(filename);
        return EXIT_FAILURE;
    }

    struct stat filestat;
    if (fstat(fd, &filestat) == -1) {
        perror(filename);
        close(fd);
        return EXIT_FAILURE;
    }

    if (!S_ISREG(filestat.st_mode)) {
        fprintf(stderr, "%s: Not a regular file\n", filename);
        close(fd);
        return EXIT_FAILURE;
    }

    FILE *file = fdopen(fd, "r");
    if (file == NULL) {
        perror(filename);
        close(fd);
        return EXIT_FAILURE;
    }

    int rescode = EXIT_FAILURE;
    char *line = NULL;
    size_t capacity = 0;
    if (getline(&line, &capacity, file) == -1) {
        if (ferror(file)) {
            perror(filename);
        } else {
            fprintf(stderr, "%s: Empty file\n", filename);
        }
        goto done;
    }
    // getting rid of \n from line so parse_port doesn't complain about trailing junk
    // strcspn gives length of string from beginning up to the first newline
    // if there's no newline it rewrites \0; no effect
    line[strcspn(line, "\n")] = '\0';

    char *space = strrchr(line, ' '); // finds last occurrence of space to go to last word in line
    if (space == NULL || parse_port(space + 1, port) != 0) {
        fprintf(stderr, "%s: Invalid reservation file\n", filename);
        goto done;
    }

    size_t username_length = strlen(filename) - 5; // len(.conf) = 5
    memcpy(reservation->owner, filename, username_length);
    reservation->owner[username_length] = '\0';
    reservation->reservation_time = filestat.st_mtime;

    rescode = EXIT_SUCCESS;

    done:
    // no longer own fd after fdopen so no close(fd)
    fclose(file);
    free(line);
    return rescode;
}

int parse_port(const char *arg, uint16_t *port) {
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
