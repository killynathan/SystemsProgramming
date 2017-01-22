#include <stdio.h>
#include <stdlib.h>
#include "camelCaser.h"
#include <string.h>

#define camelCaser camel_caser

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

int main() {
  /* char * inputs[] = {
      //basic
      "Hello. Bye.",
      //weird punctuation
      "...",
      " Hello World.. I am NATHAN.",
      "Hello my name is nathan",
      ".Hello& I am Nathan!",
      ". hi",
      " .hi.",
      "hi.hi",
      "a^]b!%c.;",
      //empty
      " .",
      ".",
      ". ",
      " ",
      "",
      NULL
    };

    char ** input = inputs;
    while(*input){
        print_input(*input);
        char **output = camel_caser(*input);
        print_output(output);
        input++;
    }
    return 0;*/

  char * testingStr = NULL;
  char ** output = NULL;

  //basic
  testingStr = "Hello. Bye.";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "hello") || strcmp(output[1], "bye")) return 0;

  testingStr = "123.";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "123")) return 0;

  //werid punct
  testingStr = "...";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "") || strcmp(output[1], "") || strcmp(output[2], "")) return 0;

  testingStr = " Hello World.. I am NATHAN.";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "helloWorld") || strcmp(output[1], "") || strcmp(output[2], "iAmNathan")) return 0;

  testingStr = "Hello my name is nathan";
  output = camelCaser(testingStr);
  if (output[0] != NULL) return 0;

  testingStr = ". hi";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "")) return 0;

  testingStr = " .hi.";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "") || strcmp(output[1], "hi")) return 0;

  testingStr = "hi.hi";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "hi")) return 0;

  testingStr = "a^]b!%c.;";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "a") || strcmp(output[1], "") || strcmp(output[2], "b") || strcmp(output[3], "") || strcmp(output[4], "c") || strcmp(output[5], "")) return 0;

  //EMPTY
  testingStr = " .";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "")) return 0;

  testingStr = ".";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "")) return 0;

  testingStr = ". ";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "")) return 0;

  testingStr = " .";
  output = camelCaser(testingStr);
  if (strcmp(output[0], "")) return 0;

  testingStr = " ";
  output = camelCaser(testingStr);
  if (output == NULL) return 0;

  testingStr = "";
  output = camelCaser(testingStr);
  if (output == NULL) return 0;

  testingStr = "\n";
  output = camelCaser(testingStr);
  if (output == NULL) return 0;
  
  
  testingStr = NULL;
  output = camelCaser(testingStr);
  if (output != NULL) return 0;

  puts("success");
  return 1;
}
