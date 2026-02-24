#include <array>
#include <span>

namespace TankConstants {
namespace Move {
constexpr std::array<std::array<int, 2>, 8> K = {{
    {1, 0},
    {1, -1},
    {0, -1},
    {-1, -1},
    {-1, 0},
    {-1, 1},
    {0, 1},
    {1, 1},
}};

constexpr std::array<std::array<int, 2>, 8> J = {
    {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}}};

constexpr std::array<std::array<int, 4>, 8> Q = {{{3, 0, 1, 0},
                                                  {3, -2, 1, -1},
                                                  {0, -3, 0, -1},
                                                  {-3, -2, -1, -1},
                                                  {-3, 0, -1, -0},
                                                  {-3, 2, -1, 1},
                                                  {0, 3, 0, 1},
                                                  {3, 3, 1, 1}}};
} // namespace Move

namespace Direction {
enum {
  HORIZONTAL_RIGHT,
  RIGHT_UP,
  VERTICAL_UP,
  LEFT_UP,
  HORIZONTAL_LEFT,
  LEFT_DOWN,
  VERTICAL_DOWN,
  RIGHT_DOWN,
};
}

using Vec2 = std::array<int, 2>;
namespace Array {
constexpr std::array<Vec2, 10> HORIZONTAL_RIGHT = {{{-2, 0},
                                                    {-1, 0},
                                                    {0, 0},
                                                    {1, 0},
                                                    {2, 0},
                                                    {-2, 1},
                                                    {-1, 1},
                                                    {0, 1},
                                                    {1, 1},
                                                    {2, 1}}};

constexpr std::array<Vec2, 10> RIGHT_UP = {{{-2, 1},
                                            {-1, 0},
                                            {0, 0},
                                            {1, 0},
                                            {2, -1},
                                            {-2, 2},
                                            {-1, 1},
                                            {0, 1},
                                            {1, 1},
                                            {2, 0}}};

constexpr std::array<Vec2, 8> VERTICAL_UP = {
    {{0, -1}, {0, 0}, {0, 1}, {0, 2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}}};

constexpr std::array<Vec2, 10> LEFT_UP = {{{-2, -0},
                                           {-1, 0},
                                           {0, 0},
                                           {1, 0},
                                           {2, 1},
                                           {-2, -1},
                                           {-1, 1},
                                           {0, 1},
                                           {1, 1},
                                           {2, 2}}};

constexpr std::array<Vec2, 10> HORIZONTAL_LEFT = {{{-2, 0},
                                                   {-1, 0},
                                                   {0, 0},
                                                   {1, 0},
                                                   {2, 0},
                                                   {-2, 1},
                                                   {-1, 1},
                                                   {0, 1},
                                                   {1, 1},
                                                   {2, 1}}};

constexpr std::array<Vec2, 10> LEFT_DOWN = {{{-2, 1},
                                             {-1, 0},
                                             {0, 0},
                                             {1, 0},
                                             {2, -0},
                                             {-2, 2},
                                             {-1, 1},
                                             {0, 1},
                                             {1, 1},
                                             {2, -1}}};

constexpr std::array<Vec2, 8> VERTICAL_DOWN = {
    {{0, -1}, {0, 0}, {0, 1}, {0, 2}, {1, -1}, {1, 0}, {1, 1}, {1, 2}}};

constexpr std::array<Vec2, 10> RIGHT_DOWN = {{{-2, -0},
                                              {-1, 0},
                                              {0, 0},
                                              {1, 0},
                                              {2, 1},
                                              {-2, -1},
                                              {-1, 1},
                                              {0, 1},
                                              {1, 1},
                                              {2, 2}}};
} // namespace Array

constexpr std::array<std::span<const Vec2>, 8> IMAGE_OFFSETS = {
    Array::HORIZONTAL_RIGHT, Array::RIGHT_UP,        Array::VERTICAL_UP,
    Array::LEFT_UP,          Array::HORIZONTAL_LEFT, Array::LEFT_DOWN,
    Array::VERTICAL_DOWN,    Array::RIGHT_DOWN,
};
} // namespace TankConstants
