#pragma once
#include "../../position.h"

class Game;
class Element : public Position {
public:
  int t{};
  bool active = true;
  int vx{};
  int vy{};
  bool is_hit = false;
  constexpr static int t_max = 30;
  virtual void cleanup(Game *game);
  virtual ~Element() = default;

  Element(const Element &) = delete;
  Element &operator=(const Element &) = delete;

  Element(Element &&) noexcept = default;
  Element &operator=(Element &&) noexcept = default;

protected:
  Element(uint32_t x, uint32_t y, uint32_t type_name);
  Element(uint32_t x, uint32_t y, uint32_t type_name, int vx, int vy);

  void declare_winner(Game *game);
  static void step(Game *game, uint32_t &x, uint32_t &y, int &vx, int &vy);
};
