#include "../include/non_player_elements.h"
#include "../include/game.h"
#include <ncurses.h>
#include <type_traits>

void Wall::draw(WINDOW *my_win) {
  if (direction == 'H')
    mvwhline(my_win, loc, start, '-', stop - start);
  if (direction == 'V')
    mvwvline(my_win, start, loc, '|', stop - start);
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

Bullet::Bullet(int x, int y, int vx, int vy)
    : Element(x, y, vx, vy), prev_loc({x, y}) {}

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
  if (active ) {
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
  for (int i = 0; i < tanks.size(); i++) {
    if (tanks[i].check_hit(x, y)) {
      tanks[!i].score += 1;
    }
  }
}

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
  player.custom_shot = [this, &player](Game *game, int x, int y, int vx,
                                       int vy) {
    custom_shot<ZapPixel>(game, x, y, vx, vy);
    player.custom_shot = [](Game *game, int sx, int sy, int vx, int vy) {
      game->spawn_bullet(sx, sy, vx, vy);
    };
    player.move = [&player](int ch, Game *game) {
      player.normal_move(ch, game);
    };
  };
  player.move = [this, &player](int ch, Game *game) {
    auto [dx, dy, vx, vy] = Tank::MOVE_Q[player.orientation];
    player.normal_move(ch, game);
    if (ch != player.shoot)
      custom_shot<ZapAimPixel>(game, player.x + dx, player.y + dy, vx, vy);
  };
  active = false;
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
  wattron(game->my_win, COLOR_PAIR(3));
  mvwaddch(game->my_win, y, x, '.');
  wattroff(game->my_win, COLOR_PAIR(3));
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
  auto &tanks = game->tanks;
  for (int i = 0; i < tanks.size(); i++) {
    if (tanks[i].check_hit(x, y)) {
      tanks[!i].score += 1;
    }
  }
}

void ZapAimPixel::hit(Game *game) {}
void ZapAimPixel::draw(Game *game) {
  wattron(game->my_win, COLOR_PAIR(7));
  mvwaddch(game->my_win, y, x, '.');
  wattroff(game->my_win, COLOR_PAIR(7));
}
void ZapAimPixel::move(Game *game) { active = false; }
