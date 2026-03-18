#pragma once
#include "element.h"

class Game;
class MineSprite : public Element {
  bool visible = true;

public:
  int amt_spawned{};
  MineSprite(int x, int y);
  void draw(Game *game) override;
  void hit(Game *game) override;
};

class Mine : public Element {
  int player;

public:
	Mine(int x, int y);
  void draw(Game *game) override;
  void hit(Game *game) override;
};
