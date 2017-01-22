/**
 * MapReduce
 * CS 241 - Fall 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "utils.h"

void usage() {
  fprintf(stderr, "shuffler destination1 destination2 ...\n");
  fprintf(stderr, "where destination1..n are files on the filesystem to which "
                  "the shuffler will write its output\n");
}

int main(int argc, char *argv[]) {
  // read from stdin
  // hash the key for the input line
  // send them to the correct output file (output files are given as command
  // line arguments
  if (argc < 2) {
    usage();
    exit(1);
  }

  int reducer_pipes[argc - 1];
  int i = 0;
  for (; i < argc - 1; i++) {
    reducer_pipes[i] = open(argv[i + 1], O_WRONLY);
  }
  
  char *line = NULL;
  size_t len = 0;
  ssize_t read = 0;
  int pipe;
  int N = argc - 1;
  
  while ((read = getline(&line, &len, stdin)) != -1) {
    char *key;
    char *value;
    split_key_value(line, &key, &value);
    pipe = reducer_pipes[hashKey(key) % N];
    dprintf(pipe, "%s: %s\n", key, value);
  }

  int j = 0;
  for (; j < argc - 1; j++) {
    close(reducer_pipes[j]);
  }
  
  return 0;
}
