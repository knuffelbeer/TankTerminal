#include "../../include/server/server.h"
#include "../../include/position.h"
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <ncurses.h>
#include <queue>
#include <stdexcept>
#include <sys/fcntl.h>
#include <thread>
#include <vector>

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
  if (pfds_in.size() >= 2) {
    printf("already %i connections", static_cast<int>(pfds_in.size()));
    return;
  }

  pfds_in.emplace_back(newfd, POLLIN, 0);
  pfds_out.emplace_back(newfd, POLLOUT, 0);

  if (pfds_in.size() == 2) {
    connections_ready = true;
    for (auto &pfd : pfds_in) {

      int sockfd = pfd.fd;
      int flags = fcntl(sockfd, F_GETFL, 0);
      fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    }
  }
}

void Server::del_from_pfds(int i) {
  pfds_in.erase(pfds_in.begin() + i);
  pfds_out.erase(pfds_out.begin() + 1);
}

void Server::handle_new_connection() {
  struct sockaddr_storage remoteaddr; // Client address
  socklen_t addrlen;
  int newfd; // Newly accept()ed socket descriptor
  char remoteIP[INET6_ADDRSTRLEN];

  addrlen = sizeof remoteaddr;
  newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

  if (newfd == -1) {
    perror("accept");
  } else {
    add_to_pfds(newfd);
    printf("pollserver: new connection from %s on socket %d\n",
           inet_ntop2(&remoteaddr, remoteIP, sizeof remoteIP), newfd);
  }
}

void Server::check_new_connections() {
  if (listenerfd.revents & POLLIN) {
    handle_new_connection();
  }
}

Server::Server() {
  pfds_in.reserve(2);
  pfds_out.reserve(2);
  // Set up and get a listening socket
  listener = get_listener_socket();

  if (listener == -1) {
    fprintf(stderr, "error getting listening socket\n");
    exit(1);
  }

  listenerfd.fd = listener;
  listenerfd.events = POLLIN;

  puts("pollserver: waiting for connections...");
  pfd_data = {{Reader(), 0}, {Reader(), 0}};
}

void Server::listen_for_connections() {
  if ((pfds_in.size() == 2) || connections_ready) {
    printf("already enough connections");
    throw;
  }

  int poll_count = poll(&listenerfd, 1, -1);
  if (poll_count == -1) {
    perror("poll");
    exit(1);
  }
  printf("waiting for players");
  check_new_connections();
  return;
}

void Server::send_data(char *vec, size_t n) {
  int poll_count_out = poll(pfds_out.data(), pfds_out.size(), 0);
  for (const auto &pfd : pfds_out) {
    if (pfd.revents & POLLHUP || pfd.revents & POLLNVAL ||
        pfd.revents & POLLERR)
      throw;
    if (pfd.revents & POLLOUT) {
      int total_bytes_sent{};
      while (total_bytes_sent < n) {
        int bytes_sent = send(pfd.fd, vec, n, 0);
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
  try {
    while (1) {
      int poll_count = poll(pfds_in.data(), pfds_in.size(), -1);
      for (int player = 0; const auto &pfd : pfds_in) {
        auto &[reader, overflow_size] = pfd_data[player];

        if (pfd.revents & POLLHUP || pfd.revents & POLLNVAL ||
            pfd.revents & POLLERR)
          throw;

        if (!(pfd.revents & (POLLIN))) {
          player++;
          continue;
        }

        Message m;
        while (true) {
          reader.make_buf(overflow_size, pfd.fd);
          if (overflow_size < sizeof(Message))
            break;
          int overflow_size_copy = overflow_size;

          Message peek_m = reader.peek_single<Message>(overflow_size_copy);

          if (overflow_size < peek_m.size)
            break;

          m = reader.read_single<Message>(overflow_size);
          uint32_t ch = reader.read_single<uint32_t>(overflow_size);
          reader.read_single<uint32_t>(overflow_size);

          int key = ServerConstants::get(ch, player);
          if (key != -1) {
            std::lock_guard<std::mutex> guard(mtx);
            input.push(key);
            if (key == 'x')
              throw std::runtime_error("player disconnected");
          }
        }
        player++;
      }
    }
  } catch (std::runtime_error &e) {
    std::cerr << "connection closed" << e.what() << '\n';
    {
      std::lock_guard<std::mutex> guard(mtx);
      input.push('x');
    }
    return;
  }
}

Server::~Server() {}
