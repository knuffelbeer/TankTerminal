#pragma once
#include "ncurses.h"

class Window {
protected:
  WINDOW *my_win;
  int startx, starty, width, height;
  int ch;

private:
  WINDOW *create_newwin();
  void destroy_win(WINDOW *local_win);

public:
  Window(int width, int height);
  Window(int width, int height, int startx, int starty);
  void add_border();
  void refresh();

  void display_text(const char *text, int x, int y);
  void clear();
  void color_on(int num_pair);
  void color_off(int num_pair);
  ~Window();
};
