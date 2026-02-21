
#include "../../include/elements/bullet.h"
#include "../../include/game.h"
#include <iostream>
#include <ncurses.h>

Bullet::Bullet(int x, int y, int vx, int vy, int &counter)
    : Element(x, y, vx, vy), prev_loc({x, y}), counter(counter) {
  counter++;
}

void Bullet::move(Game *game) {
  if (is_hit)
    return;
  if (t == t_max) {
    mvwaddch(game->my_win, y, x, ' ');
    active = false;
    return;
  }
  prev_loc = {x, y};
  step(game, x, y, vx, vy);
  t++;
}

void Bullet::draw(Game *game) {
  if (active) {
    auto [prev_x, prev_y] = prev_loc;
    mvwaddch(game->my_win, prev_y, prev_x, ' ');
    for (auto &tank : game->tanks) {
      if (tank.check_hit(x, y)) {
        return;
      }
    }
    mvwaddch(game->my_win, y, x, ACS_BLOCK);
  }
}

void Bullet::hit(Game *game) {
  is_hit = true;
  active = false;
  game->run = false;
  auto &tanks = game->tanks;
	declare_winner(game);
}

Bullet::~Bullet() { counter--; }
