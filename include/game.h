#pragma once
#include "ncurses.h"
#include "non_player_elements.h"
#include "tank.h"
#include "window.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <vector>

class Game : public Window {
  void build();
  void update_bullets();


public:
	bool run = true;
  static constexpr int DELTA_MS = 100000;
	int current_player = 0;
  std::vector<Wall> walls;
  std::vector<Tank> tanks;
  std::vector<Bullet> bullets;
  std::vector<std::unique_ptr<Element>> elements;
  using Window::Window;
  Game(int width, int height, int startx, int starty);
  Game(int width, int height);
  void make_level(int num_level);
  void spawn_bullet(int x, int y, int vx, int vy);

  template <typename T, typename... Args>
  void spawn(int x, int y, Args &&...args) {
    elements.push_back(std::make_unique<T>(x, y, std::forward<Args>(args)...));
  }
  void loop();
};
