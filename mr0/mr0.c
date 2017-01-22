/**
 * Map Reduce 0 Lab
 * CS 241 - Fall 2016
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

int main(int argc, char **argv) {

  //get args
  char * inputFileName = argv[1];
  char * outputFileName = argv[2];
  char * mapperName = argv[3];
  //char * reducerName = argv[4];

  // Open the input file.
  FILE * inputFile = fopen(inputFileName, "r");

  // Open the output file.
  FILE * outputFile = fopen(outputFileName, "w");
  
  // Create a pipe to connect the mapper to the reducer.
  int filedes[2];
  pipe(filedes);

  //fork
  pid_t MapperChild = fork();
  pid_t ReducerChild = fork();
  
  if (MapperChild > 0) {
    int status1;
    waitpid(MapperChild, &status1, 0);
    fclose(inputFile);
  }

  else if (MapperChild == 0) {
    close(filedes[0]);
    execvp(mapperName, &mapperName);
    /*
    char buffer[100];
    while (fgets(buffer, sizeof(buffer), inputFile)) {
      if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = '\0';
      write(filedes[1], buffer, sizeof(buffer));
    }
    */
  }

  if (ReducerChild > 0) {
    int status2;
    //int status3;
    waitpid(ReducerChild, &status2, 0);
    //waitpid(MapperChild, &status3, 0);
    fclose(outputFile);
  }

  else if (ReducerChild == 0) {
    execvp(reducerName, &reducerName);
    /*
    close(filedes[1]);
    char buffer[100];
    while(read(filedes[0], buffer, sizeof(buffer))) {
      printf("%s\n", buffer);
    }
    */
  }

  
  // Start the mapper.
  
  // Start the reducer.

  // Wait for the reducer to finish.

  // Print nonzero subprocess exit codes.

  // Count the number of lines in the output file.

  return 0;
}
