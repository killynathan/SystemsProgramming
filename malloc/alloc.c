
/**
 * Machine Problem: Malloc
 * CS 241 - Fall 2016
 z*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int start = 1;
void *heapStart = NULL;
void *heapEnd = NULL;

typedef struct memblock {
  size_t size;
  char isFree;
  struct memblock *next;
  struct memblock *prev;
} memblock;

#define meta_size sizeof(memblock)

void * head = NULL;

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
  size_t newSize = num * size;
  void * ptr = malloc(newSize);
  memset(ptr, 0, newSize);
  return ptr;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginn
ing of the block.  The content of the newly allocated block of
* memory is not initialized, remaining with indeterminate values.
*
* @param size
*    Size of the memory block, in bytes.
*
* @return
*    On success, a pointer to the memory block allocated by the function.
*
*    The type of this pointer is always void*, which can be cast to the
*    desired type of data pointer in order to be dereferenceable.
*
*    If the function failed to allocate the requested block of memory,
*    a null pointer is returned.
*
* @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
*/

memblock *createMemblock(size_t size) {
  if (start) {
    heapStart = sbrk(0);
    start = 0;
  }
  void *result = sbrk(meta_size + size + sizeof(size_t));
  ((memblock*)result)->isFree = 0;
  ((memblock*)result)->size = size;
  ((memblock*)result)->next = NULL;
  ((memblock*)result)->prev = NULL;
  *((size_t*)((char*)result + meta_size + size)) = size;
  heapEnd = sbrk(0);
  return (memblock*)result;
}

memblock *findFreeBlock(size_t size) {
  memblock *current = head;
  memblock *prev = NULL;
  while (current) {
    if (current->size >= size) {
      /*    
      if ((current->size - size) >= ((sizeof(memblock)) + 8)) {
	memblock *newblock = (memblock *)(((char *)current) + meta_size + size);
	newblock->size = current->size - size - meta_size;
	current->size = size;
	if (head == current) {
	  head = newblock;
	  newblock->next = current->next;
	}
	else {
	  prev->next = newblock;
	  newblock->next = current->next;
	}
      }
      else {
      */
      if (1) {
	if (head == current) {
	  head = current->next;
	}
	else {
	  prev->next = current->next;
	}
      }
      return current;
    }
    prev = current;
    current = current->next;
  }
  return NULL;
}

void *malloc(size_t size) {
  // case 1: inserting into empty list
  // case 2: inserting into nonempty list
  // case 2.1: find a match
  // case2.2: dont find a match

  memblock *block;
  if (head == NULL) {
    block = createMemblock(size);
  }
  else {
    block = findFreeBlock(size);
    if (!block) {
      block = createMemblock(size);
    }
  }
  block->isFree = 0;
  return block + 1;
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */

void addblock(memblock* block) {
  if (head != NULL) 
    ((memblock*)head)->prev = block;
  block->next = head;
  block->prev = NULL;
  head = block;
}

void free(void *ptr) {
  // implement free!
  if (!ptr) return;
  memblock * blockPtr = (memblock*)ptr - 1;
  blockPtr->isFree = 1;

  memblock *rightNeighbor = (memblock*)((char*)blockPtr + meta_size + blockPtr->size + sizeof(size_t));
  
  if ((void*)rightNeighbor < heapEnd && rightNeighbor->isFree) {
    blockPtr->size += meta_size + rightNeighbor->size;
    /*if (rightNeighbor->next != NULL) {
      rightNeighbor->next->prev = rightNeighbor->prev;
    }
    if (rightNeighbor->prev != NULL) {
      rightNeighbor->prev->next = rightNeighbor->next;
    }
    if (rightNeighbor == head) {
      head = rightNeighbor->next;
    }
    */
    /*if (head == rightNeighbor) {
      head = rightNeighbor->next;
      if (rightNeighbor->next != NULL)
	rightNeighbor->next->prev = NULL;
    }
    else if (rightNeighbor->next == NULL) {
      if (rightNeighbor->prev != NULL) {
	rightNeighbor->prev->next = NULL;
      }
    }
    else {
      if (rightNeighbor->prev != NULL)
	rightNeighbor->prev->next = rightNeighbor->next;
      if (rightNeighbor->prev != NULL)
	rightNeighbor->next->prev = rightNeighbor->prev;
    }
    */
  }
  if ((void*)((char*)blockPtr - 1) < heapStart) {

  }
  addblock(blockPtr);
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
  // case 1: NULL
  // case 2: non-NULL
  // case 2.1: size == 0
  // case 2.2: size > 0
  // case 2.2.1: big enough already
  // case 2.2.1 not big enough
  if (!ptr)
    return malloc(size);
  else {
    memblock *blockPtr = (memblock *)ptr - 1;
    if (size == 0) {
      free(blockPtr);
      return NULL;
    }
    else {
      if (blockPtr->size >= size) {
	/*
	if ((blockPtr->size - size) >= meta_size + 8) {
	  memblock *newblock = (memblock *)(((char *)blockPtr) + meta_size + size);
	  newblock->size = blockPtr->size - size - meta_size;
	  addblock(newblock);
	  blockPtr->size = size;
	}
	*/
	return ptr;
      }
      else {
	void *newBlock = malloc(size);
	memmove(newBlock, ptr, blockPtr->size);
	free(ptr);
	return newBlock;
      }
    }
  }
  return NULL;
}
