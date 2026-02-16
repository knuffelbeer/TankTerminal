#include "../../include/elements/rocket.h"
#include "../../include/game.h"
#include "../../include/tank.h"
#include <ncurses.h>

Rocket::Rocket(int x, int y, int vx, int vy, int player)
    : Element(x, y, vx, vy), player(player) {}
void Rocket::adjust_dir(Game *game, Tank &tank, int ch) {
  if (ch == tank.left) {
    if (vx < 1)
      vx += 1;
  } else if (ch == tank.right) {
    if (vx > -1)
      vx -= 1;
  } else if (ch == tank.up) {
    if (vy > -1)
      vy -= 1;
  } else if (ch == tank.down) {
    if (vy < 1)
      vy += 1;
  }
}

void Rocket::move(Game *game) {
  if (t == t_max) {
    auto &tank = game->tanks[player];
    tank.move = Tank::normal_move;
    tank.custom_shot = Tank::normal_shoot;
    active = false;
    return;
  }
  prev_loc = {x, y};
  step(game, x, y, vx, vy);
  t++;
}

void Rocket::hit(Game *game) {
  game->run = false;
  active = false;
}

void Rocket::draw(Game *game) {
  if (active) {
    auto [prev_x, prev_y] = prev_loc;
    mvwaddch(game->my_win, prev_y, prev_x, ' ');
    mvwaddch(game->my_win, y, x, '*');
  }
}

RocketSprite::RocketSprite(int x, int y) : Element(x, y) {}
void RocketSprite::hit(Game *game) {
  active = false;

  auto &tank = game->tanks[game->current_player];
  tank.custom_shot = [&tank](Game *game, int x, int y, int vx, int vy) {
    auto [x_displacement, y_distplacement, new_vx, new_vy] =
        Tank::MOVE_Q[tank.orientation];
    Rocket &rocket =
        game->spawn<Rocket>(x, y, new_vx, new_vy, game->current_player);
    tank.move = [&tank, &rocket](int ch, Game *game) {
      rocket.adjust_dir(game, tank, ch);
    };
  };
}

void RocketSprite::draw(Game *game) { mvwaddch(game->my_win, y, x, '>'); }
