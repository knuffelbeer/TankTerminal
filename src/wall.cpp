#include "../include/wall.h"
#include <ncurses.h>

void Wall::draw(WINDOW *my_win) {
  if (direction == 'H')
    mvwhline(my_win, loc, start, '-', stop - start);
  if (direction == 'V')
    mvwvline(my_win, start, loc, '|', stop - start);
}
