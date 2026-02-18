#include "../../include/elements/zap.h"
#include "../../include/game.h"

/*
 * The ZapSprite activates the special. the ZapPixel's get shot when firing the
 * special, and the ZapAimPixel's get displaid when aiming.
 * they all inherit from Element, and thus can be updated by the game loop.
 * */

ZapSprite::ZapSprite(int x, int y) : Element(x, y) {}

template <typename T>
void ZapSprite::custom_shot(Game *game, int x, int y, int vx, int vy) {
  for (int i = 0; i < T::range; i++) {
    step(game, x, y, vx, vy);
    for (auto &t : game->tanks) {
      if (t.check_hit(x, y)) {
        if constexpr (std::is_same_v<T, ZapPixel>) {
          game->spawn<T>(x, y);
        }
        return;
      }
    }
    game->spawn<T>(x, y);
  }
}

void ZapSprite::hit(Game *game) {
  active = false;
  auto &player = game->tanks[game->current_player];
  player.custom_shot = [&player](Game *game, int x, int y, int vx, int vy) {
    ZapSprite::custom_shot<ZapPixel>(game, x, y, vx, vy);
    player.move = Tank::normal_move;
    player.custom_shot = Tank::normal_shoot;
  };
  player.move = [&player](int ch, Game *game) {
    auto [dx, dy, vx, vy] = Tank::MOVE_Q[player.orientation];
    player.normal_move(ch, game);
    if (ch != player.shoot_button)
      ZapSprite::custom_shot<ZapAimPixel>(game, player.x + dx, player.y + dy,
                                          vx, vy);
  };
}

void ZapSprite::draw(Game *game) {
  if (active)
    mvwaddch(game->my_win, y, x, '+');
}

void ZapSprite::cleanup(Game *game) {} // no cleanup, since it's always
                                       // overlapped by a tank.
void ZapSprite::move(Game *game) {}

void ZapPixel::draw(Game *game) {
  if (is_hit)
    return;
  wattron(game->my_win, COLOR_PAIR(GREEN_BLACK));
  mvwaddch(game->my_win, y, x, '.');
  wattroff(game->my_win, COLOR_PAIR(GREEN_BLACK));
}

ZapPixel::ZapPixel(int x, int y) : Element(x, y) {}

void ZapPixel::move(Game *game) {
  t++;
  if (t == t_max)
    active = false;
}

void ZapPixel::hit(Game *game) {
  game->run = false;
  is_hit = true;
  declare_winner(game);
}

void ZapAimPixel::hit(Game *game) {}

void ZapAimPixel::draw(Game *game) {
  wattron(game->my_win, COLOR_PAIR(YELLOW_BLACK));
  mvwaddch(game->my_win, y, x, '.');
  wattroff(game->my_win, COLOR_PAIR(YELLOW_BLACK));
}

void ZapAimPixel::move(Game *game) { active = false; }
