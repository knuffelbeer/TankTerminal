#include "../../include/client/reader.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <vector>

#define PORT "9034" // the port client will be connecting to

#define BUFFERSIZE 1000


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  if (argc != 2) {
    fprintf(stderr, "usage: client hostname\n");
    exit(1);
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and connect to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
              sizeof s);
    printf("client: attempting connection to %s\n", s);

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("client: connect");
      close(sockfd);
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
            sizeof s);
  printf("client: connected to %s\n", s);
  freeaddrinfo(servinfo); // all done with this structure

  int start = 0;
  auto num_tank_bytes = sizeof(TankLayout) * 2;
  auto num_message_bytes = sizeof(Message);
  auto reader = Reader();
  while (1) {
    start = reader.make_buf(start, sockfd);
    printf("start: %i\n", start);
    std::cout << "reader_length" << reader.length << '\n';
    std::array<TankLayout, 2> tanks = reader.read<TankLayout, 2>();
    if ((reader.length - num_tank_bytes - num_message_bytes) %
            sizeof(Position) !=
        0) {
      printf("not an interger number of positions\n");
    }

    std::vector<Position> positions = reader.read<Position>(
        (reader.length - num_tank_bytes - num_message_bytes) /
        sizeof(Position));
    for (const auto &t : tanks) {
      std::cout << "tank: x: " << t.x << " y: " << t.y << '\n';
    }

    for (const auto &e : positions)
      std::cout << "x: " << e.x << " y: " << e.y << "type" << e.type_idx <<'\n';
    reader.swap_buffer();
  }

  close(sockfd);

  return 0;
}
