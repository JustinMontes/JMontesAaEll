// Part 2
// Authors: Justin Montes Aaron Elliot
//pa3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */
#define DSIZE       8			/* Doubleword size (bytes) */
#define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */

// Define baseptr and such
typedef char *addrs_t;
typedef void *any_t;

// Counting variables for heap Check
int count_Vmalloc;
int count_Vfree;

// Doubly-Linked list architecture for tracking
struct node{
  // start of memory chunk
  addrs_t start;
  // end of memory chunk
  addrs_t end;
  // next node in linked list
  struct node *next;
  // previous item in linked list
  struct node *prev;
};

// (NC) node null constructor
void init_node_types_1(struct node* n){
   n->start = NULL;
   n->end = NULL;
   n->next = NULL;
}

// (NC) node constructor for end of linked list
void init_node_types_2(struct node* n, addrs_t start, addrs_t end){
   n->start = start;
   n->end = end;
   n->next = NULL;
}

// (NC) node constructor for middle of linked list
void init_node_types_3(struct node* n, addrs_t start, addrs_t end, struct node* nextNode){
   n->start = start;
   n->end = end;
   n->next = nextNode;
}

// Init VHead for heap
struct node * Head;

//init array dynamically in heap
void heap_init(size_t size){
  addrs_t baseptr;
  baseptr = (addrs_t) malloc(size);
  VHead = malloc(sizeof(struct node));
  init_node_types_2(Head,baseptr, baseptr);
  TOTALSIZE = size;
  printf("value of a = %08llx",(uint64_t)baseptr);
}

// VMalloc
addrs_t *VMalloc (size_t size){
  count_Vmalloc += 1;
  struct node * pointer;
  struct node * look_ahead;
  size = (size+(0x8-size%0x8)%0x8);
  pointer = Head;
  while(pointer!=NULL){
    look_ahead = pointer->next;
    VLook_ahead = look_ahead;
    if (look_ahead != NULL){
  		if ((uint64_t)(look_ahead->start)-(uint64_t)(pointer->end)>=size){
  			// Make a new node and set pointer.next = to it, and its pointer to look_ahead
  			struct node * new = malloc(sizeof(struct node));
  			init_node_types_3(new,pointer->end,pointer->end+size,pointer->next);
  			pointer->next = new;
        //pointer to pointer
  			return pointer->start;
  		}
    }
    else if (TOTALSIZE+Head->end - pointer->end >=size){ // IF we reach the end of the linked list, THEN check if there is space
      struct node * new = malloc(sizeof(struct node));
      init_node_types_2(new,pointer->end,pointer->end+size);
      pointer->next = new;
      //pointer to pointer
      VPointer->next = pointer;
      return (addrs_t*)VPointer->start;
    }
    pointer = look_ahead->start;
  }
  //printf("NoSpaceLeftError : no space left");
  return (NULL);
}

// VFree
void VFree (addrs_t *addr){
  count_Vfree += 1;
  // frees corresponding node
  struct node* current = Head;
  while (current!=NULL){
    struct node* temp = current->next;
    if ( ((char*)(temp->start)) == ((char*)(addr)) ){
	  current->next = current->next->next;
	  free(temp);
      break;
    }
    current = temp;
  }
    while(current != NULL){
      struct node* temp = current->next;
      if(temp != NULL){
        memmove(current->end,temp->start,(uint64_t)temp->end-(uint64_t)temp->start);
        temp->end = current->end+(uint64_t)temp->end-(uint64_t)temp->start;
        temp->start = current->end;
      }
    }
}

// VPut
addrs_t *VPut (any_t data, size_t size) {
     addrs_t* rtnVal = VMalloc (size);
     if (rtnVal!=NULL){
   	   memmove( rtnVal,data, size);
     }
     return rtnVal;
   }

// VGet
void VGet (any_t return_data, addrs_t *addr, size_t size) {
    memmove(return_data, addr, size);
    VFree(addr);
}

// Heap Checker
int main(){
  Init(80);
  char data[80];
  int num_trials = 1000000;
  int counter;
  for(counter = 0; counter < num_trials; counter++ ){
    VPut(TOTALSIZE);
    VGet(VMalloc(TOTALSIZE));
  }
  printf("Num of VMalloc calls for VPut run with trial size n = 1,000,000 = %d\n",count_Vmalloc);
  printf("Num of VFree calls for VGet run with trial size n = 1,000,000 = %d\n",count_Vfree);
}
