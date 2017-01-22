/**
 * Chatroom Lab 2 
 * CS 241 - Fall 2016
 */
#include "camelCaser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

//count periods to find out how many sentences there are in input
//= how long the result array should be 

int getNumOfPuncts(const char *input_str) {
  if (input_str == NULL) {
    return 0;
  }
  int count = 0;
  while (*input_str) {
    if (ispunct(*input_str)) {
      count++;
    }
    input_str++;
  }
  return count;
}

int getSizeOfCurrentSentence(const char *input_str) { 
  if (input_str == NULL) {
    return 0;
  }
  int count = 0;
  while (*input_str) {
    if (ispunct(*input_str)) {
      if (count == 0) return 0;
      else return count;
    }
    else if (isalpha(*input_str) || isdigit(*input_str)){
      count++;
    }
    input_str++;
  }
  return -1;
}

void appendChar(char **input_str, char ch) {
  if (input_str == NULL) return; //check for NULL
  char newStr[strlen(*input_str) + 1]; // +1 for new char
  char * letter = malloc(2); //convert letter to string to concat
  letter[0] = ch;
  letter[1] = '\0';
  strcpy(newStr, *input_str);
  strcat(newStr, letter);
  free(*input_str); 
  *input_str = malloc(strlen(newStr) + 1);
  strcat(*input_str, newStr);
  free(letter);
}

void append(char **input_str, char ch) {
  if (input_str == NULL || *input_str == NULL) return;
  char * letter = malloc(2);
  letter[0] = ch;
  letter[1] = '\0';
  strcat(*input_str, letter);
  free(letter);
}

char **camel_caser(const char *input_str){
  if (input_str == NULL) {
    return NULL;
  }

  int resultSize = getNumOfPuncts(input_str);
  char ** result = malloc((resultSize + 1) * (sizeof(char*)));
  int resultIndex = 0;
  int isFirstLetter = 0;
  int isFirstLetterOfSentence = 1;
  char * currentStr = malloc(1);
  int diff = 'A' - 'a';
  char currentChar;
  int sizeOfCurrentSentence = getSizeOfCurrentSentence(input_str);
  
  
  while (*input_str) {
    if (isalpha(*input_str)|| isdigit(*input_str)) { //if current char is letter
      currentChar = *input_str;
      if (isFirstLetter && !isFirstLetterOfSentence) { //check if is first letter of word and not first of whole sentence
	if ((*input_str) >= 'a' && (*input_str) <= 'z') {
	  currentChar += diff;
	}
	isFirstLetter = 0;
      }
      else if (isFirstLetterOfSentence) { //check if it is first letter of sentence
	//sizeOfCurrentSentence = getSizeOfCurrentSentence(input_str); 
	if (sizeOfCurrentSentence == -1) break; //exit if no punct mark after this point.
	free(currentStr);
	currentStr = malloc(sizeOfCurrentSentence + 1);
	if ((*input_str) >= 'A' && (*input_str) <= 'Z') {
	  currentChar -=diff;
	}
	isFirstLetterOfSentence = 0;
	isFirstLetter = 0;
      }
      else { //general case
	if ((*input_str) >= 'A' && (*input_str) <= 'Z') {
	  currentChar -= diff;
	}
      }
      append(&currentStr, currentChar); //add current char to current string
    }
    else if (*input_str == ' ') { //if we encounter a space, we know next letter could a first letter
      isFirstLetter = 1;
    }
    else if (ispunct(*input_str)) { //if we encoutner a punctuation mark, we know sentence is over
      
      if (sizeOfCurrentSentence == 0) { //justa single punct mark
	free(currentStr);
	currentStr = malloc(1);
	currentStr[0] = '\0';
      }
      result[resultIndex] = malloc(strlen(currentStr) + 1); //add current string to result 
      strcpy(result[resultIndex], currentStr); 
      free(currentStr);
      currentStr = NULL;
      resultIndex++;
      isFirstLetterOfSentence = 1;
      sizeOfCurrentSentence = getSizeOfCurrentSentence(input_str + 1);
    }
    input_str++;
  }
  result[resultSize] = NULL; //set last element of result to NULL
  return result;
}
