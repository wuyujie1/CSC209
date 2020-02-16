#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"



void *mem;
struct block *freelist;
struct block *allocated_list;


void *smalloc(unsigned int nbytes) {
	//TODO
    unsigned int n = nbytes % 8 == 0 ? nbytes : nbytes - nbytes % 8 + 8;
    struct block *curr = freelist;
    struct block *previous = freelist;
    while (curr != NULL){
      if (curr->size < n){ //not enough size
        curr = curr->next;
      } else if (curr->size == n){ //equal size
        if (curr == freelist){ //link first node of freelist to curr's next
          freelist = curr->next;
        } else { //link curr's previous to curr's next
          while (previous != NULL && previous->next != curr){
            previous = previous->next;
          }
          previous->next = curr->next;
        }
        curr->next = allocated_list;
        allocated_list = curr;
        return curr->addr;
      } else{ //more size
         //link new node to allocated_list
         struct block *newNode = malloc(sizeof(struct block));
         newNode->addr = curr->addr;
         newNode->size = n;
         newNode->next = allocated_list;
         allocated_list = newNode;

         curr->addr += n;
         curr->size -= n;
         return newNode->addr;
      }
    }
      return NULL;
}


int sfree(void *addr) {
	//TODO
    struct block *curr = allocated_list;
    struct block *prev = allocated_list;
    while (curr != NULL && curr->addr != addr){
      curr = curr->next;
    }
    if (curr == NULL){
      return -1;
    }
    if (curr == allocated_list){ //assign first node of allocated_list to curr's next
      allocated_list = curr->next;
    } else {
      while (prev != NULL && prev->next != curr){
        prev = prev->next;
      }
      prev->next = curr->next;
    }
    curr->next = freelist;
    freelist = curr;

    return -1;
}


/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory.
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }

	//TODO
    struct block{
      void *addr;
      int size;
      struct block *next;
    };
    freelist = malloc(sizeof(struct block));

    freelist->addr = mem;
    freelist->size = size;
    freelist->next = NULL;

    allocated_list = NULL;
}

void mem_clean(){
	//TODO
  struct block *tmpAll;
  struct block *tmpFree;
  while (allocated_list != NULL){
    tmpAll = allocated_list;
    allocated_list = allocated_list->next;
    free(tmpAll);
  }
  while (freelist != NULL){
    tmpFree = freelist;
    freelist = freelist->next;
    free(tmpFree);
  }
}
