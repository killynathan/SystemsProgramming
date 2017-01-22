
/**
 * Machine Problem 0
 * CS 241 - Spring 2016
 */

#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
  //1
  first_step(81);
  
  //2
  int val2 = 132;
  second_step(&val2);

  //3
  int * val3 = malloc(sizeof(int));
  *val3 = 8942;
  double_step(&val3);
  free(val3); val3 = NULL;

  //4
  char * val4 = malloc(5 + sizeof(int));
  val4[5] = 15;
  strange_step(val4);
  free(val4); val4 = NULL;

  //5
  char * tempVal5 = malloc(3);
  tempVal5[2] = 0;
  void * val5 = tempVal5;
  empty_step(val5);
  free(val5); val5 = NULL; tempVal5 = NULL;

  //6
  char s2[4];
  s2[3] = 'u';
  void * s = s2;
  two_step(s, s2);

  //7
  char first[5];
  char * second = first + 2;
  char * third = second + 2;
  three_step(first, second, third);

  //8
  char third8[4];
  third8[3] = 8;
  char second8[3];
  second8[2] = 0;
  char first8[2];
  first8[1] = -8;
  step_step_step(first8, second8, third8);

  //9
  int val_9 = 5;
  char * ch_9 = malloc(1);
  *ch_9 = (char)5;
  it_may_be_odd(ch_9, val_9);

  //10
  char str_10[8] = "a,CS241";
  tok_step(str_10);

  //11
  char arr_11[4];
  arr_11[0] = 0x01;
  arr_11[1] = 0x02;
  arr_11[2] = 0x00;
  arr_11[3] = 0x00;
  void *ptr1_11 = arr_11;
  void *ptr2_11 = arr_11;
  the_end(ptr1_11, ptr2_11);
  
  return 0;
}
