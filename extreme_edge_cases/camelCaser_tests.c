/**
 * Chatroom Lab
 * CS 241 - Fall 2016
 */
#include "camelCaser_tests.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"

/*
 * Testing function for various implementations of camelCaser.
 *
 * @param  camelCaser   A pointer to the target camelCaser function.
 * @return              Correctness of the program (0 for wrong, 1 for correct).
 */



void print_output(char **output){
  printf("Got the following:\n");
  if(output){
    char ** line = output;
    while(*line){
      printf("\t'%s'\n", *line);
      line++;
    }
  } else{
    printf("NULL POINTER!\n");
  }
  printf("-----------------------------------------------------------------------------------------\n");
}

void print_input(char * input){
  printf("testing: \n\t'%s'\n\n", input);
}

int test_camelCaser(char **(*camelCaser)(const char *)) {
  // TODO: Return 1 if the passed in function works properly; 0 if it doesn't.
  
  char * testingStr = NULL;
  char ** output = NULL;
  int success = 1;
  
  //basic
  testingStr = "Hello. Bye.";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "hello") || strcmp(output[1], "bye")) success = 0;
  
  testingStr = "123.";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "123")) success = 0;
  
  //werid punct
  testingStr = "...";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "") || strcmp(output[1], "") || strcmp(output[2], "")) success = 0;

  testingStr = " Hello World.. I am NATHAN.";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "helloWorld") || strcmp(output[1], "") || strcmp(output[2], "iAmNathan")) success = 0;

  testingStr = "Hello my name is nathan";
  output = camelCaser(testingStr);
  if (output[0] != NULL) success = 0;

  testingStr = ". hi";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "")) success = 0;

  testingStr = " .hi.";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "") || strcmp(output[1], "hi")) success = 0;

  testingStr = "hi.hi";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "hi")) success = 0;

  testingStr = "a^]b!%c.;";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "a") || strcmp(output[1], "") || strcmp(output[2], "b") || strcmp(output[3], "") || strcmp(output[4], "c") || strcmp(output[5], ""))   success = 0;
  
  //EMPTY
  testingStr = " .";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "")) success = 0;

  testingStr = ".";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "")) success = 0;

  testingStr = ". ";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "")) success = 0;

  testingStr = " .";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "")) success = 0;

  testingStr = " ";
  output = camelCaser(testingStr);
  if (output == NULL) success = 0;

  testingStr = "";
  output = camelCaser(testingStr);
  if (output == NULL) success = 0;

  testingStr = "\n";
  output = camelCaser(testingStr);
  if (output == NULL) success = 0;

  testingStr = NULL;
  output = camelCaser(testingStr);
  if (output != NULL) success = 0;
  
  return success;
}

