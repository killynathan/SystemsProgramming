/**
 * Machine Problem: Shell
 * CS 241 - Fall 2016
 */

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>

#include "format.h"
#include "log.h"
#include "shell.h"

pid_t id;

int isPrefix (char * str, char * prefix) {
  return strncmp(str, prefix, strlen(prefix)) == 0;
}

void cleanup(int signal) {
  int status;
  waitpid(id, &status, 0);
}

void donothing() {

}

int shell(int argc, char *argv[]) {
  // TODO: This is the entry point for your shell.

  signal(SIGINT, donothing);
  
  int done = 0;
  int shouldgetline = 1;
  pid_t id_main = getpid();
  char * path = strdup(getenv("PWD"));
  
  char *command = NULL;
  size_t len = 0;

  Log * history = NULL;
  Log * file = NULL;
  int hasfile = 0;
  int fileIndex = 0;
  char * historyname = NULL;
  
  //check for extensions
  if (argc == 1) {
    history = Log_create();
  }
  else if (argc == 3) {
    if (!strcmp(argv[1], "-h")) {
      history = Log_create_from_file(argv[2]);
      historyname = strdup(argv[2]);
    }
    else if (!strcmp(argv[1], "-f")) {
      file = Log_create_from_file(argv[2]);
      history = Log_create();
      hasfile = 1;
    }
    else {
      print_usage();
    }
  }
  else if (argc == 5) {
    hasfile = 1;
    if (!strcmp(argv[1], "-h") && !strcmp(argv[3], "-f")) {
      history = Log_create_from_file(argv[2]);
      file = Log_create_from_file(argv[4]);
      historyname = strdup(argv[2]);
    }
    else if (!strcmp(argv[1], "-f") && !strcmp(argv[3], "-h")) {
      history = Log_create_from_file(argv[4]);
      file = Log_create_from_file(argv[2]);
      historyname = strdup(argv[4]);
    }
    else {
      print_usage();
    }
  }
  else {
    print_usage();
  }
  
  
  //loop!!
  while (!done) {
    //loap commands from file
    if (hasfile && (Log_size(file) > 0)) {
      free(command);
      command = strdup(Log_get_command(file, fileIndex));
      fileIndex++;
      if ((size_t)fileIndex >= Log_size(file)) {
	hasfile = 0;
      }
    }
    
    //get command
    else if (shouldgetline) {
      print_prompt(path, id_main);
      if (getline(&command, &len, stdin) == -1) {
	if (historyname != NULL) {
	  puts(historyname);
	  Log_save(history, historyname);
	}
	free(path);
	free(command);
	break;
      }
      char *n1 = strchr(command, '\n');
      if (n1) *n1 = 0;
    }
    shouldgetline = 1;
    
    size_t numtokens = 0;
    char * delim = " ";
    char **tokens = strsplit(command, delim, &numtokens);

    
    //cd
    if (!strcmp(tokens[0], "cd")) {
      Log_add_command(history, command);
      int chdirStatus = 0;
      chdirStatus = chdir(tokens[1]);
      if (chdirStatus == -1) {
	print_no_directory(tokens[1]);
      }
      else {
	free(path);
	path = get_current_dir_name();
      }
    }

    //history (!h)
    else if (!strcmp(tokens[0], "!history")) {
      size_t i = 0;
      for (;i < Log_size(history); i++) {
	print_history_line(i, Log_get_command(history, i));
      }
    }

    //#n
    else if ((tokens[0])[0] == '#' && isdigit(tokens[0][1])) {
      int index = tokens[0][1] - '0';
      if (index < 0 ||(size_t)index >= Log_size(history)) {
	print_invalid_index();
      }
      else {
	free(command);
	command = strdup(Log_get_command(history, index));
	shouldgetline = 0;
	print_command(command);
      }
    }

    //!prefix
    else if (tokens[0][0] == '!') {
      int i = Log_size(history) - 1;
      int index = -1;
      char * temp = NULL;
      for (; i >=0; i--) {
	temp = strdup(Log_get_command(history, i));
	if (isPrefix(temp, tokens[0] + 1)) {
	  index = i;
	  free(temp);
	  break;
	}
	free(temp);
      }
      if (index == -1) {
	print_no_history_match();
      }
      else {
	free(command);
	command  = strdup(Log_get_command(history, index));
	shouldgetline = 0;
	print_command(command);
      }
    }
    
    //external
    else {
      Log_add_command(history, command);
      id = fork();
      if (id == -1) {
	print_fork_failed();
	exit(1);
      }
      else if (id > 0) {
	if (!strcmp(tokens[numtokens - 1], "&") || tokens[numtokens - 1][strlen(tokens[numtokens - 1]) - 1] == '&') {
	  signal(SIGCHLD, cleanup);
	}
	else {
	  int status;
	  if (waitpid(id, &status, 0) == -1) {
	    print_wait_failed();
	    exit(1);
	  };
	}
      }
      else {
	print_command_executed(getpid());
	if (!strcmp(tokens[numtokens - 1], "&")) {
	  tokens[numtokens - 1] = 0;
	}
	else if (tokens[numtokens - 1][strlen(tokens[numtokens - 1]) - 1] == '&') {
	  tokens[numtokens - 1][strlen(tokens[numtokens - 1]) - 1] = '\0';
	}
	execvp(tokens[0], tokens);
	//check fail
	print_exec_failed(tokens[0]);
	exit(1);
      }
    }
  }

  
  return 0;
}
