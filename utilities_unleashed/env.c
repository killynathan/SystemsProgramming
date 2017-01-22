/**
 * Lab: Utilities Unleashed
 * CS 241 - Fall 2016
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "format.h"

char * deref(char * str) {
  char * buffer = malloc(10000); //strlen
  buffer[0] = '\0';
  char * env = malloc(strlen(str) + 1);
  env[0] = '\0';
  int isEnv = 0;
  char * ptr = str;
  int bufferIndex = 0;
  int envIndex = 0;
  
  while (*ptr) {
    if (*ptr == '%') {
      ptr++;
      isEnv = 1;
      while (isEnv && (isdigit(*ptr) || isalpha(*ptr) || *ptr == '_')) {
	env[envIndex] = *ptr;
	ptr++;
	envIndex++;
      }
      isEnv = 0;
      char * check = getenv(env);
      //printf("%s\n", check);
      if (check != NULL) {
	strcat(buffer, check);
	bufferIndex += strlen(check);
      }
      free(env);
      env = calloc(1000, 1);
      envIndex = 0;
    }
    else {
      buffer[bufferIndex] = *ptr;
      ptr++;
      bufferIndex++;
    }
  }
  buffer[bufferIndex] = '\0'; //dont need strcat will do
  return buffer; // return buffer
}

extern char ** environ;
int main(int argc, char *argv[]) {

  //check argc and print
  //make buffer
  if (argc == 1) {
    char ** ptr = environ;
    while (*ptr) {
      printf("%s\n", *ptr);
      ptr++;
    }
  }

  else if (argc == 2) {
    print_env_usage();
  }
  
  else {
    //setenv("SECRET", "55555", 1);
    char* inputStr = argv[1];
    char* bigToken = NULL;
    char* smallToken = NULL;
    char* envName = NULL;
    char* envValue = NULL;
    while (1) {
      bigToken = strsep(&inputStr, ",");
      if (!bigToken) break;
      int i;
      for (i = 0; i < 2; i++) {
	smallToken = strsep(&bigToken, "=");
	if (i == 0) {
	  envName = smallToken;
	}
	else {
	  envValue = smallToken;
	  envValue = deref(envValue);
	  if (setenv(envName, envValue, 1) == -1) {
	    print_environment_change_failed();
	  }
	}
      }
    }
  }
  
  pid_t id = fork();
  if (id == -1) {
    print_fork_failed();
    exit(1);
  }
  if (id > 0) {
    int status;
    if (waitpid(id, &status, 0) != -1) { //chck status  wait just to kill child
      
    }
  }
  else {
    if (argc > 2) {
      //update environment
      //call exec
      execvp(argv[2], argv + 2);
      print_exec_failed();
    }
  }
  return 0;
}
