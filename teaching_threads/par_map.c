/**
 * Parallel Map Lab
 * CS 241 - Fall 2016
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "map.h"
#include "mappers.h"

/* You should create a struct that will get passed in by reference to your
 * start_routine. */

/* You should create a start routine for your threads. */

typedef struct _threadParams {
  mapper map_func;
  int startValue;
  int listSize;
  int threadSize;
  double * list;
  double * result;
} threadParams;

void *assignElements(void * params) {
  threadParams * ptr = (threadParams*)params;
  int i = ptr->startValue;
  while (i < ptr->listSize) {
    ptr->result[i] = ptr->map_func(ptr->list[i]);
    i += ptr->threadSize;
  }
  return NULL;
}

double *par_map(double *list, size_t list_len, mapper map_func,
                size_t num_threads) {
  /* Your implementation goes here */
  /*
        the thread will call map_func.
          the thread will need to know the item in the list, the function to be called, and the resulting list to assign to\

    how to assign threads:
      i can either loop through threads and assign list element to them OR
      i can loop through elements and assing threads to them

      while there are still elements in the list:
        for loop through threads and run them 
	join all of them
	continue looping through until no more elements in list
  
    partition list into list_len / num_threads 
    eatch thread will take care of its partition
   */

  pthread_t threads[num_threads];

  double * result = malloc(sizeof(double) * list_len);

  int i;

  for (i = 0; (size_t)i < num_threads; i++) {
    threadParams * tp = malloc(sizeof(threadParams));
    tp->result = result;
    tp->map_func = map_func;
    tp->startValue = i;
    tp->listSize = list_len;
    tp->threadSize = num_threads;
    tp->list = list;
    pthread_create(threads + i, NULL, assignElements, (void *)tp);
  }
  
  return result;
}
