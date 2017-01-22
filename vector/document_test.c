/**
 * Machine Problem: Vector
 * CS 241 - Fall 2016
 */

#include <stdio.h>
#include "document.h"

// test your document here
int main() {
  Document* test = Document_create();
  Document_insert_line(test, 1, "hi");
  Document_insert_line(test, 2, "im nathan");
  Document_insert_line(test, 3, " ");
  Document_insert_line(test, 4, "bye");
  Document_insert_line(test, 5, "forever");
  Document_delete_line(test, 3);
  Document_set_line(test, 2, "im not a person");
  
  printf("%zu\n", Document_size(test));

  size_t i;
  for (i = 1; i <= Document_size(test); i++) {
    printf("%s\n", Document_get_line(test, i));
  }

  Document_write_to_file(test, "test.txt");

  puts("------------------------------------------");
  // test creat from file
  Document * othertest = Document_create_from_file("othertest.txt");
  Document_set_line(othertest, 3, "hi im john kou");
  Document_insert_line(othertest, 1, "first lien");
  Document_delete_line(othertest, 7);
  printf("%zu\n", Document_size(othertest));
  size_t j;
  for (j = 1; j <= Document_size(othertest); j++) {
    printf("%s\n", Document_get_line(othertest, j));
  }
  
  return 0;
}
