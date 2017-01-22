/**
 * Chatroom Lab
 * CS 241 - Fall 2016
 */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#include "utils.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
  int name_len = strlen(name);
  int msg_len = strlen(message);
  char *msg = calloc(1, msg_len + name_len + 4);
  sprintf(msg, "%s: %s", name, message);

  return msg;
}

ssize_t get_message_size(int socket) {
  int32_t size;
  ssize_t read_bytes =
      read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
  if (read_bytes == 0 || read_bytes == -1)
    return read_bytes;

  return (ssize_t)ntohl(size);
}

ssize_t write_message_size(size_t size, int socket) {
  // Your code here
  int32_t s = htonl(size);
  ssize_t written_bytes = write_all_to_socket(socket, (char*)(&s), sizeof(int32_t));
  if (written_bytes == 0 || written_bytes == -1) return written_bytes;
  return written_bytes;
}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
  // Your Code Here
  ssize_t len = 0;
  ssize_t curr = 0;
  
  while ((size_t)len < count) {
    curr = read(socket, buffer + len, count - len);

    if (curr == 0) { // connection closed
      fprintf(stderr, "closed");
      return 0;
    }

    else if (curr == -1 && errno != EINTR) { //failed
      fprintf(stderr, "failed");
      return -1;
    }

    else if (curr == -1 && errno == EINTR) { //interrupted
      fprintf(stderr, "itnerrupt");
      len += 0;
    }

    else {
      len += curr;
    }
    
  }	   
	   
  return len;
}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
  // Your Code Here
  ssize_t len = 0;
  ssize_t curr = 0;

  while ((size_t)len < count) {
    curr = write(socket, buffer + len, count - len);

    if (curr == 0) { // connection closed
      fprintf(stderr, "close 2");
      return 0;
    }

    else if (curr == -1 && errno != EINTR) { //failed
      fprintf(stderr, "fail 2");
      return -1;
    }

    else if (curr == -1 && errno == EINTR) { //interrupted
      fprintf(stderr, "interrrupt 2");
      len += 0;
    }

    else {
      len += curr;
    }

  }
  
  return len;
}
