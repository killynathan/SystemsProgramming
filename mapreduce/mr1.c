/**
 * MapReduce
 * CS 241 - Fall 2016
 */

#include "common.h"
#include "utils.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void usage() { print_mr1_usage(); }

int main(int argc, char **argv) {

  //get args
  char *input_file_name = argv[1];
  char *output_file_name = argv[2];
  char *mapper_name = argv[3];
  char *reducer_name = argv[4];
  int mapper_count = atoi(argv[5]);
  char *mapper_count_string = argv[5];
  
  // Create an input pipe for each mapper.
  int i = 0;
  int *mapper_pipes[mapper_count];
  for (; i < mapper_count; i++) {
    mapper_pipes[i] = malloc(2 * sizeof(int));
    pipe2(mapper_pipes[i], O_CLOEXEC);
  }
  
  // Create one input pipe for the reducer.
  int reducer_pipe[2];
  pipe2(reducer_pipe, O_CLOEXEC);
  
  
  // Start a splitter process for each mapper.
  int j = 0;
  pid_t splitter_id[mapper_count];
  for (; j < mapper_count; j++) {
    splitter_id[j] = fork();
    if (splitter_id[j] == -1) {
      //error handling
      puts("error in splti");
      exit(1);
    }
    else if (splitter_id[j] == 0) {

      //set pipe
      dup2(mapper_pipes[j][1], STDOUT_FILENO);

      //exec
      char string_j[10];
      sprintf(string_j, "%d", j);
      //printf("i am child %s. PID: %d\n", string_j, getpid());
      execl("splitter", "splitter", input_file_name, mapper_count_string, string_j, (char*)NULL);
    }
  }
  
  // Start all the mapper processes.
  int k = 0;
  pid_t mapper_id[mapper_count];
  for (; k < mapper_count; k++) {
    mapper_id[k] = fork();
    if (mapper_id[k] == -1) {
      //error handling
      puts("eror in mapper");
      exit(1);
    }
    else if (mapper_id[k] == 0) {
      //printf("i am child %d. PID: %d\n", k, getpid());

      //set pipe
      dup2(mapper_pipes[k][0], STDIN_FILENO);
      dup2(reducer_pipe[1], STDOUT_FILENO);

      //exec
      execl(mapper_name, mapper_name, (char*)NULL);
    }
  }
  
  // Start the reducer process.
  pid_t reducer_id = fork();
  if (reducer_id == -1) {
    //error handling
    puts("error forkign in reducer");
    exit(1);
  }
  else if (reducer_id == 0) { //child
    //set pipe
    dup2(reducer_pipe[0], STDIN_FILENO);
    FILE* f = fopen(output_file_name, "w");
    dup2(fileno(f), STDOUT_FILENO);

    //exec
    execl(reducer_name, reducer_name, (char*)NULL);
  }

  //close fd's
  close(reducer_pipe[0]);
  close(reducer_pipe[1]);

  int q = 0;
  for (; q < mapper_count; q++) {
    close(mapper_pipes[q][0]);
    close(mapper_pipes[q][1]);
  }
   
  // Wait for the reducer to finish.
  int reducer_status;
  waitpid(reducer_id, &reducer_status, 0);
  if (reducer_status != 0) print_nonzero_exit_status(reducer_name, reducer_status);

  int w = 0;
  int splitter_status;
  for (; w < mapper_count; w++) {
    waitpid(splitter_id[w], &splitter_status, 0);
    if (splitter_status != 0) print_nonzero_exit_status("splitter", splitter_status);
  }

  int e = 0;
  int mapper_status;
  for (; e < mapper_count; e++) {
    waitpid(mapper_id[e], &mapper_status, 0);
    if (mapper_status != 0) print_nonzero_exit_status(mapper_name, mapper_status);
  }
  // Print nonzero subprocess exit codes.
  

  // Count the number of lines in the output file.
  print_num_lines(output_file_name);

  int r = 0;
  for (; r < mapper_count; r++) {
    free(mapper_pipes[r]);
  }
  
  return 0;
}
