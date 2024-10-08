//#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "alloc.h"

#define HEADER (sizeof(struct header))
//#define INCREMENT 256

/*
 * This is the header for each allocated memory used internally by the
 * allocator. The test cases use this too to get the size of the header.
 */
/*
struct header {
  uint64_t size;
  struct header *next;
};
 * */

/*
 * Allocation algorithm options
 */
//enum algs { FIRST_FIT, BEST_FIT, WORST_FIT };

/*
 * Allocation statistics. The test cases only use free_size, but other fields
 * are useful for checking the correctness of the implementation.
 */
/*
struct allocinfo {
  int free_size; int free_chunks;
  int largest_free_chunk_size;
  int smallest_free_chunk_size;
};
 * */

//GLOBAL VARIABLES
static int g_limit; //heap limit
enum algs g_alg; //algorithm
int called_space = 0;
int used_space = 0;

//head of free blocks
struct header* free_blocks = NULL;

//HELPER FUNCTIONS
//
//
//
/*
 * alloc() allocates memory from the heap. The first argument indicates the
 * size. It returns the pointer to the newly-allocated memory. It returns NULL
 * if there is not enough space.
 */
void *alloc(int sz) {
  struct header* free_block_loc = free_blocks;
  struct header* block = NULL;
  struct header* prev = NULL;
  int empty = 0;
  int head = 0;


  if (free_blocks != NULL) {
    if (free_blocks->next == NULL)
      head = 1;
  }

  //FINDING FREE BLOCKS BASED OFF ALGORITHM
  enum algs opt = g_alg;
  if (free_block_loc == NULL) //free_blocks is empty
    empty = 1;
  else if (opt == FIRST_FIT) {
    while (free_block_loc != NULL) {
      if (free_block_loc->size >= sz + HEADER)
        break;
      prev = free_block_loc;
      free_block_loc = free_block_loc->next;
    }
  }
  else if (opt == BEST_FIT) {
    int lowest = called_space;
    int low;
    struct header* tmp = free_block_loc;
    struct header* tmp_prev = NULL;

    while (free_block_loc != NULL) {
      low = free_block_loc->size - sz - HEADER;
      if (low < lowest && low >= 0) {
        tmp = free_block_loc;
        tmp_prev = prev;
        lowest = low;
      }
      prev = free_block_loc;
      free_block_loc = free_block_loc->next;
    }
    free_block_loc = tmp;
    prev = tmp_prev;
  }
  else if (opt == WORST_FIT) { 
    int highest = -1;
    int high;
    struct header* tmp = free_block_loc;
    struct header* tmp_prev = NULL;
    while (free_block_loc != NULL) {
      high = free_block_loc->size - sz - HEADER;
      if (high > highest) {
        tmp = free_block_loc;
        tmp_prev = prev;
        highest = high;
      }
      prev = free_block_loc;
      free_block_loc = free_block_loc->next;
    }
    free_block_loc = tmp;
    prev = tmp_prev;
  }


  //INCREMENT HEAP
  if (free_block_loc == NULL) { //no suitable free blocks found
    if (called_space + INCREMENT <= g_limit && called_space + sz + HEADER <= g_limit) { //within limit
      //int needed_size = sz + HEADER;
      //int first = 1;
      //int i = 0;
      struct header* error_check;
      //while (needed_size > 0) { //if sz is > INCREMENT
        //if (first) {
      error_check = sbrk(INCREMENT);
          //first = 0;
        //}
        //else { sbrk(INCREMENT); }

      if (error_check == (void*)-1)
          return NULL;
        //needed_size -= INCREMENT;
        //i++;
      //}
        
      //check if coalesing is possible
      struct header* free_block = free_blocks;
      struct header* ptr_block = error_check;
      
      int found = 0;
      
      while (free_block != NULL) {
        if ((struct header*)((char*)free_block + free_block->size) == ptr_block) {
          free_block->size += INCREMENT;
          found = 1;
          break;
        }
        free_block = free_block->next;
      }
      if (found) 
        block = free_block;
      else 
        block = error_check;
      called_space += INCREMENT; //*i
      //block->size = HEADER + sz;
      block->next = NULL;

      //put remaining free block at start of free_blocks
      
      struct header* new_free_block = (struct header*)((char*)block + HEADER + sz);
      if (found) 
        new_free_block->size = free_block->size - sz - HEADER;
      else
        new_free_block->size = INCREMENT - sz - HEADER; //total size - allocated (sz+HEADER) - free block HEADER
      new_free_block->next = free_blocks;
      free_blocks = new_free_block; //set as head
      if (empty == 1) 
        free_blocks->next = NULL;
      block->size = HEADER + sz;
    }
    else { return NULL; } //not within limit
  }
  else { //free block found
    if (free_block_loc->size - sz - HEADER > HEADER) { //remaining free block is > 16bytes
      //spliting free block into 2
      struct header* new_free_block = (struct header*)((char*)free_block_loc + HEADER + sz);
      new_free_block->size = free_block_loc->size - sz - HEADER;
      block = free_block_loc;
      if (head != 1 && prev != NULL) //free_block_loc is not head
        prev->next = new_free_block;
      else {
        new_free_block->next = free_blocks;
        free_blocks = new_free_block; //set as head
      }
      new_free_block->next = free_block_loc->next;
      block->next = NULL;
      block->size = HEADER + sz;
    }
    else { //return entire free block if too small
      if (head != 1 && prev != NULL) //if not head
        prev->next = free_block_loc->next;
      else 
        free_blocks = free_block_loc->next;

      free_block_loc->next = NULL;
      used_space += free_block_loc->size;
      return (struct header*)((char*)free_block_loc + HEADER);
      //return free_block_loc;
    }
  }
  
  used_space += block->size;
  return (struct header*)((char*)block + HEADER);
  //return block;
}

