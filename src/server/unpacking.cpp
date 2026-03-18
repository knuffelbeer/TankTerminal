
#include <array>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <tuple>
#include <vector>

class Reader {
  int idx{};
  char *buffer;

public:
  Reader(char *buffer) : buffer(buffer) {}
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
};

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

struct T1 {
  int x;
  int y;
};
struct T2 {
  int x;
  int y;
  int z;
};
int main() {
  auto buf = Buffer<100>();
  for (int i{}; i < 5; i++)
    buf.add(T1{1, 2});

  for (int i{}; i < 4; i++)
    buf.add(T2{3, 2, 1});
  auto reader = Reader(buf.data);
  std::array<T1, 5> t1s = reader.read<T1, 5>();
  std::vector<T2> t2s = reader.read<T2>(4);
  for (auto t1 : t1s)
    std::cout << t1.x << ' ' << t1.y << '\n';
  for (auto t2 : t2s)
    std::cout << t2.x << ' ' << t2.y <<  ' ' << t2.z << '\n';
}
