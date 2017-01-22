/**
 * Mini Valgrind Lab
 * CS 241 - Fall 2016
 */

#include "mini_valgrind.h"
#include "print.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef malloc
#undef realloc
#undef free



/*
 * Replace normal malloc, this malloc will also create meta data info
 * and insert it to the head of the list.
 * You have to malloc enough size to hold both the size of your allocated
 * and the meta_data structure.
 * In this function, you should only call malloc only once.
 *
 * @param size
 *	Size of the memory block, in bytes.
 * @param file
 *	Name of the file that uses mini_valgrind to detect memory leak.
 * @param line
 *	Line in the file that causes memory leak
 *
 * @return
 *	On success, return a pointer to the memory block allocated by
 *	the function. Note that this pointer should return the actual
 *	address the pointer starts and not the meta_data.
 *
 *	If the function fails to allocate the requested block of memory,
 *	return a null pointer.
 */
void *mini_malloc(size_t size, const char *file, size_t line) {
  // your code here
  void * result = malloc(sizeof(meta_data) + size);
  ((meta_data *)result)->size = size;
  ((meta_data *)result)->line_num = line;
  strcpy(((meta_data *)result)->file_name, file);
  insert_meta_data((meta_data*)result, size, file, line);
  return (result + sizeof(meta_data));
}

/*
 * Replace normal realloc, this realloc will also first check whether the
 * pointer that passed in has memory. If it has memory then resize the memory
 * to it. Or if the pointer doesn't have any memory, then call malloc to
 * provide memory.
 * For total usage calculation, if the new size is larger than the old size,
 * the total usage should increase the difference between the old size and the
 * new size. If the new size is smeller or equal to the old size, the total
 * usage should remain the same.
 * You have to realloc enough size to hold both the size of your allocated
 * and the meta_data structure.
 * In this function, you should only call malloc only once.
 *
 * @param ptr
 *      The pointer require realloc
 * @param size
 *	Size of the memory block, in bytes.
 * @param file
 *	Name of the file that uses mini_valgrind to detect memory leak.
 * @param line
 *	Line in the file that causes memory leak
 *
 * @return
 *	On success, return a pointer to the memory block allocated by
 *	the function. Note that this pointer should return the actual
 *	address the pointer starts and not the meta_data.
 *
 *	If the function fails to allocate the requested block of memory,
 *	return a null pointer.
 */

int isValidPtr(void * ptr) {
  meta_data * i;
  for (i = head; i != NULL; i = i->next) {
    if (i == ptr) {
      return 1;
    }
  }
  return 0;
}

void *mini_realloc(void *ptr, size_t size, const char *file, size_t line) {
  //check if ptr is non null. if null just malloc
  // update total_usage. add difference of oldsize and newsize
  //malloc
  //need to update cell with next pointing to this
  //need to update file and line
  if (ptr == NULL) {
    return mini_malloc(size, file, line);
  }
  else if (isValidPtr(ptr)) {
    void * result = realloc(ptr, size + sizeof(meta_data));
    meta_data * i;
    for (i = head; i != NULL; i=i->next) {
      if (i->next == ptr) {
	i->next = result;
      }
    }
    if (size > ((meta_data *)result)->size) {
      ((meta_data *)result)->size += (size - ((meta_data *)result)->size);
    }
    ((meta_data*)result)->line_num = line;
    strcpy(((meta_data *)result)->file_name, file);
  }
  return NULL;
}

/*
 * Replace normal free, this free will also delete the node in the list.
 *
 * @param ptr
 *	Pointer to a memory block previously allocated. If a null pointer is
 *	passed, no action occurs.
 */
void mini_free(void *ptr) {
  // decrement total memory count
  //remove meta data
  //atuallly free
  if (ptr != NULL) {
    ptr -= sizeof(meta_data);
    remove_meta_data(ptr);
  }
}

/*
 * Helper function to insert the malloc ptr node to the list.
 * Accumulate total_usage here.
 *
 * @param md
 * 	Pointer to the meta_data
 * @param size
 *	Size of the memory block, in bytes.
 * @param file
 *	Name of the file that uses mini_valgrind to detect memory leak.
 * @param line
 *	Line in the file that causes memory leak
 */
void insert_meta_data(meta_data *md, size_t size, const char *file,
                      size_t line) {
  /* set value for malloc_info*/
  total_usage += size;
  md->next = head;
  head = md;
}

/*
 * Helper function to remove the free ptr node from the list.
 * Add to total_free here.
 *
 * @param ptr
 *	Pointer to a memory block previously allocated.
 */
void remove_meta_data(void *ptr) {
  /* check if ptr is in the list and delete it from list */
  if (isValidPtr(ptr)) {
    total_free += ((meta_data *)ptr)->size;
    meta_data * p1 = head;
    meta_data * p2 = head;
    if (head == ptr) {
      head = head->next;
    }
    for (; p1 != NULL; p1 = p1->next) {
      if (p1 == ptr) {
	p2->next = p1->next;
	free(p1);
	break;
      }
      p2 = p1;
    }
  }
  else {
    bad_frees++;
  }
}

/*
 * Deletes all nodes from the list. Called when the program exits so make sure
 * to not count these blocks as freed.
 */
void destroy() {
  // your code here
  meta_data * ptr1 = head;
  meta_data * ptr2 = head;
  for (;ptr1 != NULL; ptr1 = ptr2) {
    ptr2 = ptr1->next;
    free(ptr1);
  }
}

/*
 * Print mini_valgrind leak report.
 */
void print_report() {
    print_leak_info(head, total_usage, total_free, bad_frees);
   
}
