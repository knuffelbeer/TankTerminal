#pragma once
#include "../renderer.h"
#include "../position.h"

struct Wall : public WallLayout {
  void draw(WINDOW *my_win);
};
