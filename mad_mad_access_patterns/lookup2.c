/**
 * Mad Mad Access Pattern
 * CS 241 - Fall 2016
 */
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

#include "tree.h"
#include "utils.h"

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/

int get_wordlength(void *data, int node_start_pos) {
  int word_pos = node_start_pos + 16;
  char ch;
  int count = 0;
  while (1) {
    ch = ((char*)data)[word_pos];
    count++;
    if (ch == '\0') return count;
    word_pos++;
  }
  return -1;
}

int main(int argc, char **argv) {
  //check args
  if (argc < 3) {
    printArgumentUsage();
    return 1;
  }

  //get args
  char *data_file_name = argv[1];
  char *keywords[argc - 2];
  int i = 2;
  for (; i < argc; i++) {
    keywords[i - 2] = argv[i];
  }

  //load file
  FILE *data_file = fopen(data_file_name, "r");
  if (!data_file) {
    openFail(data_file_name);
    return 1;
  }
  char legit_check[4];
  fseek(data_file, 0, SEEK_SET);
  fread(legit_check, 4, 1, data_file);
  if (strcmp(legit_check, "BTRE") != 0) {
    formatFail(data_file_name);
    return 1;
  }

  int data_file_fd = fileno(data_file);
  
  //get size of file
  off_t size = lseek(data_file_fd, 0, SEEK_END);

  //mmap
  void *data = mmap(0, size, PROT_READ, MAP_SHARED, data_file_fd, 0);
  if (data == MAP_FAILED) {
    mmapFail(data_file_name);
    return 1;
  }
  
  //traverse file
  uint32_t curr_node_pos = BINTREE_ROOT_NODE_OFFSET;
  BinaryTreeNode *node = NULL;
  char *keyword = NULL;
  char *word = NULL;
  int comp_val;
  
  int x = 0;
  for (; x < argc - 2; x++) {
    keyword = keywords[x];
    curr_node_pos = BINTREE_ROOT_NODE_OFFSET;
    
    while (1) {
      node = (BinaryTreeNode*)(data + curr_node_pos);
      word = node->word;

      comp_val = strcmp(keyword, word);

      //if same print info
      if (comp_val == 0) {
	printFound(keyword, node->count, node->price);
	break;
      }

      //if not, go to next node
      else if (comp_val < 0) {
	curr_node_pos = node->left_child;
	if (curr_node_pos == 0) {
	  printNotFound(keyword);
	  break;
	}
      }

      else if (comp_val > 0) {
	curr_node_pos = node->right_child;
	if (curr_node_pos == 0) {
	  printNotFound(keyword);
	  break;
	}
      }
      
    }
  }
  
  return 0;
}
