#pragma once
#include "renderer.h"

struct Wall {
  char direction;
  int loc;
  int start;
  int stop;
  void draw(WINDOW *my_win);
};
