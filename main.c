#include <stdio.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <string.h>

int main(int argc, char *argv[]) {
    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path,
        "/run/fw-fanctrl/.fw-fanctrl.commands.sock",
        sizeof(address.sun_path) - 1); // -1 for null terminator \0
    int SOCKET_FD = socket(AF_UNIX, SOCK_STREAM, 0);
    int rec;
    rec = connect(SOCKET_FD, (const struct sockaddr*) &address, sizeof(address));
    if (rec == -1) {
        perror("failed to connect to socket");
        return -1;
    }
    char cmd[] = "print list"; // change later
    rec = send(SOCKET_FD, cmd, sizeof(cmd) - 1, 0);
    if (rec == -1) {
        perror("failed to send data to socket");
        return -1;
    }
    char rec_buf[108];
    rec = recv(SOCKET_FD, rec_buf, 108, 0);
    if (rec < 0) {
        perror("failed to receive from socket");
        return -1;
    } else if (rec == 0) {
        perror("socket closed trying to receive");
        return -1;
    }
    printf("%s\n", rec_buf);
    return 0;
}