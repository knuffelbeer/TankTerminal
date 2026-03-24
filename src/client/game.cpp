#include "../../include/client/game.h"
#include "../../include/tank_constants.h"
#include <array>
#include <ncurses.h>

Game::Game(int width, int height) : Window(width, height) {}

void Game::remove(const Position &el) { mvwaddch(my_win, el.y, el.x, ' '); }

void Game::remove(const TankLayout &tank) {
  for (const auto [dx, dy] : TankConstants::IMAGE_OFFSETS[tank.orientation]) {
    mvwaddch(my_win, tank.y + dy, tank.x + dx, ' ');
  }
}
void Game::remove(const WallLayout &wall) {
  wattron(my_win, COLOR_PAIR(WHITE_BLACK));
  if (wall.direction == 'H')
    mvwhline(my_win, wall.loc, wall.start, ' ', wall.stop - wall.start);
  if (wall.direction == 'V')
    mvwvline(my_win, wall.start, wall.loc, ' ', wall.stop - wall.start);
  wattroff(my_win, COLOR_PAIR(WHITE_BLACK));
}

void Game::draw(const WallLayout &wall) const {
  wattron(my_win, COLOR_PAIR(WHITE_BLACK));
  if (wall.direction == 'H')
    mvwhline(my_win, wall.loc, wall.start, '-', wall.stop - wall.start);
  if (wall.direction == 'V')
    mvwvline(my_win, wall.start, wall.loc, '|', wall.stop - wall.start);
  wattroff(my_win, COLOR_PAIR(WHITE_BLACK));
}

void Game::draw(const Position &el) const {
  switch (el.type_idx) {
  case BULLET_TYPE:
    mvwaddch(my_win, el.y, el.x, ACS_BLOCK);
    break;
  }
}

void Game::draw(const std::array<TankLayout, 2> &tanks) {
  for (int color = BLACK_RED; const auto &tank : tanks) {
    wattron(my_win, COLOR_PAIR(color));
    for (const auto [dx, dy] : TankConstants::IMAGE_OFFSETS[tank.orientation]) {
      mvwaddch(my_win, tank.y + dy, tank.x + dx, ' ');
    }
    wattroff(my_win, COLOR_PAIR(color));
    color = BLACK_BLUE;
  }
}
