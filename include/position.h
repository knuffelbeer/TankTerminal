#pragma once
enum {
  BULLET_TYPE,
  MINE_TYPE,
  MINE_SPRITE_TYPE,
  ROCKET_TYPE,
  ROCKET_SPRITE_TYPE,
  ZAP_TYPE,
  ZAP_PIXEL_TYPE,
  ZAP_AIM_PIXEL_TYPE,
  ZAP_SPRITE_TYPE,
};

#pragma pack(push, 1)
struct Position {
  int x;
  int y;
  int type_idx;
};

struct TankLayout {
  int x{};
  int y{};
  int orientation{};
  int color_pair;
};

struct Message {
  int size;
  int type;
};

struct WallLayout {
  int direction;
  int loc;
  int start;
  int stop;
};
#pragma pack(pop)
#define ELEMENT_TYPES                                                          \
  Bullet, Rocket, RocketSprite, ZapPixel, ZapAimPixel, ZapSprite, MineSprite,  \
      Mine
