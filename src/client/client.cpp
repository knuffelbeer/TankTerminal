#include "../../include/buffer.h"
#include "../../include/client/game.h"
#include "../../include/position.h"
#include "../../include/reader.h"
#include <_stdio.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ncurses.h>
#include <netdb.h>
#include <netinet/in.h>
#include <queue>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <vector>

#define PORT "9034" // the port client will be connecting to

void *get_in_addr(struct sockaddr *sa);

class Client {
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  Reader reader = Reader();
  int rv;
  char s[INET6_ADDRSTRLEN];
  int start = 0;
  size_t num_tank_bytes = sizeof(TankLayout) * 2;
  size_t num_message_bytes = sizeof(Message);
  std::vector<Position> positions;
  std::array<TankLayout, 2> tanks;
  std::vector<WallLayout> walls;
  std::queue<uint32_t> input;
  Game game;

public:
  Client(int argc, char *argv[]);
  void iteration();
  ~Client();
};

Client::Client(int argc, char *argv[]) {
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

  if (argc != 2) {
    fprintf(stderr, "usage: client hostname\n");
    exit(1);
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    std::exit(1);
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
    std::exit(2);
  }
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
            sizeof s);
  freeaddrinfo(servinfo); // all done with this structure
  nodelay(stdscr, TRUE);
  game = Game(40, 30);
}

void Client::iteration() {
  for (const auto &wall : walls) {
    game.draw(wall);
  }
  uint32_t ch = getch();
  if (ch != ERR) {
    auto buffer = Buffer();
    auto size_m = 2 * sizeof(uint32_t) + sizeof(Message);
    auto m = Message{static_cast<uint32_t>(size_m), 3};
    buffer.add(m);
    buffer.add((uint32_t)ch);
    buffer.add((uint32_t)0);
    send(sockfd, buffer.data_dynamic.data(), size_m, 0);
  }

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
        (reader.length - num_tank_bytes - num_message_bytes) / sizeof(Position);
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
  wrefresh(game.my_win);
}

Client::~Client() { close(sockfd); }

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
  auto client = Client(argc, argv);
  while (1) {
    try {
      client.iteration();
    } catch (std::runtime_error &e) {
      std::cerr << e.what() << '\n';
      break;
    }
  }
}
