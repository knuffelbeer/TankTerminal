#pragma once
#include <iostream>
#include <ncurses.h>
#include <vector>

struct Wall {
  WINDOW *my_win;
  char direction;
  int loc;
  int start;
  int stop;
  void draw();
};

class Bullet {
public:
  int x;
  int y;
  int t{};
  WINDOW *win;
  Bullet(WINDOW *win, int x, int y, int vx, int vy);

  void move(const std::vector<Wall> &walls);
  void draw();

private:
  int vx;
  int vy;
};
