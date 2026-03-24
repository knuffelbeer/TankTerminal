#pragma once
#include "element.h"
#include <array>

class Bullet : public Element {
  int *personal_counter;

public:
  int player;
  static std::array<int, 2> counter;
  Bullet(int x, int y, int vx, int vy, int current_player);
  void move(Game *game);
  void draw(Game *game);
  void hit(Game *game);
  ~Bullet() override;
  Bullet(const Bullet &other) = delete;
  Bullet(Bullet &&other);
  Bullet &operator=(const Bullet &other) = delete;
  // Bullet &operator=(Bullet &&other) noexcept = default;

  Bullet &operator=(Bullet &&other) noexcept;

private:
  std::array<int, 2> prev_loc;
};
