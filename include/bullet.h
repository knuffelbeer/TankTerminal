#pragma once
#include "ncurses.h"
#include <iostream>
#include <vector>

struct Wall {
  char direction;
  int start;
  int stop;
  int loc;
  WINDOW *my_win;
  void draw() {
    if (direction == 'H')
      mvwhline(my_win, loc, start, '-', stop - start);
    if (direction == 'V')
      mvwvline(my_win, start, loc, '|', stop - start);
  }
};

class Bullet {
public:
  int x;
  int y;
  int t{};
  WINDOW *win;
  Bullet(WINDOW *win, int x, int y, int vx, int vy)
      : win(win), x(x), y(y), vx(vx), vy(vy) {}

  void move(const std::vector<Wall> &walls) {
    for (auto w : walls) {
      if (w.direction == 'H') {
        if (vy == 0 && w.loc == y && (w.start == x || w.stop == x)) {
          vx = -vx;
        }

        if (y == w.loc && w.start <= x && x <= w.stop) {
          vy = -vy;
        }
      }
      if (w.direction == 'V') {
        if (vx == 0 && w.loc == x && (w.start == y || w.stop == y)) {
          vy = -vy;
        }
        if (x == w.loc && w.start <= y && y <= w.stop) {
          vx = -vx;
        }
      }
    }
    x += vx;
    y += vy;
    t += 1;
  }
  void draw() { mvwaddch(win, y, x, ACS_BULLET); }

private:
  int vx;
  int vy;
};
