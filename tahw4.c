//
// Created by Vianney Padonou on 11/16/21.
//

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


block* lastAllocatedBlock;
block* lastfreeBlock;


void* mymalloc(size_t size) {

    block *p;
    void *toreturn;

    if (typfit == 0) { //first fit
        // end = freelist+s
        // p = heap[0]; // or freelist
        // while((p < end) && ((*p & 1) || (*p <= len ) ) ){

        ////Start at the beginning of free list
        p = freelist;
        //// find first block. while loop.
        while ((p->size < size || p->valid == 0) && p->next != NULL)
            p = p->next;
        /// if size equals , allocate
        if (p->size == size) {
            p->valid=0;
            toreturn = (void*)(p+sizeof(block));
            //lastAllocatedBlock = p.....;
            //lastfreeBlock = freelist= p.next;
            return toreturn;

        } else if (p->size > size) {
            /// if not , split and allocate
            split(p, size);
            toreturn = (void *)(p+sizeof(block)); // p point on newly allocated block
            //lastAllocatedBlock = p;
            //lastfreeBlock = freelist = p->next;
            //return pointer to free space
            //coalesce if possible
            return toreturn;
        } else {
            printf("Error\n");
        }
        //split(p,size);

        // p = p+ (*p & -2);

    } else if (typfit == 1) { // next fit
        block* current, nestBestBlock;
        void* toreturn;

        // while((p < end) &&
        //  ((*p & 1) ||
        //  (*p <= len ) ) ){

        //  p = p+ (*p & -2);

    } else { // best fit
        block* current, bestBlock;
        void* toreturn;


    }
}




void myfree(void* ptr){

    //check if valid ptr
    block* p;
    p = (block*)ptr-1;



}

void* myrealloc(void* ptr, size_t size){


}

void mycleanup(){



}

double utilization(){




}