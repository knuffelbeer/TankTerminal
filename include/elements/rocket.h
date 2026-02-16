#pragma once
#include "../../include/elements/element.h"
#include <array>

class Game;
class Tank;

class Rocket : public Element {
  std::array<int, 2> prev_loc;
	int player;
	constexpr static int t_max  =  40;

public:
  Rocket(int x, int y, int vx, int vy, int player);
  void move(Game *game) override;
  void hit(Game *game) override;
  void draw(Game *game) override;
  void adjust_dir(Game *game, Tank &tank, int ch);
};

class RocketSprite : public Element {
public:
  RocketSprite(int x, int y);
  void draw(Game *game) override;
  void hit(Game *game) override;
};
