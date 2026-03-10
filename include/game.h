#pragma once
#include "../include/elements/bullet.h"
#include "../include/elements/mine.h"
#include "../include/elements/rocket.h"
#include "../include/elements/zap.h"
#include "tank.h"
#include "window.h"
#include <unistd.h>
#include <utility>
#include <variant>
#include <vector>

class Game : public Window {
  void build();
  void update_bullets();
  void draw_bullets();
  bool &ManageGame_run;

public:
  std::vector<std::array<int, 2>> tank_positions;
  int border_height = 10;
  int level_width;
  int level_height;
  bool run = true;
  static constexpr int DELTA_MS = 100000;
  int current_player = 0;
  std::vector<Wall> walls;
  std::vector<Tank> tanks;
  using Var = std::variant<Bullet, Rocket, RocketSprite, ZapPixel, ZapAimPixel,
                           ZapSprite, MineSprite, Mine>;
  std::vector<Var> elements;
  Game(bool &ManageGame_run, int width, int height, int startx, int starty);
  Game(bool &ManageGamerun, int width, int height);
  void make_level(int num_level);
  void spawn_bullet(int x, int y, int vx, int vy);

  template <typename T, typename... Args>
  T &spawn(int x, int y, Args &&...args) {
    elements.emplace_back(std::in_place_type<T>, x, y,
                          std::forward<Args>(args)...);
    return std::get<T>(elements.back());
  }
  void reset();
  void loop();
};
