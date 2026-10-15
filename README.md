# ulab-hosting

A setuid CLI that lets users on a virtual machine at Cooper Union's Microlab reserve a port for their personal web hosting.
The machine runs [Caddy](https://caddyserver.com/) as a reverse proxy, and each reservation becomes a config file that Caddy imports to forward `hosting.ee.cooper.edu/~<user>/` to the user's port.

```
ulab-hosting reserve [port] # reserve a specified port or the first free port
ulab-hosting status [port] # list reservations, or specific port
ulab-hosting release # delete your reservation
ulab-hosting -h # help
```

Example output:

```
$ ulab-hosting status
Port       Owner                               Reservation Date    
---        ---                                 ---                 
10000      daniel.yang                         2026-10-10 19:02    
10050      john.doe                            2026-10-15 00:04    
```

## Workflow

1. user runs `ulab-hosting reserve` or `ulab-hosting release`
2. CLI writes or deletes `/etc/caddy/reservations/<user>.conf`
3. systemd .path unit notices the directory changed and calls the .service
4. systemd .service: validates the caddy config before reloading

Each reservation is one file containing a single line:

```
import user daniel.yang 10000
```

Valid ports are `10000-10099`, though the range can be modified at `include/ulab-hosting/reservations.h`.
Each user can only hold one reservation.

### Caddy Setup

The Caddyfile must live in `/etc/caddy`, and use the snippet `user` as defined in the example file below:

```
(user) {
        # args = [username, port]
        handle_path /~{args[0]}/* {
                reverse_proxy 127.0.0.1:{args[1]}
        }
}

:8080 {
        import reservations/*.conf # relative to Caddyfile
}
```

All user reservations will be stored in the path `/etc/caddy/reservations`.

A systemd .path watches the reservations directory and triggers a .service that runs `caddy validate`, then `systemctl reload caddy`.
If validation fails, Caddy keeps its old configuration.

## Requirements

- Linux
- `gcc` and `make`
- Caddy and systemd configurations as described in [Caddy Setup](#caddy-setup)
- `sudo` permissions

The Makefile compiles with `-std=c11 -D_POSIX_C_SOURCE=200809L`.
C11 hides POSIX functions such as `openat`, `dirfd`, and `getline`, so the gcc feature macro is necessary to compile.

## Build

```
make build # compiles to build/ulab-hosting
make run ARGS="status" # build and run with arguments
make clean # removes build directory
```

## Install

```
make install # builds, then installs /usr/local/bin/ulab-hosting
```

This runs `install -o root -g root -m 4755`, which creates a root-owned, setuid binary.
Remove it with `make uninstall`.

To test the build without installing:

```
make setuid # chown root + setuid on build/ulab-hosting
./build/ulab-hosting reserve
```

Rebuilding replaces the binary and removes the setuid bit, so rerun `make setuid` after every test build.

