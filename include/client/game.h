#pragma once
#include "../position.h"
#include "../window.h"
#include <array>
#include <concepts>
#include <ranges>
#include <vector>

class Game : public Window {
  std::vector<WallLayout> walls;
  void draw(const WallLayout &wall, int color);

public:
  Game(int width, int height);
  void remove(const TankLayout &tank);
  void draw(const std::array<TankLayout, 2> &tank);
  void remove(const WallLayout &wall);
  void remove(const Position &el);
  void draw(const Position &el);
  void draw(const WallLayout &wall);

  template <std::ranges::range T> void remove(const T &elements) {
    static_assert(!std::same_as<TankLayout, T>,
                  "TankLayout shouldn't be templated!");
    for (const auto &el : elements) {
      remove(el);
    }
  }

  template <typename T> void draw(const std::vector<T> &elements) {
    static_assert(!std::same_as<TankLayout, T>,
                  "TankLayout shouldn't be templated!");
    for (const auto &el : elements) {
      draw(el);
    }
  }
};
