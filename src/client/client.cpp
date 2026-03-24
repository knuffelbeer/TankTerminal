#include "../../include/client/game.h"
#include "../../include/client/reader.h"
#include "../../include/position.h"
#include <_stdio.h>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ncurses.h>
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
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(BLACK_BLACK, COLOR_BLACK, COLOR_BLACK);
  init_pair(BLACK_RED, COLOR_BLACK, COLOR_RED);
  init_pair(BLACK_BLUE, COLOR_BLACK, COLOR_BLUE);
  init_pair(GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
  init_pair(GREEN_YELLOW, COLOR_GREEN, COLOR_YELLOW);
  init_pair(RED_BLACK, COLOR_RED, COLOR_BLACK);
  init_pair(BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);
  init_pair(YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
  init_pair(BLACK_WHITE, COLOR_BLACK, COLOR_WHITE);
  init_pair(WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
  init_pair(WHITE_WHITE, COLOR_WHITE, COLOR_WHITE);
  init_color(8, 255, 99, 0);
  init_pair(BLACK_ORANGE, COLOR_BLACK, 8);
  auto game = Game(40, 30);
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
  freeaddrinfo(servinfo); // all done with this structure
  int start = 0;
  auto num_tank_bytes = sizeof(TankLayout) * 2;
  auto num_message_bytes = sizeof(Message);
  auto reader = Reader();
  std::vector<Position> positions;
  std::array<TankLayout, 2> tanks;
  std::vector<WallLayout> walls;
  while (1) {
    start = reader.make_buf(start, sockfd);
    switch (reader.message_type) {
    case 0: {

      assert((reader.length - num_tank_bytes - num_message_bytes) %
                     sizeof(Position) ==
                 0 &&
             "not an interger number of positions\n");

      game.remove(positions);
      game.remove(tanks);

      tanks = reader.read<TankLayout, 2>();
      auto num_positions =
          (reader.length - num_tank_bytes - num_message_bytes) /
          sizeof(Position);
      positions = reader.read<Position>(num_positions);

      game.draw(positions);
      game.draw(tanks);
      break;
    }
    case 1: {
      assert((reader.length - num_message_bytes) % sizeof(WallLayout) == 0 &&
             "not integer number of walls!");

      game.remove(walls);

      auto num_walls = (reader.length - num_message_bytes) / sizeof(WallLayout);
      walls = reader.read<WallLayout>(num_walls);

      game.draw(walls);
      break;
    }
    default: {
      printf("message not valid! message: %i %i", reader.length,
             reader.message_type);
      throw;
    }
    }
    reader.swap_buffer();
    wrefresh(game.my_win);
  }
  close(sockfd);

  return 0;
}
