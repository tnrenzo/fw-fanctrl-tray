#include "include/fanctrl.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

const char CMD_ACTIVE[] = "print current";
const char CMD_LIST[] = "print list";

int connect_to_fanctrl() {
    struct sockaddr_un address = {AF_UNIX,
                                  "/run/fw-fanctrl/.fw-fanctrl.commands.sock"};
    int tmp = socket(AF_UNIX, SOCK_STREAM, 0);
    if (tmp == -1) {
        perror("Failed to create socket");
        return -1;
    }
    int SOCKET_FD = tmp;
    int ret =
        connect(SOCKET_FD, (const struct sockaddr *)&address, sizeof(address));
    if (ret == -1) {
        perror("Failed to connect to socket");
        goto cleanup;
    }
    return SOCKET_FD;

cleanup:
    close(SOCKET_FD);
    return -1;
}

int send_command(int SOCKET_FD, const char cmd[]) {
    int ret = send(SOCKET_FD, cmd, strlen(cmd), 0);
    if (ret == -1) {
        perror("Failed to send data to socket");
        return -1;
    }
    return 0;
}

int receive_response(int SOCKET_FD, char buf[], int n) {
    int rec = recv(SOCKET_FD, buf, n, 0);
    if (rec == -1) {
        perror("Failed to receive from socket");
        return -1;
    } else if (rec == 0) {
        fprintf(stderr, "Connection closed by peer\n");
        return -1;
    }
    return rec;
}

int send2socket(const char CMD[], char *ret, size_t ret_size) {
    /* Sends CMD to the socket and writes result into ret.
    returns -1 on failure or returns number send/received on success */
    int SOCKET_FD = connect_to_fanctrl();
    if (SOCKET_FD == -1)
        return -1;
    int rec = send_command(SOCKET_FD, CMD);
    if (rec == -1) {
        goto cleanup;
    }
    rec = receive_response(SOCKET_FD, ret, ret_size - 1);
    if (rec == -1) {
        goto cleanup;
    }
    ret[rec] = '\0';
    goto cleanup;

cleanup:
    close(SOCKET_FD);
    return rec;
}

int get_active_strat(char *ret, size_t ret_size) {
    return send2socket(CMD_ACTIVE, ret, ret_size);
}

int get_all_strats(char *ret, size_t ret_size) {
    return send2socket(CMD_LIST, ret, ret_size);
}

int set_strat(const char *strat, char *ret, size_t ret_size) {
    char cmd_buf[128];
    int r = snprintf(cmd_buf, sizeof(cmd_buf), "use %s", strat);
    if (r < 0) {
        perror("failed to set strategy");
        return -1;
    } else if (r >= sizeof(cmd_buf)) {
        fprintf(stderr, "Buffer length exceeded; string truncated");
        return -1;
    }
    return send2socket(cmd_buf, ret, ret_size);
}

int parse_stratlist(char *to_parse, char *ret, size_t ret_size) {
    char *line = strtok(to_parse, "\n");
    ret[0] = '\0';

    while (line != NULL) {
        if (strncmp(line, "- ", 2) == 0) {
            if (strlen(ret) >= ret_size - 1) {
                fprintf(stderr,
                        "Output buffer full, remaining strategies dropped");
                return -1;
            }
            strncat(ret, line + 2, (ret_size - strlen(ret)) - 1);
            strncat(ret, "\n", (ret_size - strlen(ret)) - 1);
        }
        line = strtok(NULL, "\n");
    }
    return 0;
}

int parse_active_strat(char *to_parse, char *ret, size_t ret_size) {
    char *start = strchr(to_parse, '\''); // where the first ' is
    if (start == NULL) {
        fprintf(stderr,
                "Failed to find token \"'\" in beginning of active strategy");
        return -1;
    }
    start++; // skip the opening quote

    char *end = strchr(start, '\''); // where the last ' is
    if (end == NULL) {
        fprintf(stderr,
                "Failed to find token \"'\" in ending of active strategy");
        return -1;
    }
    size_t len = end - start;
    if (len > ret_size - 1) {
        fprintf(stderr, "Output buffer full, remaining bytes dropped");
        return -1;
    }
    strncpy(ret, start, len);
    ret[len] = '\0';
    return 0;
}
