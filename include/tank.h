#pragma once
#include "bullet.h"
#include "ncurses.h"

class Game;

class Tank {
private:
  static constexpr int D_HORIZONTAL_RIGHT = 0;
  static constexpr int D_RIGHT_UP = 1;
  static constexpr int D_VERTICAL_UP = 2;
  static constexpr int D_LEFT_UP = 3;
  static constexpr int D_HORIZONTAL_LEFT = 4;
  static constexpr int D_LEFT_DOWN = 5;
  static constexpr int D_VERTICAL_DOWN = 6;
  static constexpr int D_RIGHT_DOWN = 7;
  int x{}, y{};
  int image{};
  int orientation{};
  WINDOW *my_win;
  int left, up, down, right, shoot;
  bool hit = false;
  int color_pair;

public:
  Tank(WINDOW *my_win, int left, int right, int up, int down, int shoot,
       int color_pair);
  Tank(WINDOW *my_win, int x, int y, int image, int left, int right, int up,
       int down, int shoot, int color_pair);
  void update(Game *game, int ch, bool &run);
  bool check_hit(const std::vector<Bullet> &bullets);
  void l();
  void h();
  void k();
  void j();

  template <typename F, typename... Args> void apply(F &&fun, Args &&...extra);
  void draw_single_point(int other_x, int other_y);
  void q(Game *game);
  void request_shot(Game *game, int x, int y, int vx, int vy);
  void draw();

  bool check_move(const std::vector<Wall> &walls);
  void straight_horizontal();
  void straight_vertical();
  void move(int ch, Game *game);
};
