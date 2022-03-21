#ifndef MY_FUNC2_H
#define MY_FUNC2_H
#define MAX_USERS 50
#define MAX_SIZE 1024
#include <stdio.h>
#include <sys/select.h>
char *trimwhitespace(char *str);
char *strremove(char *str, const char *sub);
void sendMessage(int socketFd,char* message);
int containsUserName(char *searchedString, char *userNames[MAX_USERS]);
void getUserName(int socketFd, int *userCount, char *userNames[MAX_USERS]);
void sendFileToServer(char* filePath, int destSocket,int sourceSocket);
void sendFile(char* filePath, int destSocket);
int initializeSocket(char port[],char ip[], fd_set *master,int *listener,int *fdmax);
int initializeClient(char ip[],char port[]);
void HandleReceive(int i,int *userCount, char *userNames[MAX_SIZE],int otherServerFd,int *fileSending,int *fileReceiving,FILE **fp,int otherFd,int ourFd, int fdmax, fd_set *master);
#endif
