#include "../../include/client/reader.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>


Reader::Reader() {
  buffer = (char *)&buffer_data;
  overflow = (char *)&overflow_data;
}

int Reader::make_buf(int start, int socket) {

  if (start > BUFFERSIZE) {
    printf("starting after BUFFERSIZE!");
    abort();
  }
  int num_bytes{start};
  while (num_bytes < sizeof(Message)) {
    printf(
        "while num_bytes< sizeof(Message) num_bytes: %i sizeof(Message): %i \n",
        num_bytes, (int)sizeof(Message));
    int mesg = recv(socket, buffer + num_bytes, BUFFERSIZE - num_bytes, 0);
    if (mesg < 0) {
      printf("error recieving\n");
      throw;
    }
    if (mesg == 0) {
      printf("connection closed\n");
      throw;
    }
    num_bytes += mesg;
  }
  idx = 0;
  Message m = read_single<Message>();
  length = m.size;
	message_type = m.type;
  printf("message recieved. num_bytes=%i m.size=%i, mtype=%i\n", num_bytes,
         m.size, m.type);
  if (length > BUFFERSIZE) {
    printf("start %i", start);
    printf("start %i", m.type);
    printf("length probably shouldn't exceed BUFFERSIZE: %i", length);
    throw;
  }

  if (num_bytes > m.size) {
    printf("bytes > size  num_bytes=%i m.size=%i overflow=%i \n", num_bytes,
           m.size, num_bytes - m.size);
    memcpy(overflow, buffer + m.size, num_bytes - m.size);
    return num_bytes - m.size;
  }
  if (num_bytes == m.size) {
    printf("bytes == size\n");

    return 0;
  }

  while (num_bytes < m.size) {
    printf("iter2 num_bytes < m.size num_bytes %i m.size %i\n", num_bytes,
           (int)m.size);

    int mesg = recv(socket, buffer + num_bytes, BUFFERSIZE - num_bytes, 0);
    if (mesg < 0) {
      printf("error recieving");
      throw;
    }
    if (mesg == 0) {
      printf("connection closed");
      throw;
    }
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
