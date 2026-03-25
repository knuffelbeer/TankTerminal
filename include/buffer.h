#pragma once
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <stdio.h>
#include <sys/_endian.h>

class Position;
class WallLayout;
class TankLayout;
class Message;
class Buffer {

  static constexpr size_t MAX_BYTES = 500;
  int max_bytes{MAX_BYTES};
  int index{};

public:
  Buffer() {}
  char data[MAX_BYTES];
  int get_num_bytes() { return index; }
  template <typename T> void add(T element) {
    assert(index + sizeof(T) <= max_bytes && "number of bytes exceeds buffer");
    assert(sizeof(T) % sizeof(uint32_t) == 0 &&
           "type should be composed of uint32 members.");

    constexpr int NUM_MEMBERS = sizeof(T) / sizeof(uint32_t);

    uint32_t *int_elem = reinterpret_cast<uint32_t *>(&element);
    for (int i = 0; i < NUM_MEMBERS; i++) {
      uint32_t *mem_address = int_elem + i;
      *mem_address = htonl(*mem_address);
    }

    std::memcpy(&data[index], int_elem, sizeof(T));
    index += sizeof(T);
  }
};
