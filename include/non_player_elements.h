#pragma once
#include <array>
#include <iostream>
#include <ncurses.h>
#include <vector>

struct Wall {
  char direction;
  int loc;
  int start;
  int stop;
  void draw(WINDOW *my_win);
};

/* for every element you can implement a move, draw and hit function.
 * the hit function decides what happens if the Element hits a Tank
 * move and draw get called on each iteration while the element is active.
 * */

class Game;
class Element {
public:
  int t{};
  bool active = true;
  int x;
  int y;
  bool is_hit = false;
  constexpr static int t_max = 30;
  virtual void move(Game *game) {}
  virtual void draw(Game *game) {}
  virtual void hit(Game *game) {}
  virtual void cleanup(Game *game);

protected:
  Element(int x, int y);
};

class ZapSprite : public Element {
  int vx;
  int vy;
  void move(Game *game) override;
  void draw(Game *game) override;
  void hit(Game *game) override;
  template <typename T>
  void custom_shot(Game *game, int x, int y, int vx, int vy);

public:
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

class Bullet : public Element {
public:
  Bullet(int x, int y, int vx, int vy);
  Bullet(int x, int y, int vx, int vy, int t_max);
  void move(Game *game) override;
  void draw(Game *game) override;
  void hit(Game *game) override;

private:
  std::array<int, 2> prev_loc;
  int vx;
  int vy;
};
