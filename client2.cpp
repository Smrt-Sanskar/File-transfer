// Client.c
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#define MAXIN 1000
#define MAXOUT 1000

using namespace std;

void client(int sockfd) {
  int n;
  char sndbuf[MAXIN]; char rcvbuf[MAXOUT];
  while (true) {
    memset(rcvbuf, 0, MAXOUT);
    n = read(sockfd, rcvbuf, MAXOUT - 1);
    if (n <= 0) {
      perror("ERROR communicating to server");
      close(sockfd);
      exit(1);
    }
    cout << rcvbuf;
    cin >> sndbuf;
    write(sockfd, sndbuf, strlen(sndbuf)); /* send */
    if (sndbuf[0] == 'F') {
      char reply[MAXIN];
      cout << "Enter the name of file you want from the server.\n";
      cin >> reply;

      int n = write(sockfd, reply, strlen(reply));

      if (n < 0) {
        perror("ERROR communicating to server");
        exit(1);
      }

      memset(rcvbuf, 0, MAXOUT);
      n = read(sockfd, rcvbuf, MAXOUT - 1);
      if (n <= 0) {
        perror("ERROR communicating to server");
        close(sockfd);
        exit(1);
      }

      if (rcvbuf[0] == 'N') {
        cout << "[ERROR] : File not Found. Disconnecting\n";
        close(sockfd);
        return;
      }

      else {
        string loc = ".//Data//Client2//";
        for (int i = 0; i < MAXIN; i++) {
          loc += reply[i];
          if (reply[i] == '\0') break;
        }

        fstream file;
        file.open(loc, ios::out | ios::trunc | ios::binary);
        if (file.is_open()) {
          cout << "[LOG] : File Created.\n";
        }
        else {
          cout << "[ERROR] : File creation failed, Exititng.\n";
          exit(EXIT_FAILURE);
        }

        char buffer[1024] = {};
        int valread = read(sockfd , buffer, 1024);
        cout << "[LOG] : Data received " << valread << " bytes\n";
        cout << "[LOG] : Saving data to file.\n";

        file << buffer;
        cout << "[LOG] : File Saved.\n";

        char reply[MAXOUT] = "OK";
        write(sockfd, reply, strlen(reply));
      }
    }
    else if (sndbuf[0] == 'G') {
      memset(rcvbuf, 0, MAXOUT);
      n = read(sockfd, rcvbuf, MAXOUT - 1);
      if (n <= 0) {
        perror("ERROR communicating to server");
        close(sockfd);
        exit(1);
      }
      cout << rcvbuf;

      memset(rcvbuf, 0, MAXOUT);
      n = read(sockfd, rcvbuf, MAXOUT - 1);
      if (n <= 0) {
        perror("ERROR communicating to server");
        close(sockfd);
        exit(1);
      }
      cout << rcvbuf;
    }
    else {
      // close(sockfd);
      return;
    }
  }
}

// Server address
struct hostent *buildServerAddr(struct sockaddr_in *serv_addr,
                                char *serverIP, int portno) {
  /* Construct an address for remote server */
  memset((char *) serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr->sin_family = AF_INET;
  inet_aton(serverIP, &(serv_addr->sin_addr));
  serv_addr->sin_port = htons(portno);
}


int main() {
  //Client protocol
  char *serverIP = "192.168.43.107";//
  int sockfd, portno = 5033;
  struct sockaddr_in serv_addr;

  buildServerAddr(&serv_addr, serverIP, portno);

  /* Create a TCP socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  /* Connect to server on port */
  connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  printf("Connected to %s:%d\n", serverIP, portno);

  /* Carry out Client-Server protocol */
  client(sockfd);

  /* Clean up on termination */
  close(sockfd);
}