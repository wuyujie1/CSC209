#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

extern void *mem;
extern struct block *allocated_list;
extern struct block *freelist;

#define SIZE 4096 * 64
/*
Test cases:
  1. smalloc multiple times.
  2. smalloc(A) -> smalloc(B) -> smalloc(C) -> sfree(B){C should connect to A}
  -> sfree(C){allocated_list should point to A} -> sfree(A)
  {allocated_list == NULL}.
  3. after test case 1 and 2 -> smalloc (reuse freed space).
  4. smalloc(size) where size > SIZE.
  5. smalloc(size) where size % 8 != 0.
  6. sfree(wrongAddr) ----> This should be interesting since we did not smalloc
    anything, thus the allocated_list should be empty and the program should not
    crush if we give wrong pointer to sfree function. It also should not assign
    any new node with wrong address to freelist.
  7. call smalloc when free_list == NULL
*/
int main(void){
  mem_init(SIZE);
//1st test case
  printf("1st test case:\n");
  //call smalloc 3 times
  write_to_mem(64, smalloc(64), 1);
  write_to_mem(32, smalloc(32), 2);
  write_to_mem(16, smalloc(16), 3);

  printf("List of allocated blocks:\n");
  print_allocated();
  printf("total initial size:\n");
  printf("%d\n", SIZE);
  printf("List of free blocks:\n");
  print_free();
  printf("Contents of allocated memory:\n");
  print_mem();

//2nd test case
  printf("2nd test case:\n");
  //first removel
  printf("first removel\n");
  sfree((allocated_list->next)->addr);
  printf("List of allocated blocks:\n");
  print_allocated();
  printf("List of free blocks:\n");
  print_free();

  //second removel
  printf("second removel\n");
  sfree(allocated_list->addr);
  printf("List of allocated blocks:\n");
  print_allocated();
  printf("List of free blocks:\n");
  print_free();

  //thrid removel
  printf("third removel\n");
  sfree(allocated_list->addr);
  printf("List of allocated blocks:\n");
  print_allocated();
  printf("List of free blocks:\n");
  print_free();

//3rd test case
  printf("3rd test case:\n");
  //reuse first space on free list
  printf("reuse first space on free_list:\n");
  write_to_mem(40, smalloc(40), 1);
  printf("List of allocated blocks:\n");
  print_allocated();
  printf("total initial size:\n");
  printf("%d\n", SIZE);
  printf("List of free blocks:\n");
  print_free();
  printf("Contents of allocated memory:\n");
  print_mem();
  //resue third space on free list
  printf("reuse third space on free_list:\n");
  write_to_mem(32, smalloc(32), 2);
  printf("List of allocated blocks:\n");
  print_allocated();
  printf("total initial size:\n");
  printf("%d\n", SIZE);
  printf("List of free blocks:\n");
  print_free();
  printf("Contents of allocated memory:\n");
  print_mem();

//4th test case
  printf("4th test case:\n");
  smalloc(999999);
  printf("List of allocated blocks:\n");
  print_allocated();
  printf("total initial size:\n");
  printf("%d\n", SIZE);
  printf("List of free blocks:\n");
  print_free();
  printf("Contents of allocated memory:\n");
  print_mem();

//5th test case
  printf("5th test case:\n");
  printf("try to smalloc with input 50: (should allocate 56 bytes space)\n");
  write_to_mem(50, smalloc(50), 10);
  printf("List of allocated blocks:\n");
  print_allocated();
  printf("total initial size:\n");
  printf("%d\n", SIZE);
  printf("List of free blocks:\n");
  print_free();
  printf("Contents of allocated memory:\n");
  print_mem();

//6th test case
  printf("6th test case:\n");
  sfree("ccccccc");
  printf("List of allocated blocks:\n");
  print_allocated();
  printf("List of free blocks:\n");
  print_free();

  mem_clean();

  mem_init(SIZE);
//7th test case
  printf("7th test case:\n");
  smalloc(SIZE);
  printf("List of allocated blocks:\n");
  print_allocated();
  printf("List of free blocks:\n");
  smalloc(10);
  smalloc(SIZE);
  printf("List of allocated blocks:\n");
  print_allocated();
  printf("List of free blocks:\n");
  return 0;
}
