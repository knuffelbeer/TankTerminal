#include "../include/non_player_elements.h"
#include "../include/game.h"
#include <ncurses.h>

void Wall::draw(WINDOW *my_win) {
  if (direction == 'H')
    mvwhline(my_win, loc, start, '-', stop - start);
  if (direction == 'V')
    mvwvline(my_win, start, loc, '|', stop - start);
}

Bullet::Bullet(int x, int y, int vx, int vy) : Element(x, y), vx(vx), vy(vy) {
  t_max = 10;
}

void Bullet::move(Game *game) {
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
  t++;
  if (t == t_max)
    active = false;
}

void Bullet::draw(Game *game) { mvwaddch(game->my_win, y, x, ACS_BLOCK); }
void Bullet::hit(Game *game) { game->run = false; }

Element::Element(int x, int y) : x(x), y(y) {}
Element::Element(int x, int y, int t_max) : x(x), y(y), t_max(t_max) {}

ZapSprite::ZapSprite(int x, int y) : Element(x, y) {}

void ZapSprite::hit(Game *game) {
  game->tanks[game->current_player].fire_element = 'V';
  active = false;
}

void ZapSprite::draw(Game *game) { mvwaddch(game->my_win, y, x, '+'); }
void ZapSprite::move(Game *game) {}

void ZapPixel::draw(Game *game) {
  wattron(game->my_win, COLOR_PAIR(3));
  mvwaddch(game->my_win, y, x, '.');
  wattroff(game->my_win, COLOR_PAIR(3));
}

ZapPixel::ZapPixel(int x, int y) : Element(x, y, 5) {}

void ZapPixel::move(Game *game) {
  t++;
  if (t == t_max)
    active = false;
}

void ZapPixel::hit(Game *game) { game->run = false; }
