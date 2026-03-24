#pragma once
#include "element.h"

class Game;
class MineSprite : public Element {
  bool visible = true;

public:
  int amt_spawned{};
  MineSprite(int x, int y);
  void draw(Game *game);
  void hit(Game *game) ;
	void move(Game *game){}
};

class Mine : public Element {
  int player;

public:
	Mine(int x, int y);
  void draw(Game *game) ;
  void hit(Game *game) ;
	void move(Game *game){}
};
