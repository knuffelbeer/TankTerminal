#include "../../include/elements/bullet.h"
#include "../../include/game.h"
#include <iostream>
#include <ncurses.h>
#include <utility>

std::array<int, 2> Bullet::counter{};
Bullet::Bullet(int x, int y, int vx, int vy, int player)
    : Element(x, y, vx, vy), prev_loc({x, y}), player(player) {
  personal_counter = &counter[player];
  (*personal_counter)++;
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
      if (tank.is_hit(x, y)) {
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

Bullet::~Bullet() {
  if (personal_counter == nullptr)
    return;
	(*personal_counter)--;
}
Bullet::Bullet(Bullet &&other)
    : Element(std::move(other)),
      personal_counter(std::exchange(other.personal_counter, nullptr)),
      player(other.player), prev_loc(other.prev_loc) {}
Bullet& Bullet::operator=(Bullet&& other) noexcept {
    if (this != &other) {

        // 1️⃣ Release any counter this object already owns
        if (personal_counter)
            (*personal_counter)--;

        // 2️⃣ Move the base class part
        Element::operator=(std::move(other));

        // 3️⃣ Take ownership of the counter from the other object
        personal_counter = std::exchange(other.personal_counter, nullptr);

        // 4️⃣ Move or copy other members
        player = other.player;
        prev_loc = other.prev_loc;
    }
    return *this;
}
