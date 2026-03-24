#pragma once
#include "../position.h"
#include "../window.h"
#include <array>
#include <vector>

class Game : public Window {
  std::vector<WallLayout> walls;

public:
  Game(int width, int height);
  void remove(const Position &el);
	void remove(const TankLayout& tank);
	void remove(const WallLayout& wall);
  void draw(const Position &el) const;
  void draw(const std::array<TankLayout, 2> &tank);
  void draw(const WallLayout &wall) const;
};
