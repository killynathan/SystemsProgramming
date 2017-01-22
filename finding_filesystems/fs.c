/**
* Finding Filesystems
* CS 241 - Fall 2016
*/
#include "format.h"
#include "fs.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void fs_ls(file_system *fs, char *path) {
  //check if legit, file or dir
  //if dir loop through
  inode *in = get_inode(fs, path);
  if (in == NULL) {
    print_no_file_or_directory();
    return;
  }
  else if (is_file(in)) {
    print_file(path);
  }
  else if (is_directory(in)) {
    data_block data_b;
    inode_number inode_num;
    uint64_t numofentries = in->size / 256;
    uint64_t entriesProcessed = 0;
    dirent *info = malloc(sizeof(dirent));
    inode indirect_inode = fs->inode_root[in->single_indirect];
    
    int i = 0;
    for (; i < NUM_DIRECT_INODES; i++) {
      data_b = fs->data_root[in->direct_nodes[i]];
      uint64_t maxentriesinblock = sizeof(data_b) / 256;
      uint64_t j = 0;
      for (; j < maxentriesinblock; j++) {
	if (entriesProcessed >= numofentries) {
	  free(info);
	  return;
	}
	make_dirent_from_string(data_b.data + (j * 256), info);
	inode_num = info->inode_num;
	inode curr_in = fs->inode_root[inode_num];
	if (is_file(&curr_in)) {
	  print_file(info->name);
	}
	else if (is_directory(&curr_in)) {
	  print_directory(info->name);
	}
	entriesProcessed++;
      }
    }


    int k = 0;
    for (; k < NUM_DIRECT_INODES; k++) {
      data_b = fs->data_root[indirect_inode.direct_nodes[k]];
      uint64_t maxentriesinblock = sizeof(data_b) / 256;
      uint64_t x = 0;
      for (; x < maxentriesinblock; x++) {
	if (entriesProcessed >= numofentries) {
	  free(info);
	  return;
	}
	make_dirent_from_string(data_b.data + (x * 256), info);
      	inode_num = info->inode_num;			
	inode curr_in = fs->inode_root[inode_num];
	if (is_file(&curr_in)) {
	  print_file(info->name);
	}
	else if (is_directory(&curr_in)) {
	  print_directory(info->name);
	}
	entriesProcessed++;
      }
      
    }

    free(info);
  }

}

void fs_cat(file_system *fs, char *path) {
  // Shiver me inodes
  //get inode
  //loop through
  inode *base_inode = get_inode(fs, path);
  if (base_inode == NULL) {
    print_no_file_or_directory();
    return;
  }

  inode indirect_inode = fs->inode_root[base_inode->single_indirect];
  
  //inode indirect_inode = fs->inode_root[base_inode->single_indirect];
  uint64_t filesize = base_inode->size;
  uint64_t byteswritten = 0;
  uint64_t bytesToWrite;
  
  int i = 0;
  data_block data_b;
  for (; i < NUM_DIRECT_INODES; i++) {
    if (byteswritten >= filesize) break;
    data_b = fs->data_root[base_inode->direct_nodes[i]];
    if ((filesize - byteswritten) <= (16 * KILOBYTE)) {
      bytesToWrite = filesize - byteswritten;
    }
    else {
      bytesToWrite = sizeof(data_b.data);
    }
    write(1, (void*)(data_b.data), bytesToWrite);
    byteswritten += bytesToWrite;
  }

  int j = 0;
  for (; j < NUM_DIRECT_INODES; j++) {
    if (byteswritten >= filesize) break;
    data_b = fs->data_root[indirect_inode.direct_nodes[j]];
    if ((filesize - byteswritten) <= (16 * KILOBYTE)) {
      bytesToWrite = filesize - byteswritten;
    }
    else {
      bytesToWrite = sizeof(data_b.data);
    }
    write(1, (void*)(data_b.data), bytesToWrite);
    byteswritten += bytesToWrite;
  }

}
