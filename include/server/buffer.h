#pragma once
#include "../position.h"
#include <stdio.h>

template <size_t N> class Buffer {
  int max_bytes{N};
  int index{};

public:
  Buffer() {}
  char data[N];
  int get_num_bytes() { return index; }
  template <typename T> void add(T element) {
    if (index + sizeof(T) >= max_bytes) {
      printf("number of bytes exceeds buffer");
      throw;
    }
    memcpy(&data[index], &element, sizeof(T));
    index += sizeof(T);
  }
};
