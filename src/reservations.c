#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "ulab-hosting/reservations.h"

int is_regular_file(const char *path) {
    struct stat path_stat;

    if (stat(path, &path_stat) != 0) {
        return 0;
    }
    
    return S_ISREG(path_stat.st_mode);
}

int get_reservations(Reservation *reservations) {
    return 0;
}
