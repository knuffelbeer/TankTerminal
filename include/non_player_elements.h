#pragma once
#include <iostream>
#include <ncurses.h>
#include <vector>

struct Wall {
  char direction;
  int start;
  int stop;
  int loc;
  WINDOW *my_win;
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
