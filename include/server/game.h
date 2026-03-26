#pragma once
#include "../window.h"
#include "elements/bullet.h"
#include "elements/mine.h"
#include "elements/rocket.h"
#include "elements/zap.h"
#include "server.h"
#include "tank.h"
#include <cstdint>
#include <mutex>
#include <queue>
#include <unistd.h>
#include <utility>
#include <variant>
#include <vector>

class Game : public Window {
  void build();
  std::mutex mtx;
  void update_bullets();
  void draw_bullets();
  bool &ManageGame_run;
  std::queue<uint32_t> input;
  void iteration(Server *server);

public:
  std::vector<std::array<int, 2>> tank_positions;
  uint32_t border_height = 10;
  uint32_t level_width;
  uint32_t level_height;
  bool run = true;
  static constexpr int DELTA_MS = 100000;
  int current_player = 0;
  std::vector<Wall> walls;
  std::vector<Tank> tanks;
  std::vector<std::variant<ELEMENT_TYPES>> elements;
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
  void loop(Server *server);
  void send_data();
  void run_game();
};
