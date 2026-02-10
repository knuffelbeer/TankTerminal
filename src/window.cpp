#include "../include/window.h"

Window::Window(int width, int height, int startx, int starty)
    : width(width), height(height), startx(startx), starty(starty) {
  my_win = create_newwin();
}

Window::Window(int width, int height) : width(width), height(height) {
  startx = 0;
  starty = 0;
  my_win = create_newwin();
  refresh();
}

WINDOW *Window::create_newwin() {
  WINDOW *local_win;
  local_win = newwin(height, width, starty, startx);
  keypad(local_win, TRUE);
  wrefresh(local_win);
  return local_win;
}

void Window::destroy_win(WINDOW *local_win) {
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(local_win);
  delwin(local_win);
}

void Window::add_border() { box(my_win, '|', '-'); }

void Window::refresh() { wrefresh(my_win); }

void Window::display_text(const char *text, int x, int y) {
  mvwprintw(my_win, y, x, text);
}
void Window::clear() { wclear(my_win); }
void Window::color_on(int num_pair) { wattron(my_win, COLOR_PAIR(num_pair)); }
void Window::color_off(int num_pair) { wattroff(my_win, COLOR_PAIR(num_pair)); }
Window::~Window() { destroy_win(my_win); }
