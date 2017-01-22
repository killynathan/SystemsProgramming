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
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

void usage() { print_mr2_usage(); }

int main(int argc, char **argv) {
  
  // get args
  char *input_file_name = argv[1];
  char *output_file_name = argv[2];
  char *mapper_name = argv[3];
  char *reducer_name = argv[4];
  char *mapper_count_string = argv[5];
  int mapper_count = atoi(argv[5]);
  //char *reducer_count_string = argv[6];
  int reducer_count = atoi(argv[6]);
  
  // setup mapper pipes
  int *mapper_pipes[mapper_count];
  int i = 0;
  for (; i < mapper_count; i++) {
    mapper_pipes[i] = malloc(2 * sizeof(int));
    pipe2(mapper_pipes[i], O_CLOEXEC);
  }

  // setup shuffler pipe
  int shuffler_pipe[2];
  pipe2(shuffler_pipe, O_CLOEXEC);

  
  // setup reducer FIFO pipe
  char *reducer_pipe_names[reducer_count];
  int reducer_pipes[reducer_count]; 
  int j = 0;
  for (; j < reducer_count; j++) {
    reducer_pipe_names[j] = malloc(9);
    sprintf(reducer_pipe_names[j], "./fifo_%d", j);
    mkfifo(reducer_pipe_names[j], S_IRWXU);
    //reducer_pipes[j] = open(reducer_pipe_names[j], O_RDONLY);
  }
  

  // start splitters
  int k = 0;
  pid_t splitter_ids[mapper_count];
  for (; k < mapper_count; k++) {
    splitter_ids[k] = fork();
    if (splitter_ids[k] == -1) {
      puts("error for split");
      exit(1);
    }

    else if (splitter_ids[k] == 0) {
      dup2(mapper_pipes[k][1], STDOUT_FILENO);
      char string_k[10];
      sprintf(string_k, "%d", k);
      execl("splitter", "splitter", input_file_name, mapper_count_string, string_k, (char*)NULL);
    }
  }
  
  // start mappers
  int l = 0;
  pid_t mapper_ids[mapper_count];
  for (; l < mapper_count; l++) {
    mapper_ids[l] = fork();
    if (mapper_ids[l] == -1) {
      puts("error in map");
      exit(1);
    }

    else if (mapper_ids[l] == 0) {
      dup2(mapper_pipes[l][0], STDIN_FILENO);
      dup2(shuffler_pipe[1], STDOUT_FILENO);
      execl(mapper_name, mapper_name, (char*)NULL);
    }
  }
  /*
  //free memory pipe memory
  int d = 0;
  for (; d < mapper_count; d++) {
    free(mapper_pipes[d]);
  }
  */
  // start shuffler
  pid_t shuffler_id = fork();
  if (shuffler_id == -1) {
    puts("shuffler error");
    exit(1);
  }

  else if (shuffler_id == 0) {
    dup2(shuffler_pipe[0], STDIN_FILENO);

    char *shuffler_args[reducer_count + 2];
    shuffler_args[0] = "shuffler";
    int a = 1;
    for (; a <= reducer_count; a++) {
      shuffler_args[a] = reducer_pipe_names[a - 1];
    }
    shuffler_args[reducer_count + 1] = NULL;
    
    execv(shuffler_args[0], shuffler_args);
    puts("FAIL");
  }
 
  // start reducers
  int m = 0;
  pid_t reducer_ids[reducer_count];
  int fd = open(output_file_name, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
  for (; m < reducer_count; m++) {
    reducer_ids[m] = fork();
    if (reducer_ids[m] == -1) {
      puts("reducer fail!");
      exit(1);
    }

    else if (reducer_ids[m] == 0) {

      reducer_pipes[m] = open(reducer_pipe_names[m], O_RDONLY);
      
      dup2(reducer_pipes[m], STDIN_FILENO);
      dup2(fd, STDOUT_FILENO);
      execl(reducer_name, reducer_name, (char*)NULL);
      perror("reducer fail");
    }
  }

  /*
  //free reducer pipe name memory
  int v = 0;
  for (; v < reducer_count; v++) {
    free(reducer_pipe_names[v]);
  }
  */

  //close fds
  close(shuffler_pipe[0]);
  close(shuffler_pipe[1]);

  int x = 0;
  for (; x < mapper_count; x++) {
    close(mapper_pipes[x][0]);
    close(mapper_pipes[x][1]);
  }

  /*
  int y = 0;
  for (; y < reducer_count; y++) {
    close(reducer_pipes[y]);
  }
  */

  // wait for everything to finish
  int shuffler_status;
  waitpid(shuffler_id, &shuffler_status, 0);
  if (shuffler_status != 0) print_nonzero_exit_status("shuffler", shuffler_status);

  int splitter_status;
  int b = 0;
  for (; b < mapper_count; b++) {
    waitpid(splitter_ids[b], &splitter_status, 0);
    if (splitter_status != 0) print_nonzero_exit_status("splitter", splitter_status);
  }
  
  int mapper_status;
  int a = 0;
  for (; a < mapper_count; a++) {
    waitpid(mapper_ids[a], &mapper_status, 0);
    if (mapper_status != 0) print_nonzero_exit_status(mapper_name, mapper_status);
  }

  int reducer_status;
  int c = 0;
  for (; c < reducer_count; c++) {
    waitpid(reducer_ids[c], &reducer_status, 0);
    if (reducer_status != 0) print_nonzero_exit_status(reducer_name, reducer_status);
  }
  
  // count lines
  print_num_lines(output_file_name);
  
  //free memory
  
  int d = 0;
  for (; d < mapper_count; d++) {
    free(mapper_pipes[d]);
  }
  
  //free reducer pipe name memory
  int v = 0;
  for (; v < reducer_count; v++) {
    free(reducer_pipe_names[v]);
  }
  
  
  
}
