#include "../include/tank.h"
#include "../include/game.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <ncurses.h>
#include <unistd.h>
#include <vector>

std::function<void(Game *game, int, int, int, int)> Tank::normal_shoot =
    [](Game *game, int sx, int sy, int vx, int vy) {
      game->spawn_bullet(sx, sy, vx, vy,
                         game->tanks[game->current_player].counter);
    };

Tank::Tank(WINDOW *my_win, int left, int right, int up, int down, int shoot,
           int color_pair)
    : my_win(my_win), left(left), right(right), up(up), down(down),
      shoot_button(shoot), color_pair(color_pair) {
  setup();
}

Tank::Tank(WINDOW *my_win, int x, int y, int image, int left, int right, int up,
           int down, int shoot, int color_pair)
    : my_win(my_win), x(x), y(y), image(image), orientation(image), left(left),
      right(right), up(up), down(down), shoot_button(shoot),
      color_pair(color_pair) {

  setup();
}
void Tank::reset() {
  exploded = false;
  setup();
}

void Tank::setup() {
  custom_shot = normal_shoot;
  move = normal_move;
}

void Tank::l() {
  image += 1;
  orientation += 1;
  image = image % 4;
  orientation = orientation % 8;
}

void Tank::h() {
  if (image > 0)
    image -= 1;
  else
    image = 3;
  if (orientation > 0)
    orientation -= 1;
  else
    orientation = 7;
}

void Tank::k() {
  auto [move_x, move_y] = MOVE_K[orientation];
  x += move_x;
  y += move_y;
}

void Tank::j() {
  auto [dx, dy] = MOVE_J[orientation];
  x += dx;
  y += dy;
}

void Tank::straight_horizontal() {
  for (int i = -2; i < 3; i++) {
    draw_single_point(y, x + i);
    draw_single_point(y + 1, x + i);
  }
}

void Tank::straight_vertical() {
  for (int i = -1; i < 3; i++) {
    draw_single_point(y + i, x);
    draw_single_point(y + i, x + 1);
  }
}

void Tank::draw_single_point(int x, int y) { mvwaddch(my_win, y, x, ' '); }
template <typename F> void Tank::for_all_points(F &&fun) {
  for (auto [dx, dy] : image_offsets.at(image)) {
    fun(x + dx, y + dy);
  }
}

bool Tank::check_hit(int other_x, int other_y) {
  bool is_hit = false;
  for_all_points([&is_hit, other_x, other_y](int x, int y) {
    if (x == other_x && y == other_y)
      is_hit = true;
  });
  return is_hit;
}

bool Tank::check_and_process_hit(Game *game) {
  bool is_hit = false;
  for (auto &b : game->elements) {
    bool hit = false;
    for_all_points([&b, &hit, &is_hit](int x, int y) {
      if (x == b->x && y == b->y)
        hit = true;
      is_hit = true;
    });
    if (hit) {
      b->hit(game);
    }
  }
  return is_hit;
}
void Tank::draw_color(int color) {
  wattron(my_win, COLOR_PAIR(color));
  for_all_points([this](int x, int y) { draw_single_point(x, y); });
  wattroff(my_win, COLOR_PAIR(color));
}
void Tank::draw() {
  if (!exploded)
    draw_color(color_pair);
  else
    animation();
}

void Tank::animation() {
  wattron(my_win, COLOR_PAIR(BLACK_BLACK));
  for_all_points(
      [this](int new_x, int new_y) { draw_single_point(new_x, new_y); });
  wattroff(my_win, COLOR_PAIR(BLACK_BLACK));

  constexpr int amt_explosion_colors = 3;
  int colors[amt_explosion_colors];
  colors[0] = color_pair;
  colors[1] = GREEN_YELLOW;
  colors[2] = BLACK_ORANGE;

  for (int i = 0; i < 6; i++) {

    for (int j = 0; j < amt_explosion_colors; j++) {

      int radius = i - j;

      if (radius < 0)
        continue;

      wattron(my_win, COLOR_PAIR(colors[j]));

      for_all_points([this, radius](int new_x, int new_y) {
        int dx = new_x - x;
        int dy = new_y - y;

        mvwaddch(my_win, y + radius * dy - radius / 2,
                 x + radius * dx - radius / 2, ' ');
      });

      wattroff(my_win, COLOR_PAIR(colors[j]));
    }

    wrefresh(my_win);
    usleep(120000);
  }
}

