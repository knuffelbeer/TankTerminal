#include "../../include/server/game.h"

int main(int argc, char *argv[]) {
  auto m = Game(40,30);
  m.run_game();
  return 0;
}
