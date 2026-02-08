#include "game.h"

Game::Game(int width, int height, int startx, int starty)
    : Window(width, height, startx, starty) {
  build();
}
Game::Game(int width, int height) : Window(width, height) { build(); }

void Game::build() {
  Tank tank = Tank(my_win, 10, 10, 0, 'd', 'a', 'w', 's', 'q', 1);
  Tank tank2 =
      Tank(my_win, 25, 5, 0, KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN, ' ', 2);
  tanks.push_back(tank);
  tanks.push_back(tank2);
  Wall h1{.my_win = my_win,
          .direction = 'H',
          .loc = 0, // y_location
          .start = 0,
          .stop = width};
  Wall h2{.my_win = my_win,
          .direction = 'H',
          .loc = height - 1, // y_location
          .start = 0,
          .stop = width};
  Wall h3{.my_win = my_win,
          .direction = 'V',
          .loc = 0, // y_location
          .start = 0,
          .stop = height};
  Wall h4{.my_win = my_win,
          .direction = 'V',
          .loc = width - 1, // y_location
          .start = 0,
          .stop = height};
  Wall h5{.my_win = my_win,
          .direction = 'V',
          .loc = width / 2, // y_location
          .start = 0,
          .stop = height / 2};
  Wall h6{.my_win = my_win,
          .direction = 'H',
          .loc = height / 2 + 3,
          .start = 5,
          .stop = width / 3};
  Wall h7{.my_win = my_win,
          .direction = 'H',
          .loc = 5, // y_location
          .start = width - 10,
          .stop = width};
  Wall h8{.my_win = my_win,
          .direction = 'H',
          .loc = height - 5, // y_location
          .start = width - 10,
          .stop = width};
  walls.push_back(h1);
  walls.push_back(h2);
  walls.push_back(h3);
  walls.push_back(h4);
  walls.push_back(h5);
  walls.push_back(h6);
  walls.push_back(h7);
  walls.push_back(h8);
}

void Game::update_bullets() {
  for (int i = 0; i < bullets.size();) {
    bullets[i].move(walls);
    bullets[i].draw();
    if (bullets[i].t == 40) {
      bullets.erase(bullets.begin() + i);
    } else {
      i++;
    }
  }
}

void Game::spawn_bullet(int x, int y, int vx, int vy) {
  bullets.push_back(Bullet(my_win, x, y, vx, vy));
}

void Game::loop() {
  bool run = true;
  while (run) {
    ch = getch();
    wclear(my_win);
    for (Wall w : walls) {
      w.draw();
    }
    update_bullets();

    for (Tank &t : tanks) {
      t.update(this, ch, run);
    }
    if (ch == 'x')
      break;
    wrefresh(my_win);
    usleep(DELTA_MS);
  }
}