/*
 * dealloc() frees the memory pointed to by the first argument.
 */
void dealloc(void * ptr) {
  //TOP SEARCH: iterate through LL to check if (struct header*)(char*)ptr + HEADER + ptr->size == free_block
  //if matched, ptr->size += free_block->size + HEADER
  struct header* free_block = free_blocks;
  struct header* ptr_block = (struct header*)((char*)ptr - HEADER);
  struct header* tmp = (struct header*)((char *)ptr + ptr_block->size - HEADER);
  int found = 0;
  
  while (free_block != NULL) {
    if (tmp == free_block) {
      ptr_block->size += free_block->size;
      found = 1;
      break;
    }
    free_block = free_block->next;
  }
  
  //BOT SEARCH; iterate through LL to check if (struct header*)(char*)free_block + HEADER + free_block->size == ptr
  //if matched, free_block->size += ptr->size + HEADER
  free_block = free_blocks; //put back at start of LL
  while (free_block != NULL) {
    if ((struct header*)((char*)free_block + free_block->size) == ptr_block) {
      free_block->size += ptr_block->size;
      found = 1;
      break;
    }
    free_block = free_block->next;
  }
  if (found == 0) { //no need for coaleasing, put at head
    ptr_block->next = free_blocks;
    free_blocks = ptr_block;
  }


}

/*
 * allocopt() sets the options for the memory allocator.
 *
 * The first argument sets the algorithm. The second argument sets the size
 * limit.
 */
void allocopt(enum algs alg, int limit) {
  free_blocks = NULL;
  sbrk(-(called_space));
  called_space = 0;
  g_limit = limit;
  g_alg = alg;
  used_space = 0;
}

/*
 * allocinfo() returns the current statistics.
 */
struct allocinfo allocinfo(void) {
  
  struct header *head = free_blocks;
  
  //finding highest
  int highest = 0;
  int high = 0;
  while (head != NULL) {
    high = head->size;
    if (high > highest) {
      highest = high;
    }
    head = head->next;
  }
  //finding lowest
  head = free_blocks;
  int lowest = 0;
  int low = 0;
  while (head != NULL) {
    low = head->size;
    if (low < lowest) {
      lowest = low;
    }
    head = head->next;
  }
  //finding # free chunks
  head = free_blocks;
  int counter = 0;
  while (head != NULL) {
    counter++;
    head = head->next;
  }
  //finding total free size
  head = free_blocks;
  int total = 0;
  while (head != NULL) {
    total += head->size - HEADER;
    head = head->next;
  }
  struct allocinfo tmp;
  tmp.free_size = total;
  tmp.free_chunks = counter;
  tmp.largest_free_chunk_size = highest;
  tmp.smallest_free_chunk_size = low;
  return tmp;
}





















