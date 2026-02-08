#pragma once
#include "ncurses.h"
class Window {
protected:
  WINDOW *my_win;
  int startx, starty, width, height;
  int ch;

private:
  WINDOW *create_newwin() {
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    keypad(local_win, TRUE);
    wrefresh(local_win);
    return local_win;
  }
  void destroy_win(WINDOW *local_win) {
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(local_win);
    delwin(local_win);
  }

public:
  Window(int width, int height) : width(width), height(height) {
    startx = 0;
    starty = 0;
    my_win = create_newwin();
    refresh();
  }
  void add_border() { box(my_win, '|', '-'); }
  void refresh() { wrefresh(my_win); }

  void display_text(const char *text, int x, int y) {
    mvwprintw(my_win, y, x, text);
  }
	void clear(){
					wclear(my_win);
	}
  void color_on(int num_pair) { wattron(my_win, COLOR_PAIR(num_pair)); }
  void color_off(int num_pair) { wattroff(my_win, COLOR_PAIR(num_pair)); }
  Window(int width, int height, int startx, int starty)
      : width(width), height(height), startx(startx), starty(starty) {
    my_win = create_newwin();
  }
  ~Window() { destroy_win(my_win); }
};
