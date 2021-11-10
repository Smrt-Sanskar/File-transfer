// Server.c
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
#define MAXREQ 1000
#define MAXQUEUE 1000

using namespace std;

typedef struct
{
  char name[1000];
} filename;

typedef struct
{
  int sock;
  struct sockaddr address;
  socklen_t addr_len;
  vector <filename> files_transferred;
  long long data_transferred = 0;
} connection_t;

connection_t * users[10];
int cnt;

void * server(void * ptr) {
  connection_t * conn;
  long addr = 0;
  if (!ptr) pthread_exit(0);
  conn = (connection_t *)ptr;
  int consockfd = conn->sock;

  char reqbuf[MAXREQ];
  int n;

  // logic
  while (1) {
    char reply[MAXREQ] = "Welcome to the Server.\n\t-press F to get file.\n\t-press G to get details.\n\t-press E to exit.\n";
    n = write(consockfd, reply, strlen(reply));

    memset(reqbuf, 0, MAXREQ);
    n = read(consockfd, reqbuf, MAXREQ - 1);
    if (n <= 0) {
      close(conn->sock);
      free(conn);
      pthread_exit(0);
      return 0;
    }

    if (reqbuf[0] == 'F') {
      filename f;

      memset(f.name, 0, 1000);
      n = read(consockfd, f.name, 1000 - 1);
      if (n <= 0) {
        close(conn->sock);
        free(conn);
        pthread_exit(0);
        return 0;
      }

      char loc[MAXREQ] = ".//Data//Server//";

      for (int i = 0; i < MAXREQ; i++) {
        loc[i + 17] += f.name[i];
        if (f.name[i] == '\0') break;
      }

      fstream file;
      file.open(loc, ios::in | ios::binary);
      if (file.is_open()) {
        char reply[MAXREQ] = "Y";
        n = write(consockfd, reply, strlen(reply));

        cout << "[LOG] : File is ready to Transmit.\n";

        string contents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        cout << "[LOG] : Transmission Data Size " << contents.length() << " Bytes.\n";

        cout << "[LOG] : Sending...\n";

        int bytes_sent = send(consockfd , contents.c_str() , contents.length() , 0 );
        cout << "[LOG] : Transmitted Data Size " << bytes_sent << " Bytes.\n";

        cout << "[LOG] : File Transfer Complete.\n";

        (conn->files_transferred).push_back(f);
        (conn->data_transferred) += bytes_sent;

        memset(reqbuf, 0, MAXREQ);
        n = read(consockfd, reqbuf, MAXREQ - 1);
        if (n <= 0) {
          close(conn->sock);
          free(conn);
          pthread_exit(0);
          return 0;
        }
      }
      else {
        char reply[MAXREQ] = "N";
        n = write(consockfd, reply, strlen(reply));
        cout << "[ERROR] : File not Found. Disconnecting\n";
        close(conn->sock);
        free(conn);
        pthread_exit(0);
      }
    }
    else if (reqbuf[0] == 'G') {
      char reply[MAXREQ] = "Files transferred:\n";
      int i = 19;
      for (auto x : (conn->files_transferred)) {
        reply[i] = '\t';
        reply[i + 1] = '-';
        i += 2;
        int j = 0;
        while (x.name[j] != '\0') {
          reply[i] = x.name[j];
          i++;
          j++;
        }
        reply[i] = '\n';
        i++;
      }
      n = write(consockfd, reply, strlen(reply));

      char reply1[MAXREQ] = "Data transferred: ";
      i = 18;
      string tmp = to_string(conn->data_transferred);
      for (auto j : tmp) {
        reply1[i] = j;
        i++;
      }
      reply1[i] = ' ';
      reply1[i + 1] = 'b';
      reply1[i + 2] = 'y';
      reply1[i + 3] = 't';
      reply1[i + 4] = 'e';
      reply1[i + 5] = 's';
      reply1[i + 6] = '\n';
      n = write(consockfd, reply1, strlen(reply1));
    }
    else {
      close(conn->sock);
      free(conn);
      pthread_exit(0);
    }
  }

  // end logic

  close(conn->sock);
  free(conn);
  pthread_exit(0);
  return 0;
}


int main() {

  int lstnsockfd, consockfd, clilen, portno = 5033;
  struct sockaddr_in serv_addr, cli_addr;

  connection_t * connection;
  pthread_t thread;

  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port        = htons(portno);

// Server protocol
  /* Create Socket to receive requests*/
  lstnsockfd = socket(AF_INET, SOCK_STREAM, 0);

  /* Bind socket to port */
  bind(lstnsockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  printf("Bounded to port\n");

  /* Listen for incoming connections */
  listen(lstnsockfd, MAXQUEUE);

  while (1) {
    printf("Listening for incoming connections\n");

    connection = (connection_t *)malloc(sizeof(connection_t));
    //clilen = sizeof(cl_addr);

    /* Accept incoming connection, obtaining a new socket for it */
    connection->sock = accept(lstnsockfd, &connection->address,
                              &connection->addr_len);
    printf("Accepted connection\n");

    if (consockfd <= 0) {
      free(connection);
    }
    else {
      pthread_create(&thread, 0, server, (void *)connection);
      pthread_detach(thread);
    }
  }
  close(lstnsockfd);
}
