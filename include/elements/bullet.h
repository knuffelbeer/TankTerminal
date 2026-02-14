#pragma once
#include <array>
#include "element.h"

class Bullet : public Element {
public:
  Bullet(int x, int y, int vx, int vy);
  Bullet(int x, int y, int vx, int vy, int t_max);
  void move(Game *game) override;
  void draw(Game *game) override;
  void hit(Game *game) override;

private:
  std::array<int, 2> prev_loc;
};
