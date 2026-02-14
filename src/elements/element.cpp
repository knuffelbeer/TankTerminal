#include "../../include/game.h"
#include "../../include/elements/element.h"

void Element::cleanup(Game *game) {
  for (auto &tank : game->tanks) {
    if (tank.check_hit(x, y))
      return;
  }
  wattron(game->my_win, 0);
  mvwaddch(game->my_win, y, x, ' ');
  wattroff(game->my_win, 0);
}

Element::Element(int x, int y) : x(x), y(y) {}
Element::Element(int x, int y, int vx, int vy) : x(x), y(y), vx(vx), vy(vy) {}
void Element::step(Game *game, int &x, int &y, int &vx, int &vy) {
  bool flipped_x{}, flipped_y{};
  for (auto w : game->walls) {
    if (w.direction == 'H') {
      if (!flipped_x && vy == 0 && w.loc == y &&
          (w.start == x || w.stop - 2 == x)) {
        flipped_x = true;
        vx = -vx;
      }

      if (!flipped_y && y == w.loc && w.start <= x && x < w.stop) {
        vy = -vy;
        flipped_y = true;
      }
    }
    if (w.direction == 'V') {
      if (!flipped_y && vx == 0 && w.loc == x &&
          (w.start == y || w.stop - 2 == y)) {
        vy = -vy;
        flipped_y = true;
      }
      if (!flipped_x && x == w.loc && w.start <= y && y < w.stop) {
        flipped_x = true;
        vx = -vx;
      }
    }
  }
  x = x + vx;
  y = y + vy;
}
