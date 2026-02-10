#include "../include/non_player_elements.h"

void Wall::draw(WINDOW* my_win) {
  if (direction == 'H')
    mvwhline(my_win, loc, start, '-', stop - start);
  if (direction == 'V')
    mvwvline(my_win, start, loc, '|', stop - start);
}

Bullet::Bullet(WINDOW *win, int x, int y, int vx, int vy)
    : win(win), x(x), y(y), vx(vx), vy(vy) {}

void Bullet::move(const std::vector<Wall> &walls) {
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
void Bullet::draw() { mvwaddch(win, y, x, ACS_BLOCK); }
