/**
 * Mad Mad Access Pattern
 * CS 241 - Fall 2016
 */
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "utils.h"

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/

int get_wordlength (FILE *data_file) {
  fseek(data_file, 16, SEEK_CUR);
  int count = 0;
  char current_letter;
  while (1) {
    fread(&current_letter, 1, 1, data_file);
    count++;
    if (current_letter == '\0') return count;
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
  
  //traverse file
  uint32_t curr_node_pos = BINTREE_ROOT_NODE_OFFSET;
  int word_length;
  void *node = NULL;
  char *word = NULL;
  int comp_val;
  size_t node_size = sizeof(BinaryTreeNode);
  char *keyword;
  
  int x = 0;
  for (; x < argc - 2; x++) {
    //reset pos
    curr_node_pos = BINTREE_ROOT_NODE_OFFSET;
    
    //get keyword
    keyword = keywords[x];

    
    //go to current node
    while (1) {
      //get nodes 
      fseek(data_file, curr_node_pos, SEEK_SET);
      word_length = get_wordlength(data_file);
      fseek(data_file, curr_node_pos, SEEK_SET);
      node = malloc(node_size + word_length);
      fread(node, node_size + word_length, 1, data_file);
      word = ((BinaryTreeNode*)node)->word;
      
      //compare words
      comp_val = strcmp(keyword, word);
      
      //if same print info
      if (comp_val == 0) {
	printFound(keyword, ((BinaryTreeNode*)node)->count, ((BinaryTreeNode*)node)->price);
	free(node);
	break;
      }
      
      //if not, go to next node
      else if (comp_val < 0) {
	curr_node_pos = ((BinaryTreeNode*)node)->left_child;
	if (curr_node_pos == 0) {
	  printNotFound(keyword);
	  free(node);
	  break;
	}
      }

      else if (comp_val > 0) {
	curr_node_pos = ((BinaryTreeNode*)node)->right_child;
	if (curr_node_pos == 0) {
	  printNotFound(keyword);
	  free(node);
	  break;
	}
      }
    }
  }
  return 0;
}
