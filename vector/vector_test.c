/**
 * Machine Problem: Vector
 * CS 241 - Fall 2016
 */

#include "vector.h"
#include <stdio.h>

// Test your vector here

//TEST APPEND
int testAppend(Vector * vector) {
  int j;
  for (j = 5; j < 16; j++) {
    int * num = malloc(sizeof(int));
    *num = j;
    Vector_append(vector, num);
  }
  return 1;
}

//TEST INSERT
int testInsert(Vector * vector) {
  size_t i;
  for (i = 0; i < 5; i++) {
    int *num = malloc(sizeof(int));
    *num = (int)i;
    Vector_insert(vector, i, num);
  }
  return 1;
}

//TEST SET
int testSet(Vector* vector) {
  //int * nineninenine = malloc(sizeof(int));
  //*nineninenine = 999;
  Vector_set(vector, 0, NULL);
  return 1;
}

//TEST DELETE
int testDelete(Vector* vector) {
  Vector_delete(vector, 0);
  size_t i;
  for (i = 0; i < 5; i++) {
    Vector_delete(vector, 10);
  }
  return 1;
}

int testDelete2(Vector* vector) {
  size_t i;
  for (i=0; i < 10; i++) {
    Vector_delete(vector, 5);
  }
  return 1;
}

//TEST RESIZE
int testResize(Vector* vector) {
  Vector_resize(vector, 25);
  Vector_resize(vector, 5);
  Vector_resize(vector, 15);
  return 1;
}


void *my_copy_ctor(void *elem) {
  if (elem == NULL) return NULL;
  int * num = malloc(sizeof(int));
  *num = *((int *)elem);
  return num;
}

void my_destructor(void *elem) { free(elem); }

int main() {
  Vector * test = Vector_create(my_copy_ctor, my_destructor);

  testAppend(test);
  testInsert(test);
  testSet(test);
  testDelete(test); //start with 99 1 - 15
  testResize(test);
  testDelete2(test);
  printf("%zu\n", Vector_size(test));
  printf("%zu\n", Vector_capacity(test));
  size_t i;
  puts("NOW PRINTING ALL VALUES");
  for (i = 0; i < Vector_size(test); i++) {
    if (Vector_get(test, i) == NULL) printf("NULL\n");
    else printf("%d\n", *((int*)Vector_get(test, i)));
  }
  return 0;
}
