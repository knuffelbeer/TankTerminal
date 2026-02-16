#include "../../include/elements/mine.h"
#include "../../include/game.h"
#include <ncurses.h>
// #include <ncurses.h>

Mine::Mine(int x, int y) : Element(x, y) {}
MineSprite::MineSprite(int x, int y) : Element(x, y) {}

void Mine::hit(Game *game) {
  game->run = false;
  declare_winner(game);
}

void Mine::draw(Game *game) {
  if (t < 5) {
    t++;
    mvwaddch(game->my_win, y, x, '=');
  }
  if (t == 5)
    mvwaddch(game->my_win, y, x, ' ');
}

void MineSprite::hit(Game *game) {
  visible = false;
  int player = game->current_player;
  auto &tank = game->tanks[player];
  tank.move = Tank::normal_move;
  tank.custom_shot = [&tank, player, this](Game *game, int sx, int sy, int vx,
                                           int vy) {
    if (this->amt_spawned == 3) {
      this->active = false;
      game->tanks[player].custom_shot = Tank::normal_shoot;
      game->tanks[player].q(game);
      return;
    }
    this->amt_spawned++;
    auto [dx, dy, _vx, _vy] = Tank::MOVE_Q[tank.orientation];
    game->spawn<Mine>(tank.x + dx, tank.y + dy);
  };
}

void MineSprite::draw(Game *game) {
  if (visible)
    mvwaddch(game->my_win, y, x, '=');
}
