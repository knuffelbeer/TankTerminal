#include "../../../include/server/elements/mine.h"
#include "../../../include/server/game.h"
#include "../../../include/renderer.h"

Mine::Mine(int x, int y) : Element(x, y, MINE_TYPE) {}
MineSprite::MineSprite(int x, int y) : Element(x, y, MINE_SPRITE_TYPE) {}

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
  tank.custom_shot = [player, this](Game *game, int sx, int sy, int vx,
                                    int vy) {
    auto &tank = game->tanks[player];
    if (this->amt_spawned == 3) {
      this->active = false;
      tank.custom_shot = Tank::normal_shoot;
      tank.q(game);
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
