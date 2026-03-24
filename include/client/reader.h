#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_endian.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cassert>
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
    constexpr auto size_T = sizeof(T);
    assert(size_T % sizeof(uint32_t) == 0 &&
           "type must be composed of 32 bit integers");
    constexpr int amt_member_ints = size_T / sizeof(uint32_t);

    T result;
    uint32_t *int_members = reinterpret_cast<uint32_t *>(&result);
    std::memcpy(int_members, buffer + idx, size_T);

    for (int i = 0; i < amt_member_ints; i++) {
      uint32_t *int_member = int_members + i;
      *int_member = ntohl(*int_member);
    }

    idx += size_T;
    return result;
  }

  template <typename T, size_t N> std::array<T, N> read() {
    std::array<T, N> result;
    for (int i = 0; i < N; i++) {
      result[i] = read_single<T>();
    }
    return result;
  }

  template <typename T> std::vector<T> read(int amt) {
    std::vector<T> result(amt);
    for (int i = 0; i < amt; i++) {
      result[i] = read_single<T>();
    }
    return result;
  }

  int make_buf(int start, int socket);

  void swap_buffer();
};
