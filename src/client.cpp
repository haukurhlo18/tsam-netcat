//
// Created by Haukur Hlöðversson on 23/08/2020.
// https://google.github.io/styleguide/cppguide.html
//
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <algorithm>
#include <map>
#include <vector>

#include <iostream>
#include <sstream>
#include <thread>
#include <map>

class Client {
public:

  Client(char* hostname, char* port) {
    this->hostname = hostname;
    this->socket = this->build_socket(hostname, port);
  }

  /**
   * Getter for socket file descriptor
   *
   * @return socket file descriptor
   */
  int socket_fd() {
    return this->socket;
  }

  /**
   * "start" the client,
   * i.e. wait for input and send messages from stdin through socket.
   * this will also spawn a new thread which receives messages the socket.
   */
  void start() {
    std::thread receiver_thread([&]{
      this->receiver();
    });
    char buffer[1024];
    int n;
    while (!this->stop) {
      n = 0;
      bzero(buffer, sizeof(buffer));
      while ((buffer[n++] = getchar()) != '\n') continue;
      if (strncmp(buffer, "exit", 4) == 0 || strncmp(buffer, "logout", 5) == 0) {
        printf("Client Exit...\n");
        this->close();
        receiver_thread.join();
        continue;
      }
      send(this->socket, buffer, sizeof(buffer), 0);
      bzero(buffer, sizeof(buffer));
    }
  }

  /**
   * destructor for client,
   * here we run our "exit" procedure
   */
  ~Client() {
    this->close();
  }

protected:
  int socket;
  char* hostname;
  bool stop = false;

  /**
   * set stop flag to true, for other processes
   * close the socket
   */
  void close() {
    this->stop = true;
    ::close(this->socket);
  }

  /**
   * Start the receiver, which print out messages it receives from the socket.
   * ideal to run in it's own thread, so it's non-blocking
   *
   */
  void receiver() {
    int re;
    char buffer[64];
    bzero(buffer, sizeof(buffer));
    while (!this->stop) {
      re = recv(this->socket, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
      if (re < 1 && !this->stop) {
        fprintf(stderr, "Lost connection to server (%s)\n", this->hostname);
        this->close();
        exit(1);
      }
      printf("%s", buffer);
      bzero(buffer, sizeof(buffer));
    }
  }

  /**
   * Resolves hostname and establishes connection to host via socket
   * Inspiration from getaddrinfo manual page
   *
   * @param hostname
   * @param port
   * @return socket file descriptor
   */
  int build_socket(char* hostname, char* port) {
    struct addrinfo hints, *res, *res0;
    int error;
    int s;
    const char *cause = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    error = getaddrinfo(hostname, port, &hints, &res0);
    if (error) {
      fprintf(stderr, "%s: %s", hostname, gai_strerror(error));
      exit(1);
    }
    s = -1;
    for (res = res0; res; res = res->ai_next) {
      s = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
      if (s < 0) {
        cause = "ERROR opening socket";
        continue;
      }

      if (connect(s, res->ai_addr, res->ai_addrlen) < 0) {
        cause = "ERROR connecting";
        ::close(s);
        s = -1;
        continue;
      }

      break;  /* okay we got one */
    }
    if (s < 0) {
      fprintf(stderr, "%s", cause);
      exit(1);
    }
    freeaddrinfo(res0);
    return s;
  }
};

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <hostname> port\n", argv[0]);
    exit(1);
  }

  Client client(argv[1], argv[2]);
  client.start();

  return 0;
}
