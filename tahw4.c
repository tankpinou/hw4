#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>//for strstr and strcpy
#include <ctype.h> //for tolower
#include <signal.h>
#include <sys/wait.h>

typedef struct block{

    size_t size;
    int valid;
    struct block* next;
    struct block* prev;

}block;

void *heap;
int typfit;
block*  freelist;
block* lastAllocatedBlock;
block* lastfreeBlock;

void myinit(int allocAlg){

    typfit = allocAlg;

    size_t size = 1 << 20 ; // 10000000... = 1 mb or 1 mill bits

    heap = malloc(size);

    // top creates like the empty array that we have to give the suff when they call
    //  1000000 bits =
    freelist = heap;
    freelist->size  = size ;//-(sizeof(freelist));
    freelist->valid =1;
    freelist->next =NULL;
    freelist->prev = NULL;

    lastfreeBlock=freelist;

    printf("Initialized\n");
}

void split(block *fitting_slot,size_t size){
    block *new=(void*)((void*)fitting_slot+size+sizeof(block)); // bytes further in memory
    new->size=(fitting_slot->size)-size-sizeof( block);
    new->valid=1;
    new->next=fitting_slot->next;
    new->prev=fitting_slot; // dont care  because the previous just got allocated

    fitting_slot->size=size;
    fitting_slot->valid=0;
    fitting_slot->next=new;//  the curr_fitting_slot point to the next free block

     }

void* mymalloc(size_t size) {

    block *p;
    void *toreturn;

    if (!freelist)
        myinit(0);

    if (typfit == 0) { //first fit
        ////Start at the beginning of free list
        p = freelist;
        //// find first block. while loop.
        while ((p->size < size || p->valid == 0) && p->next != NULL)
            p = p->next;
        /// if size equals , allocate
        if (p->size == size) {
            p->valid = 1;
            toreturn = (void *) (p + sizeof(block));
            lastAllocatedBlock = freelist;
            freelist = freelist->next;
            printf("block malloced\n");
            return toreturn;

        } else if (p->size > size) {
            /// if not , split and allocate
            split(p, size);
            toreturn = (void *) (p + sizeof(block));// p point on newly allocated block
            lastAllocatedBlock = freelist;
            freelist = freelist->next;
            printf("block malloced\n");
            return toreturn;
        } else {
            printf("Error\n");
            return toreturn;
        }

    } else if (typfit == 1) { // next fit
        block *current, nestBestBlock;
        current = lastfreeBlock;
        while ((current->size < size || current->valid == 0) && current->next != NULL)
            current = current->next;
        if (p->size == size) {
            p->valid = 1;
            toreturn = (void *) (p + sizeof(block));
            lastAllocatedBlock = freelist;
            freelist = freelist->next;
            printf("block malloced\n");
            return toreturn;

        } else if (p->size > size) {
            /// if not , split and allocate
            split(p, size);
            toreturn = (void *) (p + sizeof(block));// p point on newly allocated block
            lastAllocatedBlock = freelist;
            freelist = freelist->next;
            printf("block malloced\n");
            return toreturn;
        }

    } else { // best fit
        block *current, *bestBlock;
        current = freelist;
        bestBlock = current;

        while ((current->size != size || current->valid == 0) && current->next != NULL) {

            if (current->size >= size && current->size <= bestBlock->size) {
                bestBlock = current; //nice
            }
            current = current->next;
        }
        bestBlock->valid = 0;
        toreturn = bestBlock + sizeof(block);
        lastfreeBlock = freelist;
        printf("block malloced\n");
        return toreturn;
    }
    return toreturn;

}

void coalesce(){
    struct block *curr,*prev;
    curr=freelist;
    while((curr->next)!=NULL){
        if((curr->valid) && (curr->next->valid) && ((curr+curr->size)==curr->next) ){ // to be checked
            curr->size+=(curr->next->size)+sizeof(struct block);
            curr->next=curr->next->next;
            printf("successful Merge\n");
        }
        prev=curr;
        curr=curr->next;

    }
}

void MyFree(void* ptr){
    struct block* curr=ptr;
    struct block* temp;
    --curr;

    if (--curr->valid==0 )
        printf("\"error: not a heap pointer\n");
    else if(curr-> valid==1)
        printf("Error double free\n");
    else {//(((void*)freeList<=ptr)) {//&&(ptr<=(void*)(freeList+1048576))){
         // change of address pointer
        curr->valid=1;
       // if (curr->next && curr->next->valid==1) // || (curr->prev && curr->prev->valid==1))
            coalesce();
        printf("pointer freed\n");

        curr->prev = freelist->prev;
        curr->next = freelist;
        freelist = curr; // nice
        // add blocks at  the beginning of free list
    }
}

//void* myrealloc(void* ptr, size_t size){};

//void mycleanup(){}

//double utilization(){};
int main(){
   int* p= mymalloc(500);

   MyFree(p);

}
