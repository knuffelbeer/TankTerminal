#include "../../include/server/managegame.h"
#include <ncurses.h>

void ManageGame::run() {
  bool run = true;
  int l{};
  auto g = Game(run, 40, 30);
  mvwprintw(g.my_win, g.level_height + 5, 0, "press x to quit game");
  nodelay(stdscr, TRUE);
  while (run) {
    clear();
    l = (l + 1) % 3;
    g.make_level(l);
    for (int i = 0; i < g.tanks.size(); i++) {
      wattron(g.my_win, COLOR_PAIR(g.tanks[i].color_pair));
      mvwprintw(g.my_win, g.level_height + 3, i * 10, "score: %i",
                g.tanks[i].score);
      wattroff(g.my_win, COLOR_PAIR(g.tanks[i].color_pair));
    }

    g.loop();
    usleep(999999);
    g.reset();
  }
}

ManageGame::ManageGame() {
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(BLACK_BLACK, COLOR_BLACK, COLOR_BLACK);
  init_pair(BLACK_RED, COLOR_BLACK, COLOR_RED);
  init_pair(BLACK_BLUE, COLOR_BLACK, COLOR_BLUE);
  init_pair(GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
  init_pair(GREEN_YELLOW, COLOR_GREEN, COLOR_YELLOW);
  init_pair(RED_BLACK, COLOR_RED, COLOR_BLACK);
  init_pair(BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);
  init_pair(YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
  init_pair(BLACK_WHITE, COLOR_BLACK, COLOR_WHITE);
  init_pair(WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
  init_pair(WHITE_WHITE, COLOR_WHITE, COLOR_WHITE);
  init_color(8, 255, 99, 0);
  init_pair(BLACK_ORANGE, COLOR_BLACK, 8);
}

ManageGame::~ManageGame() { endwin(); /* End curses mode		  */ }
