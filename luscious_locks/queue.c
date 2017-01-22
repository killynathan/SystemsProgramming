/**
 * Luscious Locks Lab 
 * CS 241 - Fall 2016
 */
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Struct representing a node in a queue_t
 */
typedef struct queue_node_t {

  struct queue_node_t *next;
  void *data;
} queue_node_t;

/**
 * Struct representing a queue
 */
struct queue_t {

  queue_node_t *head, *tail;
  int size;
  int maxSize;
  pthread_cond_t cv;
  pthread_mutex_t m;
};

/**
 *  Given data, place it on the queue.  Can be called by multiple threads.
 *  Blocks if the queue is full.
 */
void queue_push(queue_t *queue, void *data) {
  /* Your code here */
  //lock
  //check max
  //if max is positve check if size is max. if so wait
  //if max is negative push
  //unlock

  pthread_mutex_lock(&queue->m);
  if (queue->maxSize >= 0) {
    if (queue->size == queue->maxSize) {
      pthread_cond_wait(&queue->cv, &queue->m);
    }
  }
  queue_node_t * newNode = malloc(sizeof(queue_node_t));
  newNode->data = data; //hardcopy?
  if (queue->head == NULL) {
    queue->tail = newNode;
    queue->head = newNode;
    newNode->next = NULL;
  }
  else {
    newNode->next = NULL;
    queue->tail->next = newNode;
    queue->tail = newNode;
  }
  queue->size++;
  pthread_cond_signal(&queue->cv);
  pthread_mutex_unlock(&queue->m);
}

/**
 *  Retrieve the data from the front of the queue.  Can be called by multiple
 * threads.
 *  Blocks if the queue is empty.
 */
void *queue_pull(queue_t *queue) {
  /* Your code here */
  //lock
  //check if empty wait if so
  //save first element and remove it
  //unlock
  //return first element
  pthread_mutex_lock(&queue->m);
  while (queue->size == 0) {
    pthread_cond_wait(&queue->cv, &queue->m);
  }
  void* result = queue->head->data;
  queue_node_t * temp = queue->head;
  queue->head = queue->head->next;
  if (queue->head == NULL) queue->tail = NULL;
  queue->size--;
  free(temp);
  pthread_cond_signal(&queue->cv);
  pthread_mutex_unlock(&queue->m);
  return result;
}

/**
 *  Allocates heap memory for a queue_t and initializes it.
 *  Returns a pointer to this allocated space.
 */
queue_t *queue_create(int maxSize) {
  queue_t * result = malloc(sizeof(queue_t));
  result->head = NULL;
  result->tail = NULL;
  result->size = 0;
  result->maxSize = maxSize;
  pthread_mutex_init(&result->m, NULL);
  pthread_cond_init(&result->cv, NULL);
  return result;
}

/**
 *  Destroys the queue, freeing any remaining nodes in it.
 */
void queue_destroy(queue_t *queue) {
  pthread_mutex_destroy(&queue->m);
  pthread_cond_destroy(&queue->cv);

  queue_node_t * i = queue->head;
  queue_node_t * j = queue->head;
  for (; i != NULL; i = j) {
    j = i->next;
    free(i);
  }
  free(queue);
}
