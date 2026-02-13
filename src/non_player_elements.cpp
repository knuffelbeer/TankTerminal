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

Bullet::Bullet(int x, int y, int vx, int vy) : Element(x, y), vx(vx), vy(vy) {
}

void Bullet::move(Game *game) {
  wattron(game->my_win, 0);
  mvwaddch(game->my_win, y, x, ' ');
  wattroff(game->my_win, 0);
  for (auto w : game->walls) {
    if (w.direction == 'H') {
      if (vy == 0 && w.loc == y && (w.start == x || w.stop == x)) {
        vx = -vx;
      }

      if (y == w.loc && w.start <= x && x < w.stop) {
        vy = -vy;
      }
    }
    if (w.direction == 'V') {
      if (vx == 0 && w.loc == x && (w.start == y || w.stop == y)) {
        vy = -vy;
      }
      if (x == w.loc && w.start <= y && y < w.stop) {
        vx = -vx;
      }
    }
  }
  x += vx;
  y += vy;
  t++;
  if (t == t_max)
    active = false;
}

void Bullet::draw(Game *game) { mvwaddch(game->my_win, y, x, ACS_BLOCK); }
void Bullet::hit(Game *game) {
  game->run = false;
  auto &tanks = game->tanks;
  for (int i = 0; i < tanks.size(); i++) {
    if (tanks[i].check_hit(x, y)) {
      tanks[!i].score += 1;
    }
  }
}

void Element::cleanup(Game *game) {
  wattron(game->my_win, 0);
  mvwaddch(game->my_win, y, x, ' ');
  wattroff(game->my_win, 0);
}

Element::Element(int x, int y) : x(x), y(y) {}

ZapSprite::ZapSprite(int x, int y) : Element(x, y) {}

template <typename T>
void ZapSprite::custom_shot(Game *game, int x, int y, int vx, int vy) {
  for (int i = 0; i < T::range; i++) {
    for (auto w : game->walls) {
      if (w.direction == 'H') {
        if (vy == 0 && w.loc == y && (w.start == x || w.stop == x)) {
          vx = -vx;
        }

        if (y == w.loc && w.start <= x && x <= w.stop) {
          vy = -vy;
        }
      }
      if (w.direction == 'V') {
        if (vx == 0 && w.loc == x && (w.start == y || w.stop == y)) {
          vy = -vy;
        }
        if (x == w.loc && w.start <= y && y <= w.stop) {
          vx = -vx;
        }
      }
    }
    x += vx;
    y += vy;
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
  game->tanks[game->current_player].custom_shot =
      [this](Game *game, int x, int y, int vx, int vy) {
        custom_shot<ZapPixel>(game, x, y, vx, vy);
        auto &tank = game->tanks[game->current_player];

        tank.custom_shot = [](Game *game, int sx, int sy, int vx, int vy) {
          game->spawn_bullet(sx, sy, vx, vy);
        };
        tank.move = [](int ch, Game *game) {
          game->tanks[game->current_player].normal_move(ch, game);
        };
      };
  game->tanks[game->current_player].move = [this](int ch, Game *game) {
    auto &tank = game->tanks[game->current_player];
    auto [dx, dy, vx, vy] = Tank::MOVE_Q[tank.orientation];
    tank.normal_move(ch, game);
    custom_shot<ZapAimPixel>(game, tank.x + dx, tank.y + dy, vx, vy);
    // tank.draw();
  };
  active = false;
}

void ZapSprite::draw(Game *game) { mvwaddch(game->my_win, y, x, '+'); }

void ZapSprite::move(Game *game) {}

void ZapPixel::draw(Game *game) {
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

void ZapPixel::hit(Game *game) { game->run = false;

  game->run = false;
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
