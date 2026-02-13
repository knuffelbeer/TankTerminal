#include "../include/game.h"
#include <memory>
#include <utility>

Game::Game(bool &ManageGame_run, int width, int height, int startx, int starty)
    : Window(width, height, startx, starty), ManageGame_run(ManageGame_run) {
  build();
}

Game::Game(bool &ManageGame_run, int width, int height)
    : Window(width, height), ManageGame_run(ManageGame_run) {
  build();
}
void Game::reset() {
  wclear(my_win);
  run = true;
  elements.erase(elements.begin(), elements.end());
  for (int i = 0; i < tanks.size(); i++) {
    auto &tank = tanks[i];
    tank.reset();
    auto [x, y] = tank_positions[i];
    tank.x = x;
    tank.y = y;
  }
}

void Game::build() {
  level_height = height - border_height;
  level_width = width;
  tanks = {
      Tank(my_win, 10, 10, 0, 'd', 'a', 'w', 's', 'q', 1),
      Tank(my_win, 25, 5, 0, KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN, ' ', 2)};
  tank_positions = {{10, 10}, {25, 5}};
}

void Game::make_level(int num_level) {
  switch (num_level) {
  case 0: {
    walls = {{'H', 0, 0, level_width},
             {'H', level_height - 1, 0, level_width},
             {'V', 0, 0, level_height},
             {'V', level_width - 1, 0, level_height},
             {'V', level_width / 2, 0, level_height / 2},
             {'H', level_height / 2 + 3, 5, level_width / 3},
             {'H', 5, level_width - 10, level_width},
             {'H', level_height - 5, level_width - 10, level_width}};
    elements.push_back(std::make_unique<ZapSprite>(5, 5));
  } break;
  case 1: {
    walls = {{'H', 0, 0, level_width},
             {'H', level_height - 1, 0, level_width},
             {'V', 0, 0, level_height},
             {'V', level_width - 1, 0, level_height},
             {'V', level_width / 2, 0, level_height / 2},
             {'V', level_height / 2 + 3, 5, level_width / 3},
             {'H', 5, level_width - 10, level_width},
             {'V', level_width - 10, level_height - 10, level_height}};
    elements.push_back(std::make_unique<ZapSprite>(5, 5));
  } break;
  case 2: {
    walls = {{'H', 0, 0, level_width - 5},
             {'H', level_height - 1, 0, level_width - 5},
             {'V', 0, 0, level_height},
             {'V', (level_width - 5) - 1, 0, level_height},
             {'V', 10, level_height - 5, level_height},
             {'V', 20, 5, level_height - 10},
             {'H', 5, 10, 20},
             {'V', level_width - 15, level_height - 10, level_height}};
    elements.push_back(std::make_unique<ZapSprite>(8, 7));
  } break;
  }
}

void Game::update_bullets() {
  for (int i = 0; i < elements.size();) {
    if (!elements[i]->active) {
      elements[i]->cleanup(this);
      elements.erase(elements.begin() + i);
    } else {
      elements[i]->move(this);
      elements[i]->draw(this);
      i++;
    }
  }
}

void Game::spawn_bullet(int x, int y, int vx, int vy) {
  bullets.push_back(Bullet(x, y, vx, vy));
  spawn<Bullet>(x, y, vx, vy);
}

void Game::loop() {
  for (Tank &t : tanks) {
    t.draw();
  }
  while (run) {
    ch = getch();
    for (Wall w : walls) {
      w.draw(my_win);
    }
    current_player = 0;
    for (Tank &t : tanks) {
      t.update(this, ch, run);
      current_player++;
    }
    update_bullets();
    if (ch == 'x') {
      ManageGame_run = false;
      break;
    }
    wrefresh(my_win);
    usleep(DELTA_MS);
  }
}
