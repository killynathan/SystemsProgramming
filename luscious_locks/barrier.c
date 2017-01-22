/**
 * Luscious Locks Lab 
 * CS 241 - Fall 2016
 */
#include "barrier.h"
#include <stdio.h>
// The returns are just for errors if you want to check for them.
int barrier_destroy(barrier_t *barrier) {
  int error = 0;
  pthread_mutex_destroy(&barrier->mtx);
  pthread_cond_destroy(&barrier->cv);
  return error;
}

int barrier_init(barrier_t *barrier, unsigned int num_threads) {
  int error = 0;
  pthread_mutex_init(&barrier->mtx, NULL);
  pthread_cond_init(&barrier->cv, NULL);
  barrier->n_threads = num_threads;
  barrier->count = 1;
  barrier->times_used = 0;
  return error;
}

int barrier_wait(barrier_t *barrier) {
  //lock
  //increment times used
  //check if reached max
  //if so wake up all
  //if did not reach max, sleep
  //unlock
  unsigned int max = barrier->n_threads * barrier->count;
  pthread_mutex_lock(&barrier->mtx);
  barrier->times_used++;
  if (barrier->times_used == max) {
    barrier->count++;
    pthread_cond_broadcast(&barrier->cv);
  }
  else {
    while(barrier->times_used < max) {
      pthread_cond_wait(&barrier->cv, &barrier->mtx);
    }
  }
  pthread_mutex_unlock(&barrier->mtx);
  return 0;
}
