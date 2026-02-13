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
	bool &ManageGame_run;

public:
	int border_height = 10;
	int level_width;
	int level_height;
  bool run = true;
  static constexpr int DELTA_MS = 100000;
  int current_player = 0;
  std::vector<Wall> walls;
  std::vector<Tank> tanks;
  std::vector<Bullet> bullets;
  std::vector<std::unique_ptr<Element>> elements;
  using Window::Window;
  Game(bool &ManageGame_run,int width, int height, int startx, int starty);
  Game(bool &ManageGamerun,int width, int height);
  void make_level(int num_level);
  void spawn_bullet(int x, int y, int vx, int vy);

  template <typename T, typename... Args>
  void spawn(int x, int y, Args &&...args) {
    auto temp = std::make_unique<T>(x, y, std::forward<Args>(args)...);
    temp->draw(this);
    elements.push_back(std::move(temp));
  }
	void reset();
  void loop();
};
