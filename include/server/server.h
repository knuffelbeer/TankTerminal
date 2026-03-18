/*
** pollserver.c -- a cheezy multiperson chat server
*/

#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <span>

#define PORT "9034" // Port we're listening on
/*
 * Convert socket to IP address string.
 * addr: struct sockaddr_in or struct sockaddr_in6
 */
inline const char *inet_ntop2(void *addr, char *buf, size_t size) {
  struct sockaddr_storage *sas = static_cast<sockaddr_storage *>(addr);
  struct sockaddr_in *sa4;
  struct sockaddr_in6 *sa6;
  void *src;

  switch (sas->ss_family) {
  case AF_INET:
    sa4 = static_cast<sockaddr_in *>(addr);
    src = &(sa4->sin_addr);
    break;
  case AF_INET6:
    sa6 = static_cast<sockaddr_in6 *>(addr);
    src = &(sa6->sin6_addr);
    break;
  default:
    return NULL;
  }

  return inet_ntop(sas->ss_family, src, buf, size);
}

/*
 * Return a listening socket.
 */

/*
 * Add a new file descriptor to the set.
 */

class Server {
  int listener; // Listening socket descriptor

  // Start off with room for 5 connections
  // (We'll realloc as necessary)
  int fd_size = 3;
  int fd_count = 0;
  struct pollfd *pfds;

  int get_listener_socket(void) {
    int listener; // Listening socket descriptor
    int yes = 1;  // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
      fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
      exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
      listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (listener < 0) {
        continue;
      }

      // Lose the pesky "address already in use" error message
      setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

      if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
        close(listener);
        continue;
      }

      break;
    }

    // If we got here, it means we didn't get bound
    if (p == NULL) {
      return -1;
    }

    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener, 10) == -1) {
      return -1;
    }

    return listener;
  }

  void add_to_pfds(int newfd) {
    // If we don't have room, add more space in the pfds array
    printf("fd_count %i", fd_count);
    printf("fd_count %i", fd_size);
    if (fd_count > fd_size) {
      printf("already %i connections", fd_size);
      throw;
      return;
    }

    (pfds)[fd_count].fd = newfd;
    (pfds)[fd_count].events = POLLIN; // Check ready-to-read
    (pfds)[fd_count].revents = 0;
    (fd_count)++;

    if (fd_count == fd_size) {
      printf("max_number of connections");
      for (int i = 0; i < fd_size; i++) {
        fcntl(pfds[i].fd, F_SETFL, O_NONBLOCK);
      }
      connections_ready = true;
    }
  }
  void del_from_pfds(int i) {
    pfds[i] = pfds[fd_count - 1];
    fd_count--;
  }
  void handle_new_connection() {
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;
    int newfd; // Newly accept()ed socket descriptor
    char remoteIP[INET6_ADDRSTRLEN];

    addrlen = sizeof remoteaddr;
    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
    // fcntl(newfd, F_SETFL, O_NONBLOCK);

    if (newfd == -1) {
      perror("accept");
    } else {
      add_to_pfds(newfd);

      printf("pollserver: new connection from %s on socket %d\n",
             inet_ntop2(&remoteaddr, remoteIP, sizeof remoteIP), newfd);
    }
  }
  void check_new_connections(int *fd_count, struct pollfd **pfds) {
    for (int i = 0; i < *fd_count; i++) {
      if ((*pfds)[i].revents & (POLLIN)) {
        if ((*pfds)[i].fd == listener) {
          handle_new_connection();
        }
      }
    }
  }

public:
  bool connections_ready{};
  Server() {
    pfds = static_cast<pollfd *>(malloc(sizeof *pfds * fd_size));
    // Set up and get a listening socket
    listener = get_listener_socket();
    // fcntl(listener, F_SETFL, O_NONBLOCK);

    if (listener == -1) {
      fprintf(stderr, "error getting listening socket\n");
      exit(1);
    }

    // Add the listener to set;
    // Report ready to read on incoming connection
    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1; // For the listener

    puts("pollserver: waiting for connections...");
  }

  void listen_for_connections() {
    if ((fd_count == fd_size) || connections_ready) {
      printf("already enough connections");
      throw;
    }

    int poll_count = poll(pfds, fd_count, -1);
    if (poll_count == -1) {
      perror("poll");
      exit(1);
    }
    printf("waiting for players");
    check_new_connections(&fd_count, &pfds);
    return;
  }

  void iteration(char *vec, size_t n) {
    if (!connections_ready) {

      printf("connections not ready!");
      throw;
    }
    int poll_count = poll(pfds, fd_count, 0);

    for (int j = 1; j < fd_size; j++) {
      char buf[10];
      if (pfds[j].revents & POLLHUP || pfds[j].revents & POLLNVAL ||
          pfds[j].revents & POLLERR)
        throw;
      if (pfds[j].revents & (POLLIN)) {
        int nbytes = recv(pfds[j].fd, buf, sizeof buf, 0);
        if (nbytes == 0) {
          printf("player quit");
          throw;
        }
        if (nbytes == -1) {
          if (errno != EAGAIN && errno != EWOULDBLOCK) {
            throw;
          }
        }
        if (nbytes > 0) {
          printf("%s", buf);
        }
      }
      int total_bytes_sent{};
      while (total_bytes_sent < n) {
        int bytes_sent = send(pfds[j].fd, vec, n, 0);
        if (bytes_sent < 0) {
          printf("error while sending");
          throw;
        }
        total_bytes_sent += bytes_sent;
      }
    }
  }
  ~Server() { free(pfds); }
};
