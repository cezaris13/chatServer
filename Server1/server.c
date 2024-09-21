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
#define OUR_SERVER_NAME "s1"

#include "../Lib/Functions.h"

int startServer(char ip[], char thisPort[], char otherPort[]) {
  int fdmax = -1, listener, newfd;
  char *userNames[MAX_USERS];
  int userCount = 0;
  fd_set read_fds, master;
  for (int i = 0; i < MAX_USERS; i++)
    userNames[i] = malloc(sizeof(char) * (MAX_USERNAME_SIZE + 1));

  struct sockaddr_storage remoteaddr;

  initializeSocket(thisPort, ip, &master, &listener, &fdmax);
  int ourFd = listener;
  int otherFd;
  int otherFdActive;
  int firstTime = 1;
  int fileReceiving = 0;
  int fileSending = 0;
  char *fileName = malloc(sizeof(char) * (MAX_SIZE));
  FILE *fp = NULL;
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
      if ((newfd = accept(ourFd, (struct sockaddr *)&remoteaddr, &addrlen)) ==
          -1) {
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

      char sth[MAX_SIZE];
      firstTime = 0;
      printf("waiting for the other server...");
      scanf("%99[^\n]%*c", sth);
      otherFd = initializeClient(ip, otherPort);
      FD_SET(newfd, &master);
      FD_SET(otherFd, &master);
      otherFdActive = newfd;
      if (otherFd > fdmax)
        fdmax = otherFd;

      if (newfd > fdmax)
        fdmax = newfd;
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 3)
    startServer("127.0.0.1", argv[1], argv[2]);
  else
    printf("not all parameters are given");
}
