#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX_SIZE 1024
#define MAX_USERNAME_SIZE 50
#define MAX_USERS 50
#define OUR_SERVER_NAME "s2"
#include "../Lib/Functions.h"

int startServer(char ip[], char thisPort[], char otherPort[])
{
    int fdmax = -1, listener, newfd;
    char* userNames[MAX_USERS];
    int userCount = 0;
    fd_set read_fds, master;
    struct sockaddr_storage remoteaddr;
    int otherFdActive, otherFd, fileReceiving = 0, firstTime = 1, fileSending = 0;
    char sth[MAX_SIZE];
    FILE* fp = NULL;
    char* fileName = malloc(sizeof(char) * MAX_SIZE);
    for (int i = 0; i < MAX_USERS; i++)
        userNames[i] = malloc(sizeof(char) * (MAX_USERNAME_SIZE + 1));

    initializeSocket(thisPort, ip, &master, &listener, &fdmax);
    int ourFd = listener;
    printf("waiting for the other server...");
    scanf("%99[^\n]%*c", sth);
    otherFd = initializeClient(ip, otherPort);
    FD_SET(otherFd, &master);
    if (otherFd > fdmax)
        fdmax = otherFd;

    while (1) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            printf("select error\n");
            return -1;
        }

        for (int i = 0; i <= fdmax; i++) {
            if (!FD_ISSET(i, &read_fds))
                continue;

            if (i != ourFd) {
                HandleReceive(i, &userCount, userNames, otherFdActive, &fileSending,
                    &fileReceiving, &fp, otherFd, ourFd, fdmax, &master,
                    &fileName, OUR_SERVER_NAME);
                continue;
            }

            socklen_t addrlen = sizeof remoteaddr;
            if ((newfd = accept(ourFd, (struct sockaddr*)&remoteaddr, &addrlen)) == -1) {
                printf("accept error\n");
                continue;
            }

            if (firstTime != 1) {
                FD_SET(newfd, &master);
                if (newfd > fdmax)
                    fdmax = newfd;

                getUserName(newfd, &userCount, userNames);
                continue;
            }

            firstTime = 0;
            otherFdActive = newfd;
            FD_SET(newfd, &master);
            if (newfd > fdmax)
                fdmax = newfd;
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc == 3)
        startServer("127.0.0.1", argv[1], argv[2]);
    else
        printf("not all parameters are given");
}
