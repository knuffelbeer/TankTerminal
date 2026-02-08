#include "../include/tank.h"
#include "../include/game.h"
#include <utility>
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
  switch (orientation) {
  case D_HORIZONTAL_RIGHT:
    x += 1;
    break;
  case D_RIGHT_UP:
    x += 1;
    y -= 1;
    break;
  case D_VERTICAL_UP:
    y -= 1;
    break;
  case D_LEFT_UP:
    x -= 1;
    y -= 1;
    break;
  case D_HORIZONTAL_LEFT:
    x -= 1;
    break;
  case D_LEFT_DOWN:
    x -= 1;
    y += 1;
    break;
  case D_VERTICAL_DOWN:
    y += 1;
    break;
  case D_RIGHT_DOWN:
    x += 1;
    y += 1;
    break;
  }
}

void Tank::j() {
  switch (orientation) {
  case D_HORIZONTAL_RIGHT:
    x -= 1;
    break;
  case D_RIGHT_UP:
    x -= 1;
    y += 1;
    break;
  case D_VERTICAL_UP:
    y += 1;
    break;
  case D_LEFT_UP:
    x += 1;
    y += 1;
    break;
  case D_HORIZONTAL_LEFT:
    x += 1;
    break;
  case D_LEFT_DOWN:
    x += 1;
    y -= 1;
    break;
  case D_VERTICAL_DOWN:
    y -= 1;
    break;
  case D_RIGHT_DOWN:
    x -= 1;
    y -= 1;
    break;
  }
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

template <typename F, typename... Args>
void Tank::apply(F &&fun, Args &&...extra) {
  switch (image) {
  case D_HORIZONTAL_RIGHT:
    for (int i = -2; i < 3; i++) {
      fun(x + i, y, std::forward<Args>(extra)...);
      fun(x + i, y + 1, std::forward<Args>(extra)...);
    }
    break;
  case D_RIGHT_UP:
    fun(x - 2, y + 1, std::forward<Args>(extra)...);
    fun(x - 2, y + 2, std::forward<Args>(extra)...);
    for (int i = -1; i < 2; i++) {
      fun(x + i, y, std::forward<Args>(extra)...);
      fun(x + i, y + 1, std::forward<Args>(extra)...);
    }
    fun(x + 2, y - 1, std::forward<Args>(extra)...);
    fun(x + 2, y, std::forward<Args>(extra)...);
    break;
  case D_VERTICAL_UP:
    for (int i = -1; i < 3; i++) {
      fun(x, y + i, std::forward<Args>(extra)...);
      fun(x + 1, y + i, std::forward<Args>(extra)...);
    }
    break;
  case D_LEFT_UP:
    fun(x - 2, y - 1, std::forward<Args>(extra)...);
    fun(x - 2, y, std::forward<Args>(extra)...);
    for (int i = -1; i < 2; i++) {
      fun(x + i, y, std::forward<Args>(extra)...);
      fun(x + i, y + 1, std::forward<Args>(extra)...);
    }
    fun(x + 2, y + 1, std::forward<Args>(extra)...);
    fun(x + 2, y + 2, std::forward<Args>(extra)...);
    break;
  case D_HORIZONTAL_LEFT:
    for (int i = -2; i < 3; i++) {
      fun(x + i, y, std::forward<Args>(extra)...);
      fun(x + i, y + 1, std::forward<Args>(extra)...);
      break;
    }
  case D_LEFT_DOWN:
    fun(x - 2, y + 1, std::forward<Args>(extra)...);
    fun(x - 2, y + 2, std::forward<Args>(extra)...);
    for (int i = -1; i < 2; i++) {
      fun(x + i, y, std::forward<Args>(extra)...);
      fun(x + i, y + 1, std::forward<Args>(extra)...);
    }
    fun(x + 2, y - 1, std::forward<Args>(extra)...);
    fun(x + 2, y, std::forward<Args>(extra)...);
    break;
  case D_VERTICAL_DOWN:
    for (int i = -1; i < 3; i++) {
      fun(x, y + i, std::forward<Args>(extra)...);
      fun(x + 1, y + i, std::forward<Args>(extra)...);
    }
    break;
  case D_RIGHT_DOWN:
    fun(x - 2, y - 1, std::forward<Args>(extra)...);
    fun(x - 2, y, std::forward<Args>(extra)...);
    for (int i = -1; i < 2; i++) {
      fun(x + i, y, std::forward<Args>(extra)...);
      fun(x + i, y + 1, std::forward<Args>(extra)...);
    }
    fun(x + 2, y + 1, std::forward<Args>(extra)...);
    fun(x + 2, y + 2, std::forward<Args>(extra)...);
    break;
  }
}

bool Tank::check_hit(const std::vector<Bullet> &bullets) {
  bool hit = false;
  for (const Bullet &b : bullets) {
    apply([b, &hit](int x, int y) {
      if (x == b.x && y == b.y)
        hit = true;
    });
    if (hit)
      break;
  }
  return hit;
}

void Tank::draw() {
  wattron(my_win, COLOR_PAIR(color_pair));
  apply([this](int x, int y) { draw_single_point(x, y); });
  wattroff(my_win, COLOR_PAIR(color_pair));
}

bool Tank::check_move(const std::vector<Wall> &walls) {
  bool valid = true;
  apply([&walls, &valid](int x, int y) {
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
  game->spawn_bullet(x, y, vx, vy);
}

void Tank::q(Game *game) {
  switch (orientation) {
  case D_RIGHT_DOWN:
    request_shot(game, x + 3, y + 3, 1, 1);
    break;
  case D_HORIZONTAL_RIGHT:
    request_shot(game, x + 3, y, 1, 0);
    break;
  case D_RIGHT_UP:
    request_shot(game, x + 3, y - 2, 1, -1);
    break;
  case D_VERTICAL_UP:
    request_shot(game, x, y - 3, 0, -1);
    break;
  case D_LEFT_UP:
    request_shot(game, x - 3, y - 2, -1, -1);
    break;
  case D_HORIZONTAL_LEFT:
    request_shot(game, x - 3, y, -1, 0);
    break;
  case D_LEFT_DOWN:
    request_shot(game, x - 3, y + 2, -1, 1);
    break;
  case D_VERTICAL_DOWN:
    request_shot(game, x, y + 3, 0, 1);
    break;
  }
}

void Tank::update(Game *game, int ch, bool &run) {
  if (check_hit(game->bullets)) {
    run = false;
  }
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
