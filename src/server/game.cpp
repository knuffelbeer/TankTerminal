#include "../../include/server/game.h"
#include "../../include/buffer.h"
#include "../../include/server/elements/bullet.h"
#include "../../include/server/elements/mine.h"
#include "../../include/server/elements/rocket.h"
#include "../../include/server/elements/zap.h"
#include "../../include/server/wall.h"
#include <cassert>
#include <concepts>
#include <functional>
#include <mutex>
#include <ncurses.h>
#include <thread>
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
  assert(height > border_height && "height must be bigger then border_height");
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
void Game::send_data() {}

void Game::run_game() {
  Server server;
  while (!server.connections_ready) {
    server.listen_for_connections();
  }
  std::thread t2(&Game::loop, this, &server);
  std::thread t1(&Server::recieve_input, &server, std::ref(input),
                 std::ref(mtx));
  t1.join();
  t2.join();
}

void Game::loop(Server *server) {
  int l{};
  while (ManageGame_run) {
    clear();
    l = (l + 1) % 3;
    make_level(l);
    for (int i = 0; i < tanks.size(); i++) {
      wattron(my_win, COLOR_PAIR(tanks[i].color_pair));
      mvwprintw(my_win, level_height + 3, i * 10, "score: %i", tanks[i].score);
      wattroff(my_win, COLOR_PAIR(tanks[i].color_pair));
    }
    iteration(server);
    usleep(999999);
    reset();
  }
}

void Game::iteration(Server *server) {
  bool walls_drawn{};
  nodelay(stdscr, true);
  while (run) {
    {
      std::lock_guard<std::mutex> guard(mtx);
      if (!input.empty()) {
        ch = input.front();
        input.pop();
      } else {
        ch = 0;
      }
    }

    for (Wall w : walls) {
      w.draw(my_win);
    }
    for (auto p : {0, 1}) {
      current_player = p;
      tanks[p].update(this, ch, run);
    }
    if (ch == 'x') {
      ManageGame_run = false;
      break;
    }
    auto buf = Buffer();
    uint32_t size_msg = (int)sizeof(Message) + 2 * (int)sizeof(TankLayout);

    if (!walls_drawn) {
      auto wall_buffer = Buffer();
      uint32_t size_msg =
          (int)sizeof(Message) + (int)sizeof(Wall) * walls.size();
      wall_buffer.add(Message{size_msg, 1});
      for (Wall w : walls) {
        wall_buffer.add(w);
      }
      server->send_data(wall_buffer.data, wall_buffer.get_num_bytes());
      walls_drawn = true;
    }

    for (const auto &el : elements) {
      std::visit(
          [&size_msg](const auto &obj) {
            Position pos_temp = obj;
            if constexpr (std::same_as<decltype(obj), const MineSprite &> ||
                          std::same_as<decltype(obj), const Mine &>) {
              if (obj.visible && obj.active) {
                size_msg += (int)sizeof(Position);
              }
            } else {
              if (obj.active) {
                size_msg += (int)sizeof(Position);
              }
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
            if constexpr (std::same_as<decltype(obj), const MineSprite &> ||
                          std::same_as<decltype(obj), const Mine &>) {
              if (obj.visible && obj.active) {
                buf.add(pos_temp);
              }
            } else {
              if (obj.active) {
                buf.add(pos_temp);
              }
            }
          },
          el);
    }
    if (buf.get_num_bytes() != size_msg) {
      printf("message size is corrupt! size_msg: %i, get_num_bytes(): %i",
             size_msg, buf.get_num_bytes());
    }
    update_bullets();
    server->send_data(buf.data, buf.get_num_bytes());
    wrefresh(my_win);
    usleep(DELTA_MS);
  }
}
