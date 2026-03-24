#pragma once
#include "element.h"
#include <array>

class Game;
class Tank;

class Rocket : public Element {
  std::array<int, 2> prev_loc;
  int player;
  constexpr static int t_max = 40;

public:
  Rocket(int x, int y, int vx, int vy, int player);
  void move(Game *game);
  void hit(Game *game);
  void draw(Game *game);
  void adjust_dir(Game *game, Tank &tank, int ch);
};

class RocketSprite : public Element {
public:
  RocketSprite(int x, int y);
  void draw(Game *game);
  void hit(Game *game);
  void move(Game *game) {}
};
