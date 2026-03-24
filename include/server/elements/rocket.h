#pragma once
#include "element.h"
#include <array>

class Game;
class Tank;

class Rocket : public Element {
  std::array<uint32_t, 2> prev_loc;
  int player;
  constexpr static int t_max = 40;

public:
  Rocket(uint32_t x, uint32_t y, int vx, int vy, int player);
  void move(Game *game);
  void hit(Game *game);
  void draw(Game *game);
  void adjust_dir(Game *game, Tank &tank, int ch);
};

class RocketSprite : public Element {
public:
  RocketSprite(uint32_t x, uint32_t y);
  void draw(Game *game);
  void hit(Game *game);
  void move(Game *game) {}
};
