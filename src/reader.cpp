#include "../include/reader.h"
#include "../include/position.h"
#include <cassert>
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

int Reader::make_buf(int bytes_overflow, int socket) {
  int bytes_message = sizeof(Message);
  bool mesg_read = false;
  Message m;
  if (idx) {
    if (bytes_overflow >= sizeof(Message)) {
      m = read_single<Message>();
      mesg_read = true;
      length = m.size;
      message_type = m.type;
      if (length >= buffer.size()) {
        buffer.resize(length);
      }
      if (bytes_overflow >= length) {
        return bytes_overflow - length;
      } else {
        std::memcpy(buffer.data(), buffer.data() + idx + bytes_message,
                    bytes_overflow - bytes_message);
        idx = 0;
      }
    } else {
      std::memcpy(buffer.data(), buffer.data() + idx, bytes_overflow);
      idx = 0;
    }
  }

  while (bytes_overflow < sizeof(Message)) {
    int mesg = recv(socket, buffer.data() + bytes_overflow,
                    buffer.size() - bytes_overflow, 0);
    if (mesg == 0) {
      throw std::runtime_error("connection closed");
    }
    assert(mesg > 0 && "error recieving\n");
    bytes_overflow += mesg;
  }

  if (!mesg_read) {
    m = read_single<Message>();
    length = m.size;
    message_type = m.type;
    if (length >= buffer.size()) {
      buffer.resize(length);
    }
    if (bytes_overflow >= m.size) {
      return bytes_overflow - m.size;
    }
  }

  while (bytes_overflow < m.size) {
    int mesg = recv(socket, buffer.data() + bytes_overflow,
                    buffer.size() - bytes_overflow, 0);
    if (mesg == 0) {
      throw std::runtime_error("connection closed");
    }
    assert(mesg > 0 && "error recieving\n");
    assert(bytes_overflow <= buffer.size() &&
           "length shouldn't exceed size of dynamic buffer!");
    bytes_overflow += mesg;
    if (bytes_overflow > m.size) {
      return bytes_overflow - m.size;
    }
  }
  return 0;
}
