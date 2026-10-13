#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>

#include "ulab-hosting/reservations.h"

int get_username(char *username) {
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);

    if (pw == NULL) {
        return EXIT_FAILURE;
    }

    snprintf(username, MAX_USER_LEN + 1, "%s", pw->pw_name);

    return EXIT_SUCCESS;
}

int write_reservation_file(const char *username, const uint16_t port) {
    // making temp file and then linking it to prevent malformed reservation files
    char template[128];
    char final_path[128];
    snprintf(template, sizeof template, "%s/.tmp.%s.XXXXXX", RESERVATIONS_PATH, username);
    snprintf(final_path, sizeof final_path, "%s/%s.conf", RESERVATIONS_PATH, username);

    int fd = mkstemp(template);
    if (fd == -1) {
        perror(template);
        return EXIT_FAILURE;
    }

    // fix permissions of file b/c it doesn't inherit from folder it's in
    // rw-r--r--
    if (fchmod(fd, 0644) != 0) {
        perror(template);
        unlink(template);
        close(fd);
        return EXIT_FAILURE;
    }

    FILE *file = fdopen(fd, "w");
    if (file == NULL) {
        perror(template);
        close(fd);
        unlink(template);
        return EXIT_FAILURE;
    }

    int rescode = EXIT_FAILURE;

    char content[128];
    snprintf(content, sizeof content, "import user %s %u\n", username, port);

    if (fputs(content, file) == EOF) {
        perror(template);
        goto done;
    }

    if (fflush(file) == EOF) {
        perror(template);
        goto done;
    }

    if (fsync(fileno(file)) != 0) {
        perror(template);
        goto done;
    }

    int close_rescode = fclose(file);
    file = NULL;
    if (close_rescode == EOF) {
        perror(template);
        goto done;
    }

    if (link(template, final_path) == -1) {
        perror(final_path);
        goto done;
    }

    rescode = EXIT_SUCCESS;

done:
    if (file != NULL) fclose(file);
    unlink(template);
    return rescode;
}

int get_file_lock(int *fd) {
    int lock = open(RESERVATIONS_PATH, O_RDONLY); 
    if (lock == -1) {
        perror(RESERVATIONS_PATH); 
        return EXIT_FAILURE;
    }

    if (flock(lock, LOCK_EX) == -1) {
        perror(RESERVATIONS_PATH);
        close(lock);
        return EXIT_FAILURE;
    }

    *fd = lock;
    
    return EXIT_SUCCESS;
}

void release_lock(int fd) {
    close(fd);
}
