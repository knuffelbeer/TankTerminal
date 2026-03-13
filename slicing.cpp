#include <array>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

template <typename variant, size_t... i>
variant make_variant_by_index_impl(size_t idx, int x, int y,
                                   std::index_sequence<i...>) {
  using ctor = variant (*)(int, int);
  static constexpr ctor table[] = {
      [](int x, int y) { return variant(std::in_place_index<i>, x, y); }...};

  if (idx >= sizeof...(i))
    throw std::out_of_range("invalid variant index");

  return table[idx](x, y);
}

template <typename variant, size_t... i>
std::array<variant (*)(int, int), sizeof...(i)> generate_table(size_t idx, int x, int y,
                                                    std::index_sequence<i...>) {
  using ctor = variant (*)(int, int);
  static constexpr ctor table[] = {
      [](int x, int y) { return variant(std::in_place_index<i>, x, y); }...};

  if (idx >= sizeof...(i))
    throw std::out_of_range("invalid variant index");
  return table;
}

template <typename Variant>
Variant make_variant_by_index(size_t idx, int x, int y) {
  constexpr size_t j = std::variant_size_v<Variant>;
  return make_variant_by_index_impl<Variant>(
      idx, x, y, std::make_index_sequence<std::variant_size_v<Variant>>{});
}

enum { D1_TYPE, D2_TYPE };
struct Position {
  uint t;
  int x;
  int y;
};
struct Base {};

struct D1_server : public Position, public Base {
  D1_server(int x, int y) : Position{D1_TYPE, x, y} {}
  void print() const { std::cout << "D1_server " << x << ' ' << y << '\n'; }
};
struct D2_server : public Position, public Base {
  D2_server(int x, int y) : Position{D2_TYPE, x, y} {}
  void print() const { std::cout << "D2_server " << x << ' ' << y << '\n'; }
};
struct D1_client : public Position, public Base {
  D1_client(int x, int y) : Position{D1_TYPE, x, y} {}
  void print() const { std::cout << "D1_client " << x << ' ' << y << '\n'; }
  D1_client(Position pos) : Position{pos} {}
};
struct D2_client : public Position, public Base {
  D2_client(int x, int y) : Position{D2_TYPE, x, y} {}
  void print() const { std::cout << "D2_client " << x << ' ' << y << '\n'; }
  D2_client(Position pos) : Position{pos} {}
};

#define AMT_ELEMTS 2

int main(void) {
  std::vector<std::variant<D2_server, D1_server>> vec = {D1_server{1, 2},
                                                         D2_server{2, 3}};
  std::vector<Position> positions(AMT_ELEMTS);
  for (int i = 0; auto e : vec) {
    positions[i++] = std::visit([](auto obj) -> Position { return obj; }, e);
  }
  std::vector<std::variant<D1_client, D2_client>> elements;
  elements.reserve(2);

  for (const auto &e : positions) {
    elements.push_back(
        make_variant_by_index<std::variant<D1_client, D2_client>>(e.t, e.x,
                                                                  e.y));
  }
  for (const auto &e : vec) {
    std::visit([](const auto &obj) { obj.print(); }, e);
  }
  for (const auto &e : elements) {
    std::visit([](const auto &obj) { obj.print(); }, e);
  }
}
