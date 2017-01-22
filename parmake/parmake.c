/**
 * Parallel Make
 * CS 241 - Fall 2016
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "parmake.h"
#include "parser.h"

int pars_args(int argc, char **argv, char **makefile, int *thread_count, char ***targets) {
  if (argc == 1) {
    return -1;
  }
  int opt;
  while ((opt = getopt(argc, argv, "f:j:")) != -1) {
    switch(opt) {
    case 'f':
      *makefile = optarg;
      break;
    case 'j':
      *thread_count = atoi(optarg);
      break;
    }
  }
  if (argc > 5)
    *targets = argv + 5;
  else
    *targets = NULL;
  
  return 0;
}

void make_parser(rule_t *target) {
  printf("target name: %s\n", target->target);
  size_t i = 0;
  Vector * d = target->dependencies;
  rule_t *rule;
  for (; i < Vector_size(d); i++) {
    rule = (rule_t*)Vector_get(d, i);
    printf("depends on: %s\n", rule->target);
  }
}

// Treat this as main
int parmake(int argc, char **argv) {
  
  // parse args
  char *makefile = NULL;
  int worker_thread_count = 0;
  char **targets = NULL;
  FILE * f;
  if (pars_args(argc, argv, &makefile, &worker_thread_count, &targets) == -1) {
    worker_thread_count = 1;
    f = fopen("./makefile", "r");
    if (f == NULL) {
      f = fopen("./Makefile", "r");
      if (f == NULL) {
	return 1;
      }
      else {
	makefile = "Makefile";
      }
    }
    else {
      makefile = "makefile";
    }
    
  }

  /*printf("makefile: %s, threadcount: %d\n", makefile, thread_count);
  int i = 0;
  for (; i < 2; i++) {
    printf("%s\n", targets[i]);
  }*/

  // process the makefile
  
  parser_parse_makefile(makefile, targets, make_parser);

  
  return 0;
}
