#pragma once
#include "elements/element.h"
#include "renderer.h"
#include "tank_constants.h"
#include "wall.h"
#include <functional>

class Game;
class Tank {
private:
  inline static constexpr const auto &MOVE_K =
      TankConstants::Move::K;

  inline static constexpr const auto &MOVE_J =
      TankConstants::Move::J;

  inline static constexpr const auto &image_offsets =
      TankConstants::IMAGE_OFFSETS;

  int image{};
  WINDOW *my_win;
  void draw_color(int color);
  void update_for_move(Game *game, void (Tank::*move)(),
                       void (Tank::*opposite)());

public:
  bool exploded{};
  // int counter{};
  int left, up, down, right;
  int shoot_button;
  void reset();
  void setup();
  int color_pair;
  int score{};
  int x{}, y{};
  int orientation{};

  inline static constexpr const auto& MOVE_Q =
      TankConstants::Move::Q;

  char fire_element = 'B';
  Tank(WINDOW *my_win, int left, int right, int up, int down, int shoot,
       int color_pair);
  Tank(WINDOW *my_win, int x, int y, int image, int left, int right, int up,
       int down, int shoot, int color_pair);
  void update(Game *game, int ch, bool &run);
  bool check_and_process_hit(Game *game);
  bool is_hit(int other_x, int other_y);
  void l();
  void h();
  void k();
  void j();

  std::function<void(Game *game, int x, int y, int dx, int dy)> custom_shot;
  template <typename F> void for_all_points(F &&fun);
  void draw_single_point(int other_x, int other_y);
  void q(Game *game);
  void draw();

  void animation();
  bool check_move(const std::vector<Wall> &walls);
  void straight_horizontal();
  void straight_vertical();
  std::function<void(int, Game *)> move;
  static std::function<void(Game *, int, int, int, int)> normal_shoot;
  static std::function<void(int, Game *)> normal_move;
};
