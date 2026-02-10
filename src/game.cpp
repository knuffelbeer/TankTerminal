#include "../include/game.h"

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
}

void Game::make_level(int num_level) {
  switch (num_level) {
  case 0: {
    walls = {{'H', 0, 0, width},
             {'H', height - 1, 0, width},
             {'V', 0, 0, height},
             {'V', width - 1, 0, height},
             {'V', width / 2, 0, height / 2},
             {'H', height / 2 + 3, 5, width / 3},
             {'H', 5, width - 10, width},
             {'H', height - 5, width - 10, width}};
  } break;
  case 1: {
    walls = {{'H', 0, 0, width},
             {'H', height - 1, 0, width},
             {'V', 0, 0, height},
             {'V', width - 1, 0, height},
             {'V', width / 2, 0, height / 2},
             {'V', height / 2 + 3, 5, width / 3},
             {'H', 5, width - 10, width},
             {'V', width - 10, height - 10, height}};
  } break;
  case 2: {
    walls = {{'H', 0, 0, width - 5},
             {'H', height - 1, 0, width - 5},
             {'V', 0, 0, height},
             {'V', (width - 5) - 1, 0, height},
             {'V', 10, height - 5, height},
             {'V', 20, 5, height - 10},
             {'H', 5, 10, 20},
             {'V', width - 15, height - 10, height}};
  } break;
  }
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
      w.draw(my_win);
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
