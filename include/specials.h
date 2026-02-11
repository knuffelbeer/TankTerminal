class Game;
class Special {
  int x;
  int y;
  bool active = true;
  virtual void move(Game *game);
};

class Zap : public Special {
	int t{};
  void move(Game *game) override;
};
