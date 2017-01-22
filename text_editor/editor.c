/**
 * Machine Problem: Text Editor
 * CS 241 - Fall 2016
 */

#include "document.h"
#include "editor.h"
#include "format.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int isValidLine(Document *document, int lineNumber) {
  if (lineNumber < 1 || (size_t)lineNumber > Document_size(document)) {
    return 0;
  }
  else {
    return 1;
  }
}

char *get_filename(int argc, char *argv[]) {
  // TODO implement get_filename
  // take a look at editor_main.c to see what this is used for
  return argv[1];
}

void handle_display_command(Document *document, const char *command) {
  // TODO implement handle_display_command
  //int start = 0;
  //int end = 0;
  size_t docLength = Document_size(document);
  
  //case empty doc
  if (docLength == 0) {
    print_document_empty_error();
  }
  
  //case: nonempty doc
  else {
    //case: only p
    if (strlen(command) == 1) {
      size_t i;
      for (i = 1; i <= docLength; i++) {
	print_line(document, i);
      }
    }

    //case: p with line number
    else {
      long int lineNumber = strtol(command + 2, NULL, 10);
      //case: line nubmer out of bounds
      if (!isValidLine(document, lineNumber)) {
	invalid_line();
      }
      else {
	int start = lineNumber - 5;
	int end = lineNumber + 5;
	if (lineNumber < 6) start = 1;
	if ((size_t)lineNumber > docLength - 5) end = docLength;
	int j;
	for (j = start; j <= end; j++) {
	  print_line(document, j);
	}
      }
    }
  }
}

int getIndexOfSecondSpace(char * str) {
  int count = 0;
  int index = 0;
  char * ptr = str;
  while (*ptr) {
    if (*ptr == ' ') {
      count++;
      if (count == 2) {
	return index;
      }
    }
    index++;
    ptr++;
  }
  return -1;
}

int getIndexOfInput(char * str) {
  int index = 1;
  char * ptr = str + 1;
  while (*ptr) {
    if (isalpha(*ptr)) {
      return index;
    }
    index++;
    ptr++;
  }
  return -1;
}

int getIndexOfDollarSign(char * str) {
  char * ptr = str;
  int index = 0;
  
  while (*ptr) {
    if (*ptr == '$') {
      return index;
    }
    index++;
    ptr++;
  }
  return -1;
}

void handle_write_command(Document *document, const char *command) {
  // get line number
  char * stringLineNumber = strdup(command + 2);
  stringLineNumber[getIndexOfSecondSpace(stringLineNumber)] = '\0';
  long int lineNumber = strtol(stringLineNumber, NULL, 10);

  free(stringLineNumber);
  stringLineNumber = NULL;

  char * commandTemp = strdup(command);
  int indexOfDollarSign = getIndexOfDollarSign(commandTemp);

  //case: new line
  if ((size_t)lineNumber > Document_size(document)) {
    //case: no $
    if (indexOfDollarSign == -1) {
      Document_insert_line(document, lineNumber, commandTemp + getIndexOfSecondSpace(commandTemp) + 1);
    }
    //case: with $
    else {
      commandTemp[indexOfDollarSign] = '\0';
      Document_insert_line(document, lineNumber, commandTemp + getIndexOfSecondSpace(commandTemp) + 1);
      Document_insert_line(document, lineNumber + 1, commandTemp + indexOfDollarSign + 1);
    }
  }
  //case: valid line
  else {
    //char * commandTemp = strdup(command);
    //int indexOfDollarSign = getIndexOfDollarSign(commandTemp);
    //case: has dollar sign.
    if (indexOfDollarSign != -1) {
      commandTemp[indexOfDollarSign] = '\0';
      Document_set_line(document, lineNumber, commandTemp + getIndexOfSecondSpace(commandTemp) + 1);
      Document_insert_line(document, lineNumber + 1, commandTemp + indexOfDollarSign + 1);
    }
    //case: doesn't have dolar sign
    else {
      Document_set_line(document, lineNumber, commandTemp + getIndexOfSecondSpace(commandTemp) + 1);
    }
    free(commandTemp);
  }
}

void handle_append_command(Document *document, const char *command) {
  //get line number
  char * stringLineNumber = strdup(command + 2);
  stringLineNumber[getIndexOfSecondSpace(stringLineNumber)] = '\0';
  long int lineNumber = strtol(stringLineNumber, NULL, 10);

  free(stringLineNumber);
  stringLineNumber = NULL;

  char * commandTemp = strdup(command);
  int indexOfDollarSign = getIndexOfDollarSign(commandTemp);

  //case: new line
  if ((size_t)lineNumber > Document_size(document)) {
    //case: no $
    if (indexOfDollarSign == -1) {
      Document_insert_line(document, lineNumber, commandTemp + getIndexOfSecondSpace(commandTemp) + 1);
    }
    //case: with $
    else {
      commandTemp[indexOfDollarSign] = '\0';
      Document_insert_line(document, lineNumber, commandTemp + getIndexOfSecondSpace(commandTemp) + 1);
      Document_insert_line(document, lineNumber + 1, commandTemp + indexOfDollarSign + 1);
    }
  }
  //case: normal
  else {
    char * lineTemp = strdup(Document_get_line(document, lineNumber));
    char * result = NULL;
    //case: no $
    if (indexOfDollarSign == -1) {
      lineTemp = realloc(lineTemp, strlen(lineTemp) + strlen(commandTemp + getIndexOfSecondSpace(commandTemp) + 1) + 1);
      result = strcat(lineTemp, commandTemp + getIndexOfSecondSpace(commandTemp) + 1);
      Document_set_line(document, lineNumber, result);
    }
    //case normal with $
    else {
      commandTemp[indexOfDollarSign] = '\0';
      lineTemp = realloc(lineTemp, strlen(lineTemp) + strlen(commandTemp + getIndexOfSecondSpace(commandTemp) + 1) + 1);
      result = strcat(lineTemp, commandTemp + getIndexOfSecondSpace(commandTemp) + 1);
      Document_set_line(document, lineNumber, result);
      Document_insert_line(document, lineNumber + 1, commandTemp + indexOfDollarSign + 1);
    }
    free(lineTemp);
  }
  free(commandTemp);
}

void handle_delete_command(Document *document, const char *command) {
  // get line number
  long int lineNumber = strtol(command + 2, NULL, 10);
  //case: line nubmer out of bounds
  if (!isValidLine(document, lineNumber)) {
    invalid_line();
  }
  else {
    Document_delete_line(document, lineNumber);
  }
  
}

void handle_search_command(Document *document, const char *command) {
  // TODO implement handle_search_command

  //case: nonempty
  if (strlen(command) != 1) {
    char * query = strdup(command + 1);
    char * ptr = NULL;
    
    size_t i;
    for (i = 1; i <= Document_size(document); i++) {
      ptr = strstr(Document_get_line(document, i), query);
      if (ptr != NULL) {
	print_search_line(i, Document_get_line(document, i), ptr, query);
      }
    }

    free(query);
  }
}

void handle_save_command(Document *document, const char *filename) {
  // TODO implement handle_save_command
  Document_write_to_file(document, filename);
}
