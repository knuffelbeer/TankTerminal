#pragma once
#include "elements/element.h"
#include "ncurses.h"
#include "wall.h"
#include <functional>
#include <map>
#include <memory>
class Game;

class Tank {

private:
  inline static constexpr int MOVE_K[8][2]{
      {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1},
  };
  inline static constexpr int MOVE_J[8][2] = {
      {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

  enum Direction {

    D_HORIZONTAL_RIGHT,
    D_RIGHT_UP,
    D_VERTICAL_UP,
    D_LEFT_UP,
    D_HORIZONTAL_LEFT,
    D_LEFT_DOWN,
    D_VERTICAL_DOWN,
    D_RIGHT_DOWN,

  };
  static const std::map<int, std::vector<std::pair<int, int>>> image_offsets;
  int image{};
  WINDOW *my_win;
  void draw_color(int color);
  void update_for_move(Game *game, void (Tank::*move)(),
                       void (Tank::*opposite)());

public:
  int left, up, down, right;
  int shoot;
  void reset();
  void setup();
  int color_pair;
  int score{};
  int x{}, y{};
  int orientation{};
  inline static constexpr int MOVE_Q[8][4] = {
      {3, 0, 1, 0},    {3, -2, 1, -1}, {0, -3, 0, -1}, {-3, -2, -1, -1},
      {-3, 0, -1, -0}, {-3, 2, -1, 1}, {0, 3, 0, 1},   {3, 3, 1, 1}};
  char fire_element = 'B';
  Tank(WINDOW *my_win, int left, int right, int up, int down, int shoot,
       int color_pair);
  Tank(WINDOW *my_win, int x, int y, int image, int left, int right, int up,
       int down, int shoot, int color_pair);
  void update(Game *game, int ch, bool &run);
  bool check_and_process_hit(Game *game);
  bool check_hit(int other_x, int other_y);
  void l();
  void h();
  void k();
  void j();

  std::function<void(Game *game, int x, int y, int dx, int dy)> custom_shot;
  template <typename F> void for_all_points(F &&fun);
  void draw_single_point(int other_x, int other_y);
  void q(Game *game);
  void draw();

  bool check_move(const std::vector<Wall> &walls);
  void straight_horizontal();
  void straight_vertical();
  std::function<void(int, Game *)> move;
  static std::function<void(Game *, int, int, int, int)> normal_shoot;
  static std::function<void(int, Game *)> normal_move;
};
