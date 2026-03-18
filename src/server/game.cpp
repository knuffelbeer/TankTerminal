#include "../../include/server/game.h"
#include "../../include/server/buffer.h"
#include "../../include/server/elements/bullet.h"
#include "../../include/server/elements/mine.h"
#include "../../include/server/elements/rocket.h"
#include "../../include/server/elements/zap.h"
#include "../../include/server/wall.h"
#include <ncurses.h>
#include <variant>
#include <vector>

Game::Game(bool &ManageGame_run, int width, int height, int startx, int starty)
    : Window(width, height, startx, starty), ManageGame_run(ManageGame_run) {
  elements.reserve(100);
  build();
}

Game::Game(bool &ManageGame_run, int width, int height)
    : Window(width, height), ManageGame_run(ManageGame_run) {
  build();
}

void Game::reset() {
  wclear(my_win);
  run = true;
  elements.clear();
  elements.reserve(100);
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
  tanks = {Tank(my_win, 10, 10, TankConstants::Direction::HORIZONTAL_LEFT, 'd',
                'a', 'w', 's', 'q', 1),
           Tank(my_win, 25, 5, TankConstants::Direction::HORIZONTAL_LEFT,
                KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN, ' ', 2)};
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
    elements.emplace_back(ZapSprite{5, 5});
    elements.emplace_back(RocketSprite{10, 15});
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
    elements.emplace_back(MineSprite{5, 5});
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
    elements.emplace_back(ZapSprite{8, 7});
  } break;
  }
}

void Game::update_bullets() {
  for (int i = 0; i < elements.size();) {
    bool del{};
    std::visit(
        [this, &del](auto &element) {
          if (!element.active) {
            element.cleanup(this);
            del = true;
          } else {
            element.move(this);
            element.draw(this);
          }
        },
        elements[i]);

    if (del)
      elements.erase(elements.begin() + i);
    else
      i++;
  }
}

void Game::spawn_bullet(int x, int y, int vx, int vy) {
  if (Bullet::counter[current_player] < 6)
    spawn<Bullet>(x, y, vx, vy, current_player);
}

void Game::loop() {
  bool walls_drawn{};
  while (run) {
    if (!server.connections_ready) {
      server.listen_for_connections();
      continue;
    }
    ch = getch();
    if (!walls_drawn) {
      auto wall_buffer = Buffer<500>();
      int size_msg = (int)sizeof(Message) + (int)sizeof(Wall) * walls.size();
      wall_buffer.add(Message{size_msg, 1});
      for (Wall w : walls) {
        wall_buffer.add(w);
      }
      server.iteration(wall_buffer.data, wall_buffer.get_num_bytes());
      walls_drawn = true;
    }
    for (Wall w : walls) {
      w.draw(my_win);
    }
    for (auto p : {0, 1}) {
      current_player = p;
      tanks[p].update(this, ch, run);
    }
    update_bullets();

    if (ch == 'x') {
      ManageGame_run = false;
      break;
    }
    auto buf = Buffer<500>();
    int size_msg = (int)sizeof(Message) + 2 * (int)sizeof(TankLayout);

    for (const auto &el : elements) {
      std::visit(
          [&size_msg](const auto &obj) {
            Position pos_temp = obj;
            if (obj.active) {
              size_msg += (int)sizeof(Position);
            }
          },
          el);
    }
    buf.add(Message{size_msg, 0});
    for (const auto &tank : tanks) {
      buf.add((TankLayout)tank);
    }
    for (int i = 0; const auto &el : elements) {
      std::visit(
          [&buf, &i, &size_msg](const auto &obj) {
            Position pos_temp = obj;
            if (obj.active) {
              buf.add(pos_temp);
            }
          },
          el);
    }
    if (buf.get_num_bytes() != size_msg) {
      printf("message size is corrupt! size_msg: %i, get_num_bytes(): %i",
             size_msg, buf.get_num_bytes());
    }
    server.iteration(buf.data, buf.get_num_bytes());
    wrefresh(my_win);
    usleep(DELTA_MS);
  }
}
