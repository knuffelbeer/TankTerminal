#include "../../include/client/game.h"
#include "../../include/renderer.h"
#include "../../include/tank_constants.h"
#include <array>
#include <ncurses.h>
#include <unistd.h>

Game::Game() : Window(0, 0) {}
Game::Game(int width, int height) : Window(width, height) {}

void Game::remove(const WallLayout &wall) {
  wattron(my_win, COLOR_PAIR(BLACK_BLACK));
  if (wall.direction == 'H')
    mvwhline(my_win, wall.loc, wall.start, ' ', wall.stop - wall.start);
  if (wall.direction == 'V')
    mvwvline(my_win, wall.start, wall.loc, ' ', wall.stop - wall.start);
  wattroff(my_win, COLOR_PAIR(BLACK_BLACK));
}

void Game::draw(const WallLayout &wall) { draw(wall, WHITE_BLACK); }

void Game::draw(const WallLayout &wall, int color) {
  wattron(my_win, COLOR_PAIR(color));
  if (wall.direction == 'H')
    mvwhline(my_win, wall.loc, wall.start, '-', wall.stop - wall.start);
  if (wall.direction == 'V')
    mvwvline(my_win, wall.start, wall.loc, '|', wall.stop - wall.start);
  wattroff(my_win, COLOR_PAIR(color));
}

void Game::draw(const AnimationPos &pos,
                const std::array<TankLayout, 2> &tanks) {
  wattron(my_win, COLOR_PAIR(BLACK_BLACK));
  const TankLayout &tank = tanks[pos.player];
  for (const auto &[dx, dy] : TankConstants::IMAGE_OFFSETS[tank.orientation]) {
    mvwaddch(my_win, tank.y + dy, tank.x + dx, ' ');
  }
  wattroff(my_win, COLOR_PAIR(BLACK_BLACK));

  constexpr int amt_explosion_colors = 3;
  int colors[amt_explosion_colors];
  colors[0] = tank.color_pair;
  colors[1] = GREEN_YELLOW;
  colors[2] = BLACK_ORANGE;

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < amt_explosion_colors; j++) {

      int radius = i - j;

      if (radius < 0)
        continue;

      wattron(my_win, COLOR_PAIR(colors[j]));

      for (const auto &[dx, dy] :
           TankConstants::IMAGE_OFFSETS[tank.orientation]) {
        mvwaddch(my_win, tank.y + radius * dy - radius / 2,
                 tank.x + radius * dx - radius / 2, ' ');
      }
      wattroff(my_win, COLOR_PAIR(colors[j]));
    }
    wrefresh(my_win);
    usleep(120000);
  }
  wclear(my_win);
}

void Game::remove(const Position &el) { mvwaddch(my_win, el.y, el.x, ' '); }

void Game::draw(const Position &el) {
  switch (el.type_idx) {
  case BULLET_TYPE:
    wattron(my_win, COLOR_PAIR(WHITE_BLACK));
    mvwaddch(my_win, el.y, el.x, ACS_BLOCK);
    wattroff(my_win, COLOR_PAIR(WHITE_BLACK));
    break;
  case MINE_SPRITE_TYPE:
    mvwaddch(my_win, el.y, el.x, 'M');
  case MINE_TYPE:
    mvwaddch(my_win, el.y, el.x, '+');
  case ZAP_SPRITE_TYPE:
    mvwaddch(my_win, el.y, el.x, 'Z');
    break;
  case ZAP_AIM_PIXEL_TYPE:
    wattron(my_win, COLOR_PAIR(YELLOW_BLACK));
    mvwaddch(my_win, el.y, el.x, '.');
    wattroff(my_win, COLOR_PAIR(YELLOW_BLACK));
    break;
  case ZAP_PIXEL_TYPE:
    wattron(my_win, COLOR_PAIR(GREEN_BLACK));
    mvwaddch(my_win, el.y, el.x, '.');
    wattroff(my_win, COLOR_PAIR(GREEN_BLACK));
    break;
  case ROCKET_SPRITE_TYPE:
    mvwaddch(my_win, el.y, el.x, 'R');
    break;
  case ROCKET_TYPE:
    mvwaddch(my_win, el.y, el.x, '*');
    break;

  default:
    wprintw(my_win, "type: %i not found!", el.type_idx);
  }
}

void Game::remove(const TankLayout &tank) {
  for (const auto &[dx, dy] : TankConstants::IMAGE_OFFSETS[tank.orientation]) {
    mvwaddch(my_win, tank.y + dy, tank.x + dx, ' ');
  }
}

void Game::draw(const std::array<TankLayout, 2> &tanks) {
  for (int color = BLACK_RED; const auto &tank : tanks) {
    wattron(my_win, COLOR_PAIR(color));
    for (const auto &[dx, dy] :
         TankConstants::IMAGE_OFFSETS[tank.orientation]) {
      mvwaddch(my_win, tank.y + dy, tank.x + dx, ' ');
    }
    wattroff(my_win, COLOR_PAIR(color));
    color = BLACK_BLUE;
  }
}
