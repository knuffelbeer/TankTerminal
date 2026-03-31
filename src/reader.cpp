#include "../include/reader.h"
#include "../include/position.h"
#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <vector>

Reader::Reader() { buffer = std::vector<char>(BUFFERSIZE); }

void Reader::make_buf(int &bytes_overflow, int socket) {
  if (idx) {
    std::memmove(buffer.data(), buffer.data() + idx, bytes_overflow);
    idx = 0;
  }
  while (1) {
    if (bytes_overflow == buffer.size()) {
      buffer.resize(2 * buffer.size());
    }
    int mesg =  recv(socket, buffer.data() + bytes_overflow,
                    buffer.size() - bytes_overflow, 0);
    if (mesg > 0) {
      bytes_overflow += mesg;
    }
    if (mesg == 0) {
      throw std::runtime_error("client disconnected");
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      break;
    }
    if (mesg < 0) {
      throw std::runtime_error("client disconnected");
    }
  }
}
