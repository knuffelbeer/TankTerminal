#include "../../include/server/server.h"
#include <chrono>
#include <cstdint>
#include <mutex>
#include <queue>
#include <thread>

int Server::get_listener_socket(void) {
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

void Server::add_to_pfds(int newfd) {
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

  (pfds_out)[fd_count_out].fd = newfd;
  (pfds_out)[fd_count_out].events = POLLOUT; // Check_ready-to-write
  (pfds_out)[fd_count_out].revents = 0;
  (fd_count_out)++;

  if (fd_count == fd_size) {
    printf("max_number of connections");
    for (int i = 0; i < fd_size; i++) {
      fcntl(pfds[i].fd, F_SETFL, O_NONBLOCK);
    }
    connections_ready = true;
  }
}

void Server::del_from_pfds(int i) {
  pfds[i] = pfds[fd_count - 1];
  fd_count--;
}

void Server::handle_new_connection() {
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

void Server::check_new_connections(int *fd_count, struct pollfd **pfds) {
  for (int i = 0; i < *fd_count; i++) {
    if ((*pfds)[i].revents & (POLLIN)) {
      if ((*pfds)[i].fd == listener) {
        handle_new_connection();
      }
    }
  }
}

Server::Server() {
  pfds = static_cast<pollfd *>(malloc(sizeof *pfds * fd_size));
  pfds_out = static_cast<pollfd *>(malloc(sizeof *pfds_out * fd_size_out));
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

void Server::listen_for_connections() {
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

void Server::send_data(char *vec, size_t n) {
  int poll_count_out = poll(pfds_out, fd_count_out, 0);
  for (int j = 0; j < fd_size_out; j++) {
    if (pfds_out[j].revents & POLLHUP || pfds_out[j].revents & POLLNVAL ||
        pfds_out[j].revents & POLLERR)
      throw;
    if (pfds_out[j].revents & POLLOUT) {
      int total_bytes_sent{};
      while (total_bytes_sent < n) {
        int bytes_sent = send(pfds_out[j].fd, vec, n, 0);
        if (bytes_sent < 0) {
          printf("error while sending");
          throw;
        }
        total_bytes_sent += bytes_sent;
      }
    }
  }
}

void Server::recieve_input(std::queue<uint32_t> &input, std::mutex &mtx) {
  while (1) {
    int poll_count = poll(pfds, fd_count, -1);
    for (int j = 1; j < fd_size; j++) {
      if (pfds[j].revents & POLLHUP || pfds[j].revents & POLLNVAL ||
          pfds[j].revents & POLLERR)
        throw;
      if (pfds[j].revents & (POLLIN)) {
        reader.make_buf(start, pfds[j].fd);
        uint32_t ch = reader.read_single<uint32_t>();
        if (ch > 0) {
          {
            std::lock_guard<std::mutex> guard(mtx);
            input.push(ch);
            if (ch == 'x') {
							return;
            }
          }
        }
        int player = reader.read_single<uint32_t>();
        reader.swap_buffer();
        printf("ch: %c\n", ch);
        printf("player: %i\n", player);
        printf("chi: %i\n", ch);
      }
    }
  }
}

Server::~Server() {
  free(pfds);
  free(pfds_out);
}
