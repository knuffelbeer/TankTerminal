#include "../../include/reader.h"
#include <arpa/inet.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "9034" // Port we're listening on

class Server {
  int listener;

  Reader reader = Reader();
  int start{};
  int fd_size = 3;
  int fd_size_out = 2;
  int fd_count = 0;
  struct pollfd *pfds;
  int fd_count_out = 0;
  struct pollfd *pfds_out;

  int get_listener_socket(void);
  void add_to_pfds(int newfd);
  void del_from_pfds(int i);
  void handle_new_connection();
  void check_new_connections(int *fd_count, struct pollfd **pfds);

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
