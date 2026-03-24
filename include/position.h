#pragma once
#include <cstdint>
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

// All members must be uint32_t for safe and easy sending!
struct Position {
  uint32_t x;
  uint32_t y;
  uint32_t type_idx;
};

struct TankLayout {
  uint32_t x{};
  uint32_t y{};
  uint32_t orientation{};
  uint32_t color_pair;
};

struct Message {
  uint32_t size;
  uint32_t type;
};

struct WallLayout {
  uint32_t direction;
  uint32_t loc;
  uint32_t start;
  uint32_t stop;
};

#pragma pack(pop)
#define ELEMENT_TYPES                                                          \
  Bullet, Rocket, RocketSprite, ZapPixel, ZapAimPixel, ZapSprite, MineSprite,  \
      Mine
