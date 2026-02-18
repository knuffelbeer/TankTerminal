#pragma once
#include "element.h"
#include <array>

class Bullet : public Element {
  int &counter;

public:
  Bullet(int x, int y, int vx, int vy, int &counter);
  void move(Game *game) override;
  void draw(Game *game) override;
  void hit(Game *game) override;
  ~Bullet();

private:
  std::array<int, 2> prev_loc;
};
