
/*
** client.c -- a stream socket client demo
*/

#include "../../include/position.h"
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

class Reader {
  int idx{};
  char buffer_data[BUFFERSIZE];
  char *buffer;
  char *overflow;
  char overflow_data[BUFFERSIZE];

public:
  int length;
	int message_type;
  Reader();

  template <typename T> T read_single() {
    T result;
    auto size_T = sizeof(T);
    std::memcpy(&result, buffer + idx, size_T);
    idx += size_T;
    return result;
  }

  template <typename T, size_t N> std::array<T, N> read() {
    std::array<T, N> result;
    auto num_bytes = N * sizeof(T);
    std::memcpy(result.data(), buffer + idx, num_bytes);
    idx += num_bytes;
    return result;
  }

  template <typename T> std::vector<T> read(int amt) {
    std::vector<T> result(amt);
    auto num_bytes = amt * sizeof(T);
    std::memcpy(result.data(), buffer + idx, num_bytes);
    idx += num_bytes;
    return result;
  }

  int make_buf(int start, int socket);

  void swap_buffer();
};
