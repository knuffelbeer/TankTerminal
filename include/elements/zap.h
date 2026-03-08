#pragma once

#include "../renderer.h"
#include "element.h"
#include <array>
#include <iostream>
#include <ncurses.h>
#include <vector>

class Game;

class ZapSprite : public Element {
  template <typename T>
  static void custom_shot(Game *game, int x, int y, int vx, int vy);

public:
  void move(Game *game) override;
  void draw(Game *game) override;
  void hit(Game *game) override;
  void cleanup(Game *game) override;
  bool fire = false;
  ZapSprite(int x, int y);
};

class ZapPixel : public Element {

public:
  constexpr static int t_max = 5;
  constexpr static int range = 40;
  ZapPixel(int x, int y);
  void move(Game *game) override;
  void draw(Game *game) override;
  void hit(Game *game) override;
};

class ZapAimPixel : public Element {
public:
  constexpr static int range = 20;
  ZapAimPixel(int x, int y) : Element(x, y) {};
  void hit(Game *game) override;
  void draw(Game *game) override;
  void move(Game *game) override;
};
