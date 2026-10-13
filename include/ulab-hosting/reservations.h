#pragma once

#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#define MIN_PORT 10000
#define MAX_PORT 10099
#define NUM_PORTS (MAX_PORT - MIN_PORT + 1)
#define MAX_USER_LEN 32 // doesn't include \0

typedef struct {
    char owner[MAX_USER_LEN + 1];
    time_t reservation_time;
} Reservation;

void print_reservation_header(void);
void print_reservations(const Reservation *reservations, bool include_free);
void print_reservation(uint16_t port, const Reservation *reservation);

int get_reservations(Reservation *reservations, const char *path);

int parse_port(const char *arg, uint16_t *port);
