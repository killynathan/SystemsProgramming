/**
 * Mini Valgrind Lab
 * CS 241 - Fall 2016
 */

#include "mini_valgrind.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  // your tests here using malloc and free
  int * n1 = malloc(sizeof(int));
  *n1 = 5;
  int * n2 = malloc(sizeof(int));
  *n2 = 1;
  int * n3 = malloc(sizeof(int));
  *n3 = 3;
  free(n1);
  // Do NOT modify this line
  atexit(print_report);
  return 0;
}
