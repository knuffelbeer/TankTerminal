#include "../../include/reader.h"
#include <arpa/inet.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <mutex>
#include <ncurses.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <queue>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define PORT "9034" // Port we're listening on

namespace ServerConstants {
enum {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  SPACE,
};

constexpr std::array<std::array<uint32_t, 2>, 6> player_mappings = {
    {{KEY_UP, 'w'},
     {KEY_DOWN, 's'},
     {KEY_RIGHT, 'd'},
     {KEY_LEFT, 'a'},
     {' ', 'q'},
     {'x', 'x'}}};

inline int get(uint32_t key, int player) {
  for (const auto &e : player_mappings) {
    if (e[0] == key) {
      return e[player];
    }
  }
  return -1;
}

} // namespace ServerConstants
class Server {
  int listener;
  pollfd listenerfd;
  std::vector<std::tuple<Reader, int>> pfd_data;
  std::vector<pollfd> pfds_in;
  std::vector<pollfd> pfds_out;
  void process(int type);
  int get_listener_socket(void);
  void add_to_pfds(int newfd);
  void del_from_pfds(int i);
  void handle_new_connection();
  void check_new_connections();

public:
  void send_data(char *vec, size_t n);
  void recieve_input(std::queue<uint32_t> &input, std::mutex &mtx);
  bool connections_ready{};
  Server();
  Server(Server &&) = default;
  Server(const Server &) = delete;
  Server &operator=(Server &&) = delete;
  Server &operator=(const Server &) = delete;
  ~Server();

  void listen_for_connections();
};

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
