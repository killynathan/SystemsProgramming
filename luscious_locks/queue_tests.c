/**
 * Luscious Locks Lab 
 * CS 241 - Fall 2016
 */
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"

int main(int argc, char **argv) {
  queue_t * test = queue_create(5);

  int i;
  for (i = 0; i < 5; i++) {
    void* input1 = malloc(sizeof(int));
    *((int*)input1) = i;
    queue_push(test, input1);
  }
  int j;
  for (j = 0; j < 5; j++) {
    void* output = queue_pull(test);
    printf("%d\n", *((int*)output));
    free(output);
  }
  queue_destroy(test);
  return 0;
}
