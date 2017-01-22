/**
 * Machine Problem: Vector
 * CS 241 - Fall 2016
 */

/* An automatically-expanding array of strings. */
#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 10

/*
 * Vector structure
 * Do not modify the structure
 * array: Void pointer to the beginning of an array of void pointers to
 * arbitrary data.
 * size: The number of elements in the vector. This is the number of actual
 * objects held in the vector, which is not necessarily equal to its capacity.
 * capacity: The size of the storage space currently allocated for the vector,
 * expressed in terms of elements.
 * copyt_constructor: the function callback for you to define the way you want
 * to copy elements
 * destructor:  the function callback for you to define the way you want to
 * destroy elements
 */
struct Vector {
  copy_constructor_type copy_constructor;
  destructor_type destructor;

  void **array;
  size_t size;
  size_t capacity;
};

Vector *Vector_create(copy_constructor_type copy_constructor,
                      destructor_type destructor) {
  // your code here
  Vector* result = malloc(sizeof(Vector));
  result->array = calloc(INITIAL_CAPACITY, sizeof(void*));
  result->size = 0;
  result->capacity = INITIAL_CAPACITY;
  result->copy_constructor = copy_constructor;
  result->destructor = destructor;
  return result;
}

void Vector_destroy(Vector *vector) { //fix
  assert(vector);
  size_t i = 0;
  for (i = 0; i < vector->capacity; i++) {
    if (vector->array[i]) vector->destructor(vector->array[i]);
  }
  vector->size = 0;
  Vector_resize(vector, 0);
}

size_t Vector_size(Vector *vector) {
  assert(vector);
  return vector->size;
}

size_t Vector_capacity(Vector *vector) {
  assert(vector);
  return vector->capacity;
}

void doubleCapacity(Vector * vector) {
  vector->capacity *= 2;
  vector->array = realloc(vector->array, vector->capacity*sizeof(void*));
}

void Vector_resize(Vector *vector, size_t new_size) {
  assert(vector);
  while (new_size > vector->capacity) {
    vector->capacity *= 2;
  }
  while ((new_size <= vector->capacity / 4) && vector->capacity >= 20) {
    vector->capacity /= 2;
  }

  if (new_size < vector->size) {
    size_t j;
    for (j = new_size; j < vector->size; j++) {
      if (vector->array[j] != NULL) {
	free(vector->array[j]);
	vector->array[j] = NULL;
      }
    }
  }
  
  
  void * tmp = realloc(vector->array, vector->capacity * sizeof(void*));
  vector->array = tmp;

  if (new_size > vector->size) {
    size_t i;
    for (i = vector->size; i < new_size; i++) {
      vector->array[i] = NULL;
    }
  }
  
  vector->size = new_size;
}

void Vector_set(Vector *vector, size_t index, void *elem) { //isnt it bad practice to pass in a pointer like this since it might get altered by user?
  assert(vector);
  assert(index >= 0 && index < vector->size);
  vector->destructor(vector->array[index]);
  vector->array[index] = vector->copy_constructor(elem);
}

void *Vector_get(Vector *vector, size_t index) {
  assert(vector);
  assert(index >= 0 && index < Vector_size(vector));
  return vector->array[index];
}

void Vector_insert(Vector *vector, size_t index, void *elem) {
  assert(vector);
  assert(index >= 0 && index <=  vector->size);
  if (vector->size >= vector->capacity) {
    Vector_resize(vector, vector->capacity + 1);
  }
  size_t i;
  for (i = vector->size; i > index; i--) {
    vector->array[i] = vector->array[i-1];
  }
  vector->array[index] = vector->copy_constructor(elem);
  vector->size++;
}

void Vector_delete(Vector *vector, size_t index) {
  assert(vector);
  assert(index >=0 && index < Vector_size(vector));
  //use destructor
  //check resize
  //free(vector->array[index]);
  vector->destructor(vector->array[index]);
  size_t i;
  for (i = index; i < vector->size - 1; i++) {
    vector->array[i] = vector->array[i+1];
  }
  vector->size--;
  Vector_resize(vector, vector->size);
}

void Vector_append(Vector *vector, void *elem) {
  assert(vector);
  if (vector->size >= vector->capacity) doubleCapacity(vector);
  vector->array[vector->size] = vector->copy_constructor(elem);
  vector->size++;
}
