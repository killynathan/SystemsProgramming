/**
 * Machine Problem: Wearables
 * CS 241 - Fall 2016
 */

#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"
#include "vector.h"
#include "wearable.h"

#define MAX_WEARABLES 40

// The wearable server socket, which all wearables connect to.
int wearable_server_fd;

typedef struct {
  pthread_t thread;
  int fd;
  long timestamp;
  // TODO you might want to put more things here
} thread_data;

thread_data **wearable_threads;
int wearable_threads_size = 0;
int session_end = 0;

Vector *data_struct;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void signal_received(int sig) {
 
  /*
  pthread_mutex_destroy(&m);
  free(&m);
  */
  shutdown(wearable_server_fd, SHUT_RDWR);
  close(wearable_server_fd);
  
  session_end = 1;
  exit(1);
}

void *wearable_processor_thread(void *args) {
  thread_data *td = (thread_data *)args;
  int sock_fd = td->fd;

  ssize_t msg_size = 1;
  
  while (msg_size > 0 && session_end == 0) {
    // read in data
    char buffer[64];
    msg_size = read(sock_fd, buffer, 64);
    
    // convert to struct
    
    long timestamp;
    SampleData *data;
    timestamp_entry *entry = malloc(sizeof(timestamp_entry));
    extract_key(buffer, &timestamp, &data); 
    entry->time = timestamp;
    entry->data = data;

    //printf("timestamp: %lu, type: %s, value: %d\n", timestamp, data->type_, data->data_);
    
    // add into data structure with mutex locks1
    pthread_mutex_lock(&m);
    Vector_append(data_struct, (void*)entry);
    pthread_mutex_unlock(&m);
    
  }
  //puts("WEARABLE END");
  close(sock_fd);
  return NULL;
}

bool type1_selector(timestamp_entry *entry) {
  char *type = ((SampleData*)(entry->data))->type_;
  if (strcmp(type, TYPE1) == 0) return true;
  else return false;
}

bool type2_selector(timestamp_entry *entry) {
  char *type = ((SampleData*)entry->data)->type_;
  if (strcmp(type, TYPE2) == 0) return true;
  else return false;
}

bool type3_selector(timestamp_entry *entry) {
  char *type = ((SampleData*)entry->data)->type_;
  if (strcmp(type, TYPE3) == 0) return true;
  else return false;
}

void *user_request_thread(void *args) {
  int sock_fd = *((int *)args);

  ssize_t msg_size = 1;

  while (msg_size > 0 && session_end == 0) {
    char buffer[64];
    timestamp_entry *type1_output;
    timestamp_entry *type2_output;
    timestamp_entry *type3_output;

    // get request
    msg_size = read(sock_fd, buffer, 64);
    //printf("REQUEST: %s\n", buffer);
    long start, end;
    sscanf(buffer, "%zu:%zu", &start, &end);
    //printf("start: %lu, end: %lu\n", start, end);

    // get output for each type
    ssize_t type1_output_size = gather_timestamps(data_struct, start, end, type1_selector, &type1_output);
    ssize_t type2_output_size = gather_timestamps(data_struct, start, end, type2_selector, &type2_output);
    ssize_t type3_output_size = gather_timestamps(data_struct, start, end, type3_selector, &type3_output);
    printf("1: %lu, 2: %lu, 3: %lu\n", type1_output_size, type2_output_size, type3_output_size);

    // send back output
    write_results(sock_fd, TYPE1, type1_output, type1_output_size);
    write_results(sock_fd, TYPE2, type2_output, type2_output_size);
    write_results(sock_fd, TYPE3, type3_output, type3_output_size);
    write(sock_fd, "\r\n", 2);
  }

  //puts("CLIENT END");
  close(sock_fd);
  return NULL;
}

int open_server_socket(const char *port) {

  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  
  int optval = 1;
  setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

  int s = getaddrinfo(NULL, port, &hints, &result);
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(1);
  }

  if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
    perror("bind()");
    exit(1);
  }

  if (listen(sock_fd, MAX_WEARABLES) != 0) {
    perror("listen()");
    exit(1);
  }
  
  return sock_fd;
}

int wearable_server(const char *wearable_port, const char *request_port) {
  // TODO setup signal handler for SIGINT
  signal(SIGINT, signal_received);
  
  // init data struct
  data_struct = Vector_create(timestamp_entry_copy_constructor, timestamp_entry_destructor);

  // init fd's
  int request_server_fd = open_server_socket(request_port);
  wearable_server_fd = open_server_socket(wearable_port);

  // start client port
  pthread_t request_thread;
  int request_socket = accept(request_server_fd, NULL, NULL);
  pthread_create(&request_thread, NULL, user_request_thread, &request_socket);
  close(request_server_fd);

  // accept continous requests on the wearable port
  pthread_t threads[MAX_WEARABLES];
  int count = 0;
  
  while (session_end == 0) {
    int client_fd;
    
    // accept new wearable connection
    client_fd = accept(wearable_server_fd, NULL, NULL);
    
    // create new thread to handle new wearable connection
    thread_data * data = malloc(sizeof(thread_data));
    data->fd = client_fd;
    pthread_create(threads + count, NULL, wearable_processor_thread, (void*)data);
    count++;
  }
  
  // TODO join all threads we spawned from the wearables.

  // Cleanup anything we've allocated.
  pthread_join(request_thread, NULL);

  return 0;
}
