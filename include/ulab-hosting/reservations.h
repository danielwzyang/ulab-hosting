#pragma once

#include <stdint.h>
#include <time.h>

#define MIN_PORT 10000
#define MAX_PORT 10099
#define NUM_PORTS (MAX_PORT - MIN_PORT + 1)
#define MAX_USER_LEN 32 // doesn't include \0

typedef struct {
    char owner[MAX_USER_LEN + 1];
    time_t reservation_date;
} Reservation;

int is_regular_file(const char *path);

int get_reservations(Reservation *reservations);
