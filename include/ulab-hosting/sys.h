#pragma once

#include <stdint.h>

int get_username(char *username);

int write_reservation_file(const char *username, const uint16_t port);

int delete_reservation_file(const char *username);

int get_file_lock(int *fd);

void release_lock(int fd);

