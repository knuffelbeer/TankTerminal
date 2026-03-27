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

Reader::Reader() {
  buffer_dynamic = std::vector<uint32_t>(BUFFERSIZE);
  buffer = (char *)&buffer_data;
  overflow = (char *)&overflow_data;
}

int Reader::make_buf(int start, int socket) {
  if (start > BUFFERSIZE) {
    abort();
  }
  int num_bytes{start};
  while (num_bytes < sizeof(Message)) {
    int mesg = recv(socket, buffer + num_bytes, BUFFERSIZE - num_bytes, 0);
    if (mesg == 0) {
      throw std::runtime_error("connection closed");
    }
    assert(mesg > 0 && "error recieving\n");
    num_bytes += mesg;
  }
  idx = 0;
  Message m = read_single<Message>();
  length = m.size;
  message_type = m.type;
  assert(length <= BUFFERSIZE && "length probably shouldn't exceed BUFFERSIZE");

  if (num_bytes > m.size) {
    memcpy(overflow, buffer + m.size, num_bytes - m.size);
    return num_bytes - m.size;
  }
  if (num_bytes == m.size) {
    return 0;
  }

  while (num_bytes < m.size) {
    int mesg = recv(socket, buffer + num_bytes, BUFFERSIZE - num_bytes, 0);
    if (mesg == 0) {
      throw std::runtime_error("connection closed");
    }
    assert(mesg > 0 && "error recieving\n");
    num_bytes += mesg;
    if (num_bytes > m.size) {
      memcpy(overflow, buffer + m.size, num_bytes - m.size);
      return num_bytes - m.size;
    }
  }
  return 0;
}

void Reader::swap_buffer() {
  char *buf_cpy = buffer;
  buffer = overflow;
  overflow = buf_cpy;
  idx = 0;
}
