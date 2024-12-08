#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

// block_meta holds information about allocated block
typedef struct block_meta {
  size_t size;
  bool free;
  struct block_meta *next_block;
  struct block_meta *prev_block;
} block_meta;

#define META_SIZE sizeof(block_meta)
block_meta *HEAD_BLOCK_META;

block_meta *search_free_space(block_meta **head, size_t size) {
  // traverse the free_list
  // apply first-fit algorithm
  block_meta *current = HEAD_BLOCK_META;
  while (current) {
    if (current->free && (current->size >= size)) {
      return current;
    } else {
      *last = current;
      current = current->next_block; /* if there is not next block, current will
       become NULL, and by checking that we can decide to request new block from
       kernel*/
    }
  }
  return current;
}

block_meta *allocate_space(block_meta *last, size_t size) {
  block_meta *new_block;
  new_block = sbrk(0);
  void *requested = sbrk(size + META_SIZE);

  if ((void *)new_block != requested) {
    printf("allocation error, requested memory address does not match with "
           "heap-top\n");
    return NULL;
  }

  if (requested == (void *)-1) {
    perror("sbrk");
    return NULL;
  }

  if (last) {
    last->next_block = new_block;
    new_block->prev_block = last;
  }

  new_block->size = size;
  new_block->next_block = NULL;
  return new_block;
}

void *my_malloc(size_t size) {
  block_meta *new_block;
  if (size <= 0) {
    return NULL;
  }

  if (!HEAD_BLOCK_META) { // if its null that means first time calling malloc
    new_block = allocate_space(NULL, size);
    if (!new_block) {
      return NULL;
    }
    HEAD_BLOCK_META = new_block;
  } else {
    block_meta *head = HEAD_BLOCK_META;
    new_block = search_free_space(&head, size);
    if (!new_block) { // no free space on the list
      new_block = allocate_space(head, size);
      if (!new_block) {
        return NULL;
      }
    } else {
      // if found block's size is larger than what we need
      // put another block at the end of size+block_size
      new_block->free = false;
    }
  }
  // return start of the allocated memory, not the block
  return (new_block + 1);
}

void free(void *ptr) {
  if (!ptr) {
    return;
  }

  // TODO: implement stitching (getting previous of this block to point to next block)
  block_meta *block = (block_meta *)(ptr - 1);
  block->free = true;
}
