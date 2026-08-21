#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <string.h>
#include "include/fanctrl.h"

const char CMD_ACTIVE[] = "print current";
const char CMD_LIST[]   = "print list";

int connect_to_fanctrl() {
    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path,
        "/run/fw-fanctrl/.fw-fanctrl.commands.sock",
        sizeof(address.sun_path) - 1);
    int SOCKET_FD = socket(AF_UNIX, SOCK_STREAM, 0);
    int rec;
    rec = connect(SOCKET_FD, (const struct sockaddr*) &address, sizeof(address));
    if (rec == -1) {
        perror("failed to connect to socket");
        return -1;
    }
    return SOCKET_FD;
}

int send_command(int SOCKET_FD, const char cmd[]) {
    int rec = send(SOCKET_FD, cmd, strlen(cmd), 0);
    if (rec == -1) {
        perror("failed to send data to socket");
        return -1;
    }
    return 0;
}

int receive_response(int SOCKET_FD, char buf[108], int n) {
    int rec = recv(SOCKET_FD, buf, n, 0);
    if (rec < 0) {
        perror("failed to receive from socket");
        return -1;
    } else if (rec == 0) {
        perror("socket closed trying to receive");
        return -1;
    }
    return rec;
}

/* Sends CMD to the socket and writes result into ret */
int send2socket(const char CMD[], char *ret, size_t ret_size) {
    int FD = connect_to_fanctrl();
    if (FD == -1) 
        return EXIT_FAILURE;
    int rec = send_command(FD, CMD);
    if (rec == -1) {
        close(FD);
        return EXIT_FAILURE;
    }
    rec = receive_response(FD, ret, ret_size - 1);
    if (rec == -1) {
        close(FD);
        return EXIT_FAILURE;
    }
    ret[rec] = '\0';
    close(FD);
    return rec;
}

int get_active_strat(char *ret, size_t ret_size) {
    return send2socket(CMD_ACTIVE, ret, ret_size);
}

int get_all_strats(char *ret, size_t ret_size) {
    return send2socket(CMD_LIST, ret, ret_size);
}

int set_strat(const char* strat, char *ret, size_t ret_size) {
    char cmd_buf[50];
    snprintf(cmd_buf, sizeof(cmd_buf), "use %s", strat);
    return send2socket(cmd_buf, ret, ret_size);
}

void parse_stratlist(char *to_parse, char *ret) {
    char *line = strtok(to_parse, "\n");

    ret[0] = '\0';

    while (line != NULL) {
        if (strncmp(line, "- ", 2) == 0) {
            strcat(ret, line + 2);
            strcat(ret, "\n");
        }

        line = strtok(NULL, "\n");
    }
}

void parse_active_strat(char *to_parse, char *ret) {
    char *start = strchr(to_parse, '\'');
    
    if (start != NULL) {
        start++;  // skip the opening quote

        char *end = strchr(start, '\'');
        
        if (end != NULL) {
            size_t len = end - start;
            strncpy(ret, start, len);
            ret[len] = '\0';
        }
    }
}

/*
int main() {
    char str[108];
    char parsed[256]; 
    
    get_active_strat(str, sizeof(str));
    parse_active_strat(str, parsed);
    printf("%s", parsed);
    
    get_all_strats(str, sizeof(str));
    parse_stratlist(str, parsed);
    printf("%s", parsed);

    return 0;
}
*/
