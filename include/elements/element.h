#pragma once

class Game;
class Element {
public:
  int t{};
  bool active = true;
  int x;
  int y;
  int vx{};
  int vy{};
  bool is_hit = false;
  constexpr static int t_max = 30;
  virtual void move(Game *game) {}
  virtual void draw(Game *game) {}
  virtual void hit(Game *game) {}
  virtual void cleanup(Game *game);

protected:
  Element(int x, int y);
  Element(int x, int y, int vx, int vy);

	void declare_winner(Game* game);
  static void step(Game *game, int &x, int &y, int &vx, int &vy);
};
