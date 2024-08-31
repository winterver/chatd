#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/fcntl.h>

int fd;
int size;
char buf[80];

#define REP "Reply: "
#define INS "\033[s\n\033[1F\033[1L"
#define RES "\033[u\033[1B"
#define BAK "\033[1F\033[2K"

#define SEND_TIME(fmt) {                                \
    time_t tt = time(NULL);                             \
    struct tm* t = localtime(&tt);                      \
    size = strftime(buf, sizeof(buf), "[%H:%M:%S] ", t);\
    write(fd, buf, size);                               \
    char* user = getenv("USER");                        \
    dprintf(fd, fmt, user);                             \
}

void disconnect() {
    write(fd, INS, sizeof(INS));
    SEND_TIME("INFO: %s left\n");
    write(fd, RES, sizeof(RES));
    putchar('\n');
    close(fd);
}

void handler(int sig) {
    exit(0);
}

int main() {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(12345);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return -1;
    }

    atexit(disconnect);
    signal(SIGINT, handler);
    signal(SIGPIPE, SIG_IGN);

    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    struct pollfd fds[2];
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[1].fd = fd;
    fds[1].events = POLLIN;

    write(fd, INS, sizeof(INS));
    SEND_TIME("INFO: %s joined\n");
    write(fd, RES, sizeof(RES));
    write(1, REP, sizeof(REP));

    while (1) { 
        if (poll(fds, 2, -1) < 0) {
            perror("poll");
            continue;
        }

        if (fds[0].revents & POLLIN) {
            write(1, BAK, sizeof(BAK));
            write(fd, INS, sizeof(INS));
            SEND_TIME("%s: ");
            while ((size = read(0, buf, sizeof(buf))) > 0) {
                write(fd, buf, size);
            }
            write(fd, RES, sizeof(RES));
            write(1, REP, sizeof(REP));
        }
        if (fds[1].revents & POLLIN) {
            while ((size = read(fd, buf, sizeof(buf))) > 0) {
                write(1, buf, size);
            }
            if (size == 0) {
                break;
            }
        }
    }

    return 0;
}
