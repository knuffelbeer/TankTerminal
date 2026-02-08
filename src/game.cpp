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
    walls = {h1, h2, h3, h4, h5, h6, h7, h8};
  } break;
  case 1: {
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
            .direction = 'V',
            .loc = height / 2 + 3,
            .start = 5,
            .stop = width / 3};
    Wall h7{.my_win = my_win,
            .direction = 'H',
            .loc = 5, // y_location
            .start = width - 10,
            .stop = width};
    Wall h8{.my_win = my_win,
            .direction = 'V',
            .loc = width - 10, // y_location
            .start = height - 10,
            .stop = height};
    walls = {h1, h2, h3, h4, h5, h6, h7, h8};
  } break;
  case 2: {
    Wall h1{.my_win = my_win,
            .direction = 'H',
            .loc = 0, // y_location
            .start = 0,
            .stop = width - 5};
    Wall h2{.my_win = my_win,
            .direction = 'H',
            .loc = height - 1, // y_location
            .start = 0,
            .stop = width - 5};
    Wall h3{.my_win = my_win,
            .direction = 'V',
            .loc = 0, // y_location
            .start = 0,
            .stop = height};
    Wall h4{.my_win = my_win,
            .direction = 'V',
            .loc = (width - 5) - 1, // y_location
            .start = 0,
            .stop = height};
    Wall h5{.my_win = my_win,
            .direction = 'V',
            .loc = 10, // y_location
            .start = height - 5,
            .stop = height};
    Wall h6{.my_win = my_win,
            .direction = 'V',
            .loc = 20,
            .start = 5,
            .stop = height - 10};
    Wall h7{.my_win = my_win,
            .direction = 'H',
            .loc = 5, // y_location
            .start = 10,
            .stop = 20};
    Wall h8{.my_win = my_win,
            .direction = 'V',
            .loc = width - 15, // y_location
            .start = height - 10,
            .stop = height};
    walls = {h1, h2, h3, h4, h5, h6, h7, h8};
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