bool Tank::check_move(const std::vector<Wall> &walls) {
  bool valid = true;
  for_all_points([&walls, &valid](int x, int y) {
    for (const Wall &wall : walls) {
      switch (wall.direction) {
      case 'H':
        if (y == wall.loc && x >= wall.start && x < wall.stop)
          valid = false;
        break;
      case 'V':
        if (x == wall.loc && y >= wall.start && y < wall.stop)
          valid = false;
        break;
      }
    }
  });
  return valid;
}

void Tank::q(Game *game) {
  auto [dx, dy, vx, vy] = MOVE_Q[orientation];
  custom_shot(game, x + dx, y + dy, vx, vy);
}

void Tank::update(Game *game, int ch, bool &run) {
  check_and_process_hit(game);
  move(ch, game);
  draw();
}

void Tank::update_for_move(Game *game, void (Tank::*move)(),
                           void (Tank::*opposite)()) {
  draw_color(0);
  (this->*move)();
  if (!check_move(game->walls)) {
    (this->*opposite)();
  }
}

std::function<void(int, Game *)> Tank::normal_move = [](int ch, Game *game) {
  auto &current_tank = game->tanks[game->current_player];
  if (ch == ERR)
    return;
  else if (ch == current_tank.down) {
    current_tank.update_for_move(game, &Tank::j, &Tank::k);
  } else if (ch == current_tank.up) {
    current_tank.update_for_move(game, &Tank::k, &Tank::j);

  } else if (ch == current_tank.left) {
    current_tank.update_for_move(game, &Tank::h, &Tank::l);

  } else if (ch == current_tank.right) {
    current_tank.update_for_move(game, &Tank::l, &Tank::h);

  } else if (ch == current_tank.shoot_button)
    current_tank.q(game);
};

const std::map<int, std::vector<std::pair<int, int>>> Tank::image_offsets = {
    {D_HORIZONTAL_RIGHT,
     {{-2, 0},
      {-1, 0},
      {0, 0},
      {1, 0},
      {2, 0},
      {-2, 1},
      {-1, 1},
      {0, 1},
      {1, 1},
      {2, 1}}},
    {D_RIGHT_UP,
     {{-2, 1},
      {-1, 0},
      {0, 0},
      {1, 0},
      {2, -1},
      {-2, 2},
      {-1, 1},
      {0, 1},
      {1, 1},
      {2, 0}}},
    {D_VERTICAL_UP,
     {{0, -1}, {0, 0}, {0, 1}, {0, 2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}}},
    {D_LEFT_UP,
     {{-2, -0},
      {-1, 0},
      {0, 0},
      {1, 0},
      {2, 1},
      {-2, -1},
      {-1, 1},
      {0, 1},
      {1, 1},
      {2, 2}}},
    {D_HORIZONTAL_LEFT,
     {{-2, 0},
      {-1, 0},
      {0, 0},
      {1, 0},
      {2, 0},
      {-2, 1},
      {-1, 1},
      {0, 1},
      {1, 1},
      {2, 1}}},
    {D_LEFT_DOWN,
     {{-2, 1},
      {-1, 0},
      {0, 0},
      {1, 0},
      {2, -0},
      {-2, 2},
      {-1, 1},
      {0, 1},
      {1, 1},
      {2, -1}}},
    {D_VERTICAL_DOWN,
     {{0, -1}, {0, 0}, {0, 1}, {0, 2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}}},
    {D_RIGHT_DOWN,
     {{-2, -0},
      {-1, 0},
      {0, 0},
      {1, 0},
      {2, 1},
      {-2, -1},
      {-1, 1},
      {0, 1},
      {1, 1},
      {2, 2}}}};
