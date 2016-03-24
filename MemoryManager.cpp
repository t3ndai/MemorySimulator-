#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include "MemoryManager.h"

using namespace std;


ostream & operator<<(ostream & out,const MemoryManager &M)
{
   blocknode *tmp = M.firstBlock;
   assert(tmp);
   while(tmp)
   {
      out << "[" << tmp->bsize << ",";
      if (tmp->free)
	 out << "free] ";
      else
	 out << "allocated] ";
      if (tmp->next)
	 out << " -> "; 
      tmp = tmp->next;
   }
   return out;
}

MemoryManager::MemoryManager(unsigned int memtotal): memsize(memtotal)
{
   baseptr = new unsigned char[memsize];
   firstBlock = new blocknode(memsize,baseptr);
}

blocknode *MemoryManager::getFirstPtr()
{
   return firstBlock;
}

unsigned char * MemoryManager::malloc(unsigned int request)
// Finds the first block in the list whose size is >= request
// If the block's size is strictly greater than request
// the block is split, with the newly create block being free. 
// It then changes the original block's free status to false
{
    blocknode *searchblock;
    searchblock = getFirstPtr();
   while (searchblock != NULL ){
       if (searchblock->free ) {
           if (searchblock->bsize == request) {
               searchblock->free = false;
               return searchblock->bptr;
           }
           if (request < searchblock->bsize ) {
               splitBlock(searchblock, request);
               return searchblock->bptr;
           }


       }
       searchblock = searchblock->next;
   }

    return 0;

}

void MemoryManager::splitBlock(blocknode *p, unsigned int chunksize)
// Utility function. Inserts a block after that represented by p
// changing p's blocksize to chunksize; the new successor node 
// will have blocksize the original blocksize of p minus chunksize and 
// will represent a free block.  
// Preconditions: p represents a free block with block size > chunksize
// and the modified target of p will still be free.

{

    int initialSize = p->bsize;
    //p->bptr = 0; //starts at beginning
    blocknode *freeBlock = new blocknode(initialSize-chunksize,p->bptr + chunksize); //create another new node for free memory
    freeBlock->next = p->next;
    freeBlock->prev = p; //freeblock's prev as occupied block
    p->next = freeBlock; //freeblock as occupied block next
    //freeBlock->bptr = p->bptr + chunksize;
   // freeBlock->bsize = initialSize - chunksize; //freeblock size as p's block size - chunksize
    p->bsize = chunksize; //assign occupied block size as chunksize
    p->free = false;

    return;

}

void MemoryManager::free(unsigned char *blockptr)
// makes the block represented by the blocknode free
// and merges with successor, if it is free; also 
// merges with the predecessor, it it is free
{
    assert(blockptr != NULL);
    blocknode *tmp = getFirstPtr();
        while (blockptr !=tmp->bptr){
            tmp = tmp->next;

        }
        if (tmp->bptr == blockptr) {
            tmp->free = true;
            mergeForward(tmp);
        }

    if (tmp->prev != NULL && tmp->prev->free){
        tmp = tmp->prev;
        mergeForward(tmp);
    }



}

void MemoryManager::mergeForward(blocknode *p)
// merges two consecutive free blocks
// using a pointer to the first blocknode;
// following blocknode is deleted
{
    assert(p->next != NULL);
        if (p->next->free ){
            p->bsize += p->next->bsize;
            p->next = NULL;
            delete p->next;
        }


    return;

}

