#pragma once
#include "non_player_elements.h"
#include "tank.h"
#include "ncurses.h"
#include "window.h"
#include <iostream>
#include <unistd.h>
#include <vector>

class Game : public Window {
  void build();
  void update_bullets();
public:
  static constexpr int DELTA_MS = 100000;
  std::vector<Wall> walls;
  std::vector<Tank> tanks;
  std::vector<Bullet> bullets;
  using Window::Window;
  Game(int width, int height, int startx, int starty);
  Game(int width, int height);
	void make_level(int num_level);

  void spawn_bullet(int x, int y, int vx, int vy);
  void loop();
};
