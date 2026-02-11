#pragma once
#include "ncurses.h"
#include "non_player_elements.h"
#include <map>
#include <memory>
class Game;

class Tank {

private:

static const std::map<int, std::vector<std::pair<int,int>>> image_offsets ;
  inline static constexpr int MOVE_K[8][2]{
      {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1},
  };
  inline static constexpr int MOVE_Q[8][4] = {
      {3, 0, 1, 0},    {3, -2, 1, -1}, {0, -3, 0, -1}, {-3, -2, -1, -1},
      {-3, 0, -1, -0}, {-3, 2, -1, 1}, {0, 3, 0, 1},   {3, 3, 1, 1}};
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
  int x{}, y{};
  int image{};
  int orientation{};
  WINDOW *my_win;
  int left, up, down, right, shoot;
  bool hit = false;
  int color_pair;

public:
	char fire_element = 'B';
  Tank(WINDOW *my_win, int left, int right, int up, int down, int shoot,
       int color_pair);
  Tank(WINDOW *my_win, int x, int y, int image, int left, int right, int up,
       int down, int shoot, int color_pair);
  void update(Game *game, int ch, bool &run);
  bool check_hit(Game*game);
  void l();
  void h();
  void k();
  void j();

  template <typename F> void for_all_points(F &&fun);
  void draw_single_point(int other_x, int other_y);
  void q(Game *game);
  void request_shot(Game *game, int x, int y, int vx, int vy);
  void draw();

  bool check_move(const std::vector<Wall> &walls);
  void straight_horizontal();
  void straight_vertical();
  void move(int ch, Game *game);
};



