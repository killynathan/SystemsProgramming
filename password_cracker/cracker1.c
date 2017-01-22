/**
 * Machine Problem: Password Cracker
 * CS 241 - Fall 2016
 */

#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include <crypt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "queue.h"
#include "thread_status.h"

int stillTasksLeft = 1;
int success = 0;
int fail = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

typedef struct _threadParams {
  queue_t * q;
  int index;
} threadParams;

int queue_init(queue_t * q) {
  char buffer[50];
  int size = 0;
  while (fgets(buffer, 50, stdin)) {
    if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = '\0';
    char *temp = strdup(buffer);
    queue_push(q, temp);
    size++;
  }
  char * end = "END";
  queue_push(q, end);
  return size;
}

void replacePeriodsWitha(char * s) {
  size_t i = 0;
  for (; i < strlen(s); i++) {
    if (s[i] == '.') {
      s[i] = 'a';
    }
  }
}

//clean memory for hashes
int findMatch(char * pw, char * suffix, char *solution, int * result) {
  struct crypt_data cdata;
  cdata.initialized = 0;
  int keepIncrementing = 1;
  int count = 0;
  char * hash = NULL;
  while (keepIncrementing) {
    hash = crypt_r(pw, "xx", &cdata);
    count++;
    if (strcmp(hash, solution) == 0) {
      *result = 0;
      pthread_mutex_lock(&m);
      success++;
      pthread_mutex_unlock(&m);
      return count;
    }
    keepIncrementing = incrementString(suffix);
  }
  *result = 1;
  pthread_mutex_lock(&m);
  fail++;
  pthread_mutex_unlock(&m);
  return count;
}

void do_task(queue_t * q, int index) {
  double startTime = getThreadCPUTime();
  int result;
  char *task = (char*)queue_pull(q);
  if (strcmp(task, "END") == 0) {
    stillTasksLeft = 0;
    queue_push(q,"END");
    return;
  }
  char *saveptr;
  char *username = strtok_r(task, " ", &saveptr);
  char *hash = strtok_r(NULL, " ", &saveptr);
  char *pw = strtok_r(NULL, " ", &saveptr);
  int prefixLength = getPrefixLength(pw);
  char *pwSuffix = pw + prefixLength;
  v1_print_thread_start(index, username);
  
  replacePeriodsWitha(pwSuffix);
  int count = findMatch(pw, pwSuffix, hash, &result);
  double endTime = getThreadCPUTime();
  double timeElapsed = endTime - startTime;
  v1_print_thread_result(index, username, pw, count, timeElapsed, result);
  free(task);
}


void *crack(void * params) {
  threadParams * tp = (threadParams*)params;
  tp->index = tp->index;
  
  while (stillTasksLeft) {
    threadStatusSet("waiting for task");
    do_task(tp->q, tp->index);
  }
  threadStatusSet("exited while loop");
  return NULL;
}


int start(size_t thread_count) {
  // TODO your code here, make sure to use thread_count!
  //create thread pool
  //for each trhead pull task until no more then send signal to exit
  //join that thread in this main thread
  
  queue_t * q = queue_create(1000);
  pthread_t threads[thread_count];
  threadParams * tp[thread_count];
    
  size_t j = 0;
  for (; j < thread_count; j++) {
    tp[j] = malloc(sizeof(threadParams));
    tp[j]->q = q;
    tp[j]->index = j + 1;
    pthread_create(threads + j, NULL, crack, (void*)tp[j]);
  }
  
  queue_init(q);

  size_t k = 0;
  for (; k < thread_count; k++) {
    pthread_join(threads[k], NULL);
  }

  v1_print_summary(success, fail);
  
  queue_destroy(q);
  size_t a = 0;
  for (; a < thread_count; a++) {
    free(tp[a]);
  }
  return 0;
}
