#pragma once
#include "element.h"

class Game;
class MineSprite : public Element {

public:
  bool visible = true;
  int amt_spawned{};
  MineSprite(int x, int y);
  void draw(Game *game);
  void hit(Game *game);
  void move(Game *game) {}
};

class Mine : public Element {
  int player;

public:
  bool visible = true;
  Mine(int x, int y);
  void draw(Game *game);
  void hit(Game *game);
  void move(Game *game) {}
};
