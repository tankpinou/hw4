//
// Created by Vianney Padonou on 11/6/21.
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

typedef struct block{ // or header of blocks ?
    size_t size; // block size
    int free; //or valid allocated or not
    struct block *previous; // point to previous metadata block
    struct block *next; // point to next metadata block
}block;

block *freeList;  // this one is implicit

//block *trueFreeList;//

// pointer always at the beginning of free list// use data space  for linked pointers
//need boundary tags for  coalescing




block lastFittingslot;
block lastNew;// explicit . only every new free block get added to the free list




void myinit(){
    freeList = (void*)malloc((1024*1024) * sizeof(char));
   //// if (freeList == NULL ) {
        // unable to allocate memory
  ////  }
freeList->size =(1024*1024)-sizeof(block);
freeList->free =1;
freeList->previous =NULL;
freeList->next =NULL;

//trueFreeList = freeList;

}


//split using the free list
void freelistsplit(block *fitting_slot,size_t size){
    block *new=(void*)((void*)fitting_slot+size+sizeof(block)); // bytes further in memory
    new->size=(fitting_slot->size)-size-sizeof( block);
    new->free=1;
    new->next=fitting_slot->next;
    new->previous=fitting_slot; // dont care  because the previous just got allocated

    fitting_slot->size=size;
    fitting_slot->free=0;
    fitting_slot->next=new;//  the curr_fitting_slot point to the next free block
}

void split( block *fitting_slot,size_t size){
     block *new=(void*)((void*)fitting_slot+size+sizeof(block)); // bytes further in memory
    new->size=(fitting_slot->size)-size-sizeof( block);
    new->free=1;
    new->next=fitting_slot->next;
    new->previous=fitting_slot;

    fitting_slot->size=size;
    fitting_slot->free=0;
    fitting_slot->next=new;//  the curr_fitting_slot point to the next free block
   // fitting_slot->previous = ?;//  stay unchanged ? the curr_fitting_slot point to the next free block
}

void *MyMalloc(size_t noOfBytes){
    struct block *curr,*prev;
    void *result;
    if(!(freeList)){
        myinit();
        printf("Memory initialized\n");
    }

    curr = freeList;

    while((((curr->size)<noOfBytes)||((curr->free)==0))&&(curr->next!=NULL)){
        prev=curr;
        curr=curr->next;
        printf("One block checked\n");
    }

    if((curr->size)==noOfBytes){
        curr->free=0;
        //result=(void*)(++curr); // return result to the user
        result=(void*)(++curr);
        if(freeList->next)
            freeList = freeList->next;
        //freeList = (block*)(curr+noOfBytes);//freeList->next;
        // assuming the curr->next  is always a free block
        // ? always pointing to the nextfreeblock;

        // return result to the user
        printf("Exact fitting block allocated\n");
        return result;
    }
    else if((curr->size)>(noOfBytes+sizeof( block))){
        freelistsplit(curr,noOfBytes);
        result=(void*)(++curr);
        if(freeList->next)// address of the beginning of allocated memory or empty space
        freeList = freeList->next;
        //freeList = (block*)(curr+noOfBytes);
        // assuming the curr->next  is always a free block
        printf("Fitting block allocated with a split\n");
        return result;
    }else{
        result=NULL;
        printf("Sorry. No sufficient memory to allocate\n");
        return result;
    }

//+++++++++++++++++++++++++++++++++++++++++++++++
    /*
    // pointer  moves from block to block
    curr=freeList;
    //first fist implicit
    while((((curr->size)<noOfBytes)||((curr->free)==0))&&(curr->next!=NULL)){
        prev=curr;
        curr=curr->next;
        printf("One block checked\n");
    }
    if((curr->size)==noOfBytes){
        curr->free=0;
        result=(void*)(++curr); // return result to the user
        printf("Exact fitting block allocated\n");
        return result;
    }
    else if((curr->size)>(noOfBytes+sizeof( block))){
        split(curr,noOfBytes);
        result=(void*)(++curr); // address of the beginning of allocated memory or empty space
        printf("Fitting block allocated with a split\n");
        return result;
    }// first fist end
    else{
        result=NULL;
        printf("Sorry. No sufficient memory to allocate\n");
        return result;
    }
    +++++++++++++++++++
    */

}
// freelist should d irectly point  to  free  block
void coalesce(){
    struct block *curr,*prev;
    curr=freeList;
    while((curr->next)!=NULL){
        if((curr->free) && (curr->next->free)){
            curr->size+=(curr->next->size)+sizeof(struct block);
            curr->next=curr->next->next;
        }
        prev=curr;
        curr=curr->next;
    }
    printf("successful Merge\n");
}
/*
void *MyRealloc(void* ptr, int size) {
    void* newPtr = malloc(size);
    memcpy(ptr, newPtr, ((block*)ptr)->size);
    free(ptr);
    return newPtr;
}
*/
void MyFree(void* ptr){
    if(ptr){//(((void*)freeList<=ptr)) {//&&(ptr<=(void*)(freeList+1048576))){
        struct block* curr=ptr;
        struct block* temp;
        --curr;
        curr->free=1;
       // coalesce();

        curr->previous = freeList->previous;
        curr->next = freeList;
        freeList = curr; // nice
        // add blocks at  the beginning of free list
    }
    else printf("Please provide a valid pointer allocated by MyMalloc\n");
}


int main(){

    printf("Beginning\n");

    int *p=(int*)MyMalloc(100*sizeof(int));

    char *q=(char*)MyMalloc(250*sizeof(char));

    double *d=(double*)MyMalloc(350*sizeof(double));

    char *e=(char*)MyMalloc(70050*sizeof(char ));

    //MyFree(q);
    MyFree(d);

    double *l=(double*)MyMalloc(370*sizeof(double));

    MyFree(l);
   // MyFree(p);

}


