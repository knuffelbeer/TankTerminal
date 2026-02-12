#include "../include/managegame.h"
#include <ncurses.h>

void ManageGame::run() {
  {
    nodelay(stdscr, FALSE);
    refresh();
    auto welcome = Window(30, 20);
    bool q = false;
    bool space = false;
    int ch = 0;
    while (!(q && space)) {
      if (ch == 'q')
        q = true;
      if (ch == ' ')
        space = true;
      welcome.clear();
      welcome.color_on(3);
      welcome.display_text("Welcome to TankTerminal", 4, 4);
      welcome.color_off(3);
      welcome.add_border();
      welcome.color_on(5);
      welcome.display_text("Player One", 4, 7);
      if (!q) {
        welcome.display_text("press q", 4, 11);
        welcome.color_off(5);
      }
      welcome.display_text("q  w", 5, 8);
      welcome.display_text("asd", 7, 9);
      if (q)
        welcome.color_off(5);

      welcome.color_on(6);
      welcome.display_text("Player two", 15, 7);
      if (!space) {
        welcome.display_text("press SPACE", 15, 11);
        welcome.color_off(6);
      }

      welcome.display_text("SPACE  ^", 16, 8);
      welcome.display_text("< v >", 21, 9);
      if (space)
        welcome.color_off(5);
      welcome.refresh();
      if (!(q && space))
        ch = getch();
    }
    usleep(999999);
  }
  bool run = true;
  int l{};
  while (run) {
    clear();
    nodelay(stdscr, TRUE);
    auto g = Game(40, 20);
    g.make_level(l);
    l = (l + 1) % 3;
    g.loop();
    nodelay(stdscr, FALSE);
    attron(COLOR_PAIR(3));
    mvprintw(5, 15, "Game over! ");
    mvprintw(8, 10, "[r]estart      [q]uit game");
    attroff(COLOR_PAIR(3));
    while (true) {
      int ch = getch();
      if (ch == 'q') {
        run = false;
        break;
      }
      if (ch == 'r') {
        break;
      }
    }
  }
}

ManageGame::ManageGame() {
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(0, COLOR_BLACK, COLOR_BLACK);
  init_pair(1, COLOR_BLACK, COLOR_RED);
  init_pair(2, COLOR_BLACK, COLOR_BLUE);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);
  init_pair(4, COLOR_GREEN, COLOR_YELLOW);
  init_pair(5, COLOR_RED, COLOR_BLACK);
  init_pair(6, COLOR_BLUE, COLOR_BLACK);
  init_pair(7, COLOR_YELLOW, COLOR_BLACK);
}

ManageGame::~ManageGame() { endwin(); /* End curses mode		  */ }
