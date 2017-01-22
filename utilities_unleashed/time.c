/**
 * Lab: Utilities Unleashed
 * CS 241 - Fall 2016
 */

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "format.h"

int main(int argc, char *argv[]) {
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  pid_t id = fork();
  if (id == -1) {
    print_fork_failed();
  }
  else if (id > 0) {
    //in parent
    int status;
    if (waitpid(id, &status, 0) != -1) {
      if (status != 0) exit(1);
      clock_gettime(CLOCK_MONOTONIC, &end);
      double test = ((end.tv_nsec - start.tv_nsec))/1e9;
      double diff =  (end.tv_sec - start.tv_sec) + test;
      //printf("elapsed time = %f seconds\n", diff);
      display_results(argv, diff);
    }
    else {
      exit(1);
    }
  }
  else {
    //in child
    if (execvp(argv[1], argv + 1) == -1) {
      print_exec_failed();
      exit(1);
    }
    //need to check for errors
  }

  
  return 0;
}
