#include "../../../include/server/elements/element.h"
#include "../../../include/server/game.h"

Element::Element(int x, int y, int type_name) : Position{x, y, type_name} {}
Element::Element(int x, int y, int type_name, int vx, int vy)
    : Position{x, y, type_name}, vx(vx), vy(vy) {}

void Element::declare_winner(Game *game) {
  auto &tanks = game->tanks;
  for (int i = 0; i < tanks.size(); i++) {
    if (tanks[i].is_hit(x, y)) {
      tanks[!i].score += 1;
      tanks[i].exploded = true;
    }
  }
}

void Element::cleanup(Game *game) {
  for (auto &tank : game->tanks) {
    if (tank.is_hit(x, y))
      return;
  }
  wattron(game->my_win, BLACK_BLACK);
  mvwaddch(game->my_win, y, x, ' ');
  wattroff(game->my_win, BLACK_BLACK);
}

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
