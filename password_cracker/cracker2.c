/**
 * Machine Problem: Password Cracker
 * CS 241 - Fall 2016 hi
 */

#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
int still_tasks_left = 1;
int pw_found = 0;

char * current_task = NULL;
char * current_username = NULL;
char * current_solution = NULL;
char * current_pw = NULL;
char * pw_with_a = NULL;
size_t workload_per_thread = 0;
size_t thread_c;
int num_unknowns;
int prefix_length;

//global summary statuses
char * pw = NULL;
long total_count = 0;
double start_time;
double end_time;
double CPU_start;
double CPU_end;
int result = 1;

pthread_barrier_t myBarrier;
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;

char * replace_period_with_a(char * str) {
  char * tmp = strdup(str);
  size_t i = 0;
  for (; i < strlen(tmp); i++) {
    if (tmp[i] == '.') tmp[i] = 'a';
  }
  return tmp;
}

int get_and_init_next_task() {
  char buffer[100];
  if (fgets(buffer, 100, stdin)) {
    if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = '\0';

    free(current_task);
    free(pw_with_a);
   
    char * saveptr = NULL;
    current_task = strdup(buffer);
    current_username = strtok_r(current_task, " ", &saveptr);
    current_solution = strtok_r(NULL, " ", &saveptr);
    current_pw = strtok_r(NULL, " ", &saveptr);
    num_unknowns = (int)(strlen(current_pw) - getPrefixLength(current_pw));
    prefix_length = getPrefixLength(current_pw);
    pw_with_a = replace_period_with_a(current_pw);
    return 1;
  }
  free(current_task);
  free(pw_with_a);
  return 0;
}

void * crack(void * _index) {
  size_t index = (size_t)_index;
  long start_index;
  long count;
  char * starting_string = NULL;

  double total_time;
  double total_CPU_time;
  
  while (still_tasks_left) {
    //free(starting_string);
    getSubrange(num_unknowns, thread_c, index, &start_index, &count);
    starting_string = strdup(pw_with_a);
    setStringPosition(starting_string + prefix_length, start_index);
    int status = 2;
    int hash_count = 0;
    char * hash = NULL;
    struct crypt_data cdata;
    cdata.initialized = 0;
    
    if (index == 1) {
      start_time = getTime();
      CPU_start = getCPUTime();
      v2_print_start_user(current_username);
    }
    pthread_barrier_wait(&myBarrier);
    
    v2_print_thread_start(index, current_username, start_index, starting_string);
    
    int i = 0;
    for (; i < count; i++) {
      if (pw_found) {
	status = 1;
	break;
      }
      hash = crypt_r(starting_string, "xx", &cdata);
      hash_count++;
      if (strcmp(hash, current_solution) == 0) {
	pw_found = 1;
	status = 0;
	pw = strdup(starting_string);
	result = 0;
	break;
      }
      incrementString(starting_string + prefix_length);
      //free(hash);
    }

    free(starting_string);
    pthread_barrier_wait(&myBarrier);
    pw_found = 0;
    v2_print_thread_result(index, hash_count, status);

    pthread_mutex_lock(&myMutex);
    total_count += hash_count;
    pthread_mutex_unlock(&myMutex);
    
    pthread_barrier_wait(&myBarrier);
    if (index == 1) {
      end_time = getTime();
      CPU_end = getCPUTime();

      total_time = end_time - start_time;
      total_CPU_time = CPU_end - CPU_start;
      v2_print_summary(current_username, pw, total_count, total_time, total_CPU_time, result);
      free(pw);
      pw = NULL;
      result = 1;
      total_count = 0;
      if (!get_and_init_next_task()) {
	still_tasks_left = 0;
      }
    }
    pthread_barrier_wait(&myBarrier);
  }
  
  return NULL;
}

int start(size_t thread_count) {
  // TODO your code here, make sure to use thread_count!
  thread_c = thread_count;
  pthread_barrier_init(&myBarrier, NULL, thread_count);
  pthread_t threads[thread_count];
  get_and_init_next_task();
  
  size_t i = 0;
  for (; i < thread_count; i++) {
    pthread_create(threads + i, NULL, crack, (void*)(i + 1));
  }

  size_t j = 0;
  for (; j < thread_count; j++) {
    pthread_join(threads[j], NULL);
  }
  
  return 0;
}
