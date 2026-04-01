#pragma once
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <stdio.h>
#include <arpa/inet.h>
#include <vector>

class Position;
class WallLayout;
class TankLayout;
class Message;
class Buffer {
  int index{};

public:
  std::vector<char> data_dynamic;

  Buffer() { data_dynamic = std::vector<char>(1000); }

  int get_num_bytes() { return index; }

  template <typename T> void add(T element) {
    if (index + sizeof(T) >= data_dynamic.size()) {
      data_dynamic.resize(index + sizeof(T));
    }

    assert(sizeof(T) % sizeof(uint32_t) == 0 &&
           "type should be composed of uint32 members.");

    constexpr int NUM_MEMBERS = sizeof(T) / sizeof(uint32_t);

    uint32_t *int_elem = reinterpret_cast<uint32_t *>(&element);
    for (int i = 0; i < NUM_MEMBERS; i++) {
      uint32_t *mem_address = int_elem + i;
      *mem_address = htonl(*mem_address);
    }
    std::memcpy(data_dynamic.data() + index, int_elem, sizeof(T));
    index += sizeof(T);
  }
};
