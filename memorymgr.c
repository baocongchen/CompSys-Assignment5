#include "memorymgr.h"
#include <stdio.h>
#include <stdlib.h>

int nextMulp8 (int size) {
  while (size%8!=0) size++;
  return size;
}

int *sentinelblock;
int *firstblock;

void initmemory(int size) {
  int heaplength = nextMulp8(size + 12);
  int *heap = malloc(heaplength);
  firstblock = heap+1;
  *firstblock = heaplength - 8;  //size payload and padding
  sentinelblock = heap + heaplength/4 - 1;
  *sentinelblock = 0;
}

void *myalloc(int length) {
  int newblocklength = nextMulp8(length + 4);
  int *ptr = firstBlock();
  int *last = lastBlock();
  int oldsize = *ptr;
  while (ptr < last && (*ptr < newblocklength || isAllocated(ptr))) {
    ptr = nextBlock(ptr);
    oldsize = *ptr;
  }  // loop to find free block that is large enough
  if (ptr == last) return NULL; // no such block found
  *ptr = newblocklength | 1;   // set new length  
  *(ptr+(newblocklength/4)) = oldsize - newblocklength; // set length in remaining     
  return ptr;
}   // allocate a block of length (or more) bytes

void  myfree(void *p) {
  *(int *)p = *(int *)p & -2;  // clear allocated flag
}  // free an allocated block

void  coalesce() {
  int *ptr = firstBlock();
  int *last = lastBlock();
  int *next = nextBlock(ptr);
  while(next < last) {
    if (!isAllocated(ptr) && !isAllocated(next)) {
      *ptr = *ptr + *next;
    }
    else ptr = nextBlock(ptr);
    next = nextBlock(next);
  }  // keep coalescing
}  // go through the heap, coalescing unallocated blocks

void  printallocation() {
  int block = 0;
  int *ptr = firstBlock();
  int *last = lastBlock();
  while (ptr < last) {
    if (isAllocated(ptr))
    printf("Block %2d: size %2d\t\tallocated\n", block, *ptr-1);
    else
    printf("Block %2d: size %2d\t\tunallocated\n", block, *ptr);
    ptr = nextBlock(ptr);
    block = block + 1;
  }
  printf("\n");
}  // print info about the simulated heap, for debugging purposes

int *firstBlock () {
  return firstblock;
};         // return a pointer to the first block on the heap

int *lastBlock() {
  return sentinelblock;
}          // return a pointer to the sentinel block

int isAllocated(int *p) {
  return (*p)%2 == 1;
}  // is the block at location p allocated?

int *nextBlock(int *p) {
  p = p + (*p/8*8)/4;
  return p;
}    // return a pointer to the block that follows p