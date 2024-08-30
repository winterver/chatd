#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/poll.h>

#define MAX 64

int nfds = 1;
struct pollfd fds[MAX];

void handle_connection() {
    int fd = accept(fds[0].fd, NULL, NULL);

    struct pollfd* ptr = NULL;
    for (int i = 1; i < nfds; i++) {
        ptr = fds[i].fd < 0 ? &fds[i] : NULL;
    }
    if (ptr == NULL && nfds < MAX) {
        ptr = &fds[nfds++];
    }

    if (ptr) {
        ptr->fd = fd;
        ptr->events = POLLIN;
    }
    else {
        char exceed[] = "Max clients exceeded\n";
        send(fd, exceed, sizeof(exceed), 0);
        close(fd);
    }
}

void handle_message(int i) {
    int size;
    char buf[80];

    int fd = fds[i].fd;
    int flag = MSG_DONTWAIT;
    int sflag = MSG_NOSIGNAL;

    while ((size = recv(fd, buf, sizeof(buf), flag)) > 0) {
        write(1, buf, size);
        for (int j = 1; j < nfds; j++) {
            send(fds[j].fd, buf, size, sflag);
        }
    }

    if (size == 0) {
        fds[i].fd = -1;
        close(fd);
    }
}

int main() {
    int lisfd;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(12345);

    if ((lisfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    if (bind(lisfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }
    if (listen(lisfd, SOMAXCONN) < 0) {
        perror("listen");
        return -1;
    }

    fds[0].fd = lisfd;
    fds[0].events = POLLIN;

    while (1) { 
        if (poll(fds, nfds, -1) < 0) {
            perror("poll");
            continue;
        }

        if (fds[0].revents & POLLIN) {
            handle_connection();
        }

        for (int i = 1; i < nfds; i++) {
            if (fds[i].revents & POLLIN) {
                handle_message(i);
            }
        }
    }

    return 0;
}
