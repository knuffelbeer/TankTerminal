#include "../include/tank.h"
#include "../include/game.h"
#include <memory>
#include <vector>

Tank::Tank(WINDOW *my_win, int left, int right, int up, int down, int shoot,
           int color_pair)
    : my_win(my_win), left(left), right(right), up(up), down(down),
      shoot(shoot), color_pair(color_pair) {}
Tank::Tank(WINDOW *my_win, int x, int y, int image, int left, int right, int up,
           int down, int shoot, int color_pair)
    : my_win(my_win), x(x), y(y), image(image), orientation(image), left(left),
      right(right), up(up), down(down), shoot(shoot), color_pair(color_pair) {}

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
  auto amt_rows_tank = {0, 1};
  for (auto [dx, dy] : image_offsets.at(image)) {
    fun(x + dx, y + dy);
  }
}

bool Tank::check_hit(Game *game) {
  bool hit = false;
  for (auto &b : game->elements) {
    for_all_points([&b, &hit](int x, int y) {
      if (x == b->x && y == b->y)
        hit = true;
    });
    if (hit)
		{
      b->hit(game);
		}
  }
  return hit;
}

void Tank::draw() {
  wattron(my_win, COLOR_PAIR(color_pair));
  for_all_points([this](int x, int y) { draw_single_point(x, y); });
  wattroff(my_win, COLOR_PAIR(color_pair));
}

bool Tank::check_move(const std::vector<Wall> &walls) {
  bool valid = true;
  for_all_points([&walls, &valid](int x, int y) {
    for (const Wall &wall : walls) {
      switch (wall.direction) {
      case 'H':
        if (y == wall.loc && x >= wall.start && x <= wall.stop)
          valid = false;
        break;
      case 'V':
        if (x == wall.loc && y >= wall.start && y <= wall.stop)
          valid = false;
        break;
      }
    }
  });
  return valid;
}

void Tank::request_shot(Game *game, int x, int y, int vx, int vy) {
  switch (fire_element) {
  case 'B':
    game->spawn_bullet(x, y, vx, vy);
    break;
  case 'V':
    int x_temp = x;
    int y_temp = y;
    int vx_temp = vx;
    int vy_temp = vy;
    for (int i = 0; i < 20; i++) {
      for (auto w : game->walls) {
        if (w.direction == 'H') {
          if (vy_temp == 0 && w.loc == y_temp &&
              (w.start == x_temp || w.stop == x_temp)) {
            vx_temp = -vx_temp;
          }

          if (y_temp == w.loc && w.start <= x_temp && x_temp <= w.stop) {
            vy_temp = -vy_temp;
          }
        }
        if (w.direction == 'V') {
          if (vx_temp == 0 && w.loc == x_temp &&
              (w.start == y_temp || w.stop == y_temp)) {
            vy_temp = -vy_temp;
          }
          if (x_temp == w.loc && w.start <= y_temp && y_temp <= w.stop) {
            vx_temp = -vx_temp;
          }
        }
      }
      x_temp += vx_temp;
      y_temp += vy_temp;
      game->spawn<ZapPixel>(x_temp, y_temp);
    }
    break;
  }
}

void Tank::q(Game *game) {
  auto [dx, dy, vx, vy] = MOVE_Q[orientation];
  request_shot(game, x + dx, y + dy, vx, vy);
}

void Tank::update(Game *game, int ch, bool &run) {
  check_hit(game);
  draw();
  move(ch, game);
}

void Tank::move(int ch, Game *game) {
  if (ch == ERR)
    return;

  else if (ch == down) {

    j();
    if (!check_move(game->walls))
      k();

  } else if (ch == up) {

    k();
    if (!check_move(game->walls))
      j();

  } else if (ch == left) {

    h();
    if (!check_move(game->walls))
      l();

  } else if (ch == right) {

    l();
    if (!check_move(game->walls))
      h();

  } else if (ch == shoot)
    q(game);
}

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
