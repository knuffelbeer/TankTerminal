
#include "../../include/server/buffer.h"
#include "../../include/position.h"
#include <array>
#include <cstring>
#include <initializer_list>
#include <sys/_endian.h>

void Buffer::add(const Position &pos) {
  if (index + sizeof(Position) > max_bytes) {
    printf("number of bytes exceeds buffer");
    throw;
  }
  std::array<uint32_t, 3> hostints;
  for (int i = 0; const auto &e : {pos.x, pos.y, pos.type_idx}) {
    hostints[i++] = htonl(e);
  }
  std::memcpy(&data[index], hostints.data(), sizeof(hostints));
  index += sizeof(hostints);
}

void Buffer::add(const TankLayout &tank) {
  if (index + sizeof(Position) > max_bytes) {
    printf("number of bytes exceeds buffer");
    throw;
  }
  std::array<uint32_t, 4> hostints;
  for (int i = 0;
       const auto &e : {tank.x, tank.y, tank.orientation, tank.color_pair}) {
    hostints[i++] = htonl(e);
  }
  std::memcpy(&data[index], hostints.data(), sizeof(hostints));
  index += sizeof(hostints);
}

void Buffer::add(const Message &m) {
  if (index + sizeof(Message) > max_bytes) {
    printf("number of bytes exceeds buffer");
    throw;
  }
  std::array<uint32_t, 2> hostints;
  for (int i = 0; const auto &e : {m.size, m.type}) {
    hostints[i++] = htonl(e);
  }
  std::memcpy(&data[index], hostints.data(), sizeof(hostints));
  index += sizeof(hostints);
}

void Buffer::add(const WallLayout &wall) {
  if (index + sizeof(Position) > max_bytes) {
    printf("number of bytes exceeds buffer");
    throw;
  }
  std::array<uint32_t, 4> hostints;
  for (int i = 0;
       const auto &e : {wall.direction, wall.loc, wall.start, wall.stop}) {
    hostints[i++] = htonl(e);
  }
  std::memcpy(&data[index], hostints.data(), sizeof(hostints));
  index += sizeof(hostints);
}
