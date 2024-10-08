# Custom Memory Management System

## Overview

This project involves the development of a custom memory management system in C, designed to handle dynamic memory allocation and deallocation without relying on the standard `malloc()` and `free()` functions. The system implements custom `alloc()` and `dealloc()` functions, along with additional functions for setting allocation parameters and gathering statistics.

## Allocator Design

The core of the system is centered around two primary functions, defined in `alloc.h`:
```c
/*
 * alloc() allocates memory from the heap. The first argument indicates the
 * size. It returns the pointer to the newly-allocated memory. It returns NULL
 * if there is not enough space.
 */
void *alloc(int);

/*
 * dealloc() frees the memory pointed to by the first argument.
 */
void dealloc(void *);
```

## Additional Functions
The system also include two addtional functions:

```c
/*
 * allocopt() sets the options for the memory allocator.
 *Sets options for the memory allocator. The first argument sets the allocation algorithm (FIRST_FIT, BEST_FIT, WORST_FIT), and the second argument sets a limit on the heap size.
 */
void allocopt(enum algs, int);

/*
 * allocinfo() returns the current statistics.
 */
struct allocinfo allocinfo(void);
```

## `alloc()` Function
The `alloc()` function is responsible for handling memory allocation requests. It uses `sbrk()` to increase the heap size when necessary. The heap size is increased by a predefined `INCREMENT` value each time more memory is needed, within the specified limit.

#### Key Features

* Heap Size Management: Uses `sbrk()` to increase the heap size by `INCREMENT`. If the heap size limit is reached, `alloc()` returns `NULL`.
* Block Splitting: Splits free blocks when allocating memory, except when the remaining space is too
  small to be useful.
* Algorithm Selection: Supports `FIRST_FIT`, `BEST_FIT`, and `WORST_FIT` allocation strategies.


## `dealloc()` Function
The `dealloc()` function frees previously allocated memory and manages the linked list of free blocks. It coalesces contiguous free blocks to prevent fragmentation.

#### Key Features

* Linked List Management: Maintains a custom linked list of free blocks.
* Block Coalescing: Merges contiguous free blocks to create larger free blocks, reducing
  fragmentation.

## `allocopt()` Function

The `allocopt()` function allows user to set allocation options:

* Algorithm Choice: Selects the allocation algorithm.

  ```c
  enum algs { FIRST_FIT, BEST_FIT, WORST_FIT };
  ```

* Heap Size Limit: Sets a limit on the heap size.

## `allocinfo()` Function

The `allocinfo()` function provides statistics about the current memory allocation state, including the total free size and other useful metrics for debugging and performance analysis.


## Code Structure and CMake

The project uses a CMakeLists.txt file to manage the build process. To compile the project:

  ```bash
  export CC=$(which clang)
  export CXX=$(which clang++)
  ```

  ```bash
  cmake .
  make
  ```
