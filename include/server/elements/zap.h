#pragma once
#include "element.h"
#include <ncurses.h>

class Game;

class ZapSprite : public Element {
  template <typename T>
  static void custom_shot(Game *game, uint32_t x, uint32_t y, int vx, int vy);

public:
  void move(Game *game);
  void draw(Game *game);
  void hit(Game *game);
  void cleanup(Game *game) override;
  bool fire = false;
  ZapSprite(uint32_t x, uint32_t y);
};

class ZapPixel : public Element {

public:
  constexpr static int t_max = 5;
  constexpr static int range = 40;
  ZapPixel(uint32_t x, uint32_t y);
  void move(Game *game);
  void draw(Game *game);
  void hit(Game *game);
};

class ZapAimPixel : public Element {
public:
  ZapAimPixel(uint32_t x, uint32_t y);
  constexpr static int range = 20;
  void hit(Game *game);
  void draw(Game *game);
  void move(Game *game);
};
