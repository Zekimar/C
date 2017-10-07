//KLG92 Kevin Good

//Did not implement quick fit sorry can i get extra points for admitting it
//didn't finish implementing splitting, in comment blelwo

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mymalloc.h"

// The smallest allocation possible is this many bytes.
// Any allocations <= this size will b put in bin 0.
#define MINIMUM_ALLOCATION  16

// Every bin holds blocks whose sizes are a multiple of this number.
#define SIZE_MULTIPLE       8

// The biggest bin holds blocks of this size. Anything bigger will go in the overflow bin.
#define BIGGEST_BINNED_SIZE 512

// How many bins there are. There's an "underflow" bin (bin 0) and an overflow bin (the last bin).
// That's where the '2' comes from in this formula.
#define NUM_BINS            (2 + ((BIGGEST_BINNED_SIZE - MINIMUM_ALLOCATION) / SIZE_MULTIPLE))

// The index of the overflow bin.
#define OVERFLOW_BIN        (NUM_BINS - 1)

// How many bytes the block header is, in a USED block.
// NEVER USE sizeof(BlockHeader) in your calculations! Use this instead.
#define BLOCK_HEADER_SIZE   offsetof(BlockHeader, prev_free)

// The smallest number of bytes a block (including header and data) can be.
#define MINIMUM_BLOCK_SIZE  (MINIMUM_ALLOCATION + BLOCK_HEADER_SIZE)

typedef struct BlockHeader
{
	unsigned int size; // The byte size of the data area of this block.
	int in_use;        // 1 if allocated, 0 if free.

	// Doubly-linked list pointers for the previous and next *physical* blocks of memory.
	// All blocks, allocated or free, must keep track of this for coalescing purposes.
	struct BlockHeader* prev_phys;
	struct BlockHeader* next_phys;

	// These next two members are only valid if the block is not in use (on a free list).
	// If the block is in use, the user-allocated data starts here instead!
	struct BlockHeader* prev_free;
	struct BlockHeader* next_free;
} BlockHeader;

// Your array of bins.
BlockHeader* bins[NUM_BINS] = {};

// The LAST allocated block on the heap.
// This is used to keep track of when you should contract the heap.
BlockHeader* heap_tail = NULL;

// =================================================================================================
// Math helpers
// =================================================================================================

// Given a pointer and a number of bytes, gives a new pointer that points to the original address
// plus or minus the offset. The offset can be negative.
// Since this returns a void*, you have to cast the result to another pointer type to use it.
void* ptr_add_bytes(void* ptr, int byte_offs)
{
	return (void*)(((char*)ptr) + byte_offs);
}

// Gives the number of bytes between the two pointers. first must be <= second.
unsigned int bytes_between_ptrs(void* first, void* second)
{
	return (unsigned int)(((char*)second) - ((char*)first));
}

// Given a pointer to a block header, gives the pointer to its data (such as what you'd return
// from my_malloc).
void* block_to_data(BlockHeader* block)
{
	return (void*)ptr_add_bytes(block, BLOCK_HEADER_SIZE);
}

// Given a data pointer (such as passed to my_free()), gives the pointer to the block that
// contains it.
BlockHeader* data_to_block(void* data)
{
	return (BlockHeader*)ptr_add_bytes(data, -BLOCK_HEADER_SIZE);
}

// Given a data size, gives how many bytes you'd need to allocate for a block to hold it.
unsigned int data_size_to_block_size(unsigned int data_size)
{
	return data_size + BLOCK_HEADER_SIZE;
}

// Rounds up a data size to an appropriate size for putting into a bin.
unsigned int round_up_size(unsigned int data_size)
{
	if(data_size == 0)
		return 0;
	else if(data_size < MINIMUM_ALLOCATION)
		return MINIMUM_ALLOCATION;
	else
		return (data_size + (SIZE_MULTIPLE - 1)) & ~(SIZE_MULTIPLE - 1);
}

// Given a data size in bytes, gives the correct bin index to put it in.
unsigned int size_to_bin(unsigned int data_size)
{
	unsigned int bin = (round_up_size(data_size) - MINIMUM_ALLOCATION) / SIZE_MULTIPLE;

	if(bin > OVERFLOW_BIN)
		return OVERFLOW_BIN;
	else
		return bin;
}

// =================================================================================================
// Your functions!
// =================================================================================================

// Put any of your code here.

// =================================================================================================
// Public functions
// =================================================================================================

void* my_malloc(unsigned int size)
{
	if(size == 0)
		return NULL;
	size = round_up_size(size);
	//printf("%d\n", size);
	//size = size + BLOCK_HEADER_SIZE;
	BlockHeader* b = NULL;
	if (bins[OVERFLOW_BIN] != NULL){
		BlockHeader* c = bins[OVERFLOW_BIN];
		while (c != NULL && b == NULL){
			if (c->size >= size){
				/* if (c->size >= (size + MINIMUM_BLOCK_SIZE)){
					if (c->prev_free != NULL){
						c->prev_free->next_free = c->next_free;
					}
					if (c->next_free != NULL){
						c->next_free->prev_free = c->prev_free;
					}
					int old_size = c->size - (size + BLOCK_HEADER_SIZE);
					b = c;
					b->size = size;
					b->in_use = 1;
					if (c->prev_phys != NULL){
						c->prev_phys->next_phys = b;
					}
					c = ptr_add_bytes(block_to_data(b), b->size);
					c->size = old_size;
					printf("%d %d %d %d\n", bins[OVERFLOW_BIN]->size, bytes_between_ptrs(b, c), c->size, b->size);
						
						
					c->next_phys = b->next_phys;
					if (b->next_phys != NULL){
						b->next_phys->prev_phys = c;
					}
					b->next_phys = c;
					c->prev_phys = b;
						
					bins[OVERFLOW_BIN]->prev_free = c;
					c->next_free = bins[OVERFLOW_BIN];
					bins[OVERFLOW_BIN] = c;

				}else{ */
					if (c->prev_free != NULL){
						c->prev_free->next_free = c->next_free;
					}
					if (c->next_free != NULL){
						c->next_free->prev_free = c->prev_free;
					}
					b = c;
					b->in_use = 1;
					if (bins[OVERFLOW_BIN] == c){
						bins[OVERFLOW_BIN] = bins[OVERFLOW_BIN]->next_free;
					}
				/* } */
			}else{
				c = c->next_free;
			}
		}
	}
	//printf("b\n");
	if(b == NULL){
		b = (BlockHeader*)sbrk(size + BLOCK_HEADER_SIZE);
		b->size = size;
		b->in_use = 1;
		//printf("b\n");
		if (heap_tail != NULL){
			heap_tail->next_phys = b;
			b->prev_phys = heap_tail;
		}
		heap_tail = b;
	}
	//printf("malloc %d %d\n", b->size, b->in_use);
	return block_to_data(b);
	// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// Any allocation code goes here!

	return NULL;
	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}

void my_free(void* ptr)
{
	if(ptr == NULL)
		return;
	BlockHeader* b = data_to_block(ptr);
	BlockHeader* c = b->prev_phys;
	BlockHeader* d = b->next_phys;
	//printf("\nprev is %p\n", b->prev_phys);
	//printf("free %d %d\n", b->size, b->in_use);
	while(c != NULL && c->in_use == 0){
		c->size += (b->size + BLOCK_HEADER_SIZE);
		c->next_phys = b->next_phys;
		if (d != NULL){
			d->prev_phys = c;
		}
		//printf("p1\n");
		b = c;
		c = b->prev_phys;
		//printf("merged with prev size is %d\n", b->size, b, c);
	}
	//printf("a\n");
	while (d != NULL && d->in_use == 0){
		b->size += (d->size + BLOCK_HEADER_SIZE);
		b->next_phys = d->next_phys;
		if (d->next_phys != NULL){
			d->next_phys->prev_phys = b;
		}
		d = b->next_phys;
		//printf("merged with next size is %d\n", b->size);
	}
	//printf("b\n");
	if (b->in_use == 1){
		b->prev_free = NULL;
		b->next_free = NULL;
	}
	b->in_use = 0;
	//printf("c\n");
	if (b->next_phys != NULL){
			if (bins[OVERFLOW_BIN] != NULL && bins[OVERFLOW_BIN] != b){
				bins[OVERFLOW_BIN]->prev_free = b;
				b->next_free = bins[OVERFLOW_BIN];
			}
			bins[OVERFLOW_BIN] = b;
		//printf("a1 added %p\n", b);
	}
	//printf("d\n");
	if (b->next_phys == NULL){
		heap_tail = b->prev_phys;
		if (b->prev_phys != NULL){
			heap_tail->next_phys = NULL;
		}
		brk(b);
		if (bins[OVERFLOW_BIN] == b){
			bins[OVERFLOW_BIN] = NULL;
		}
		b = NULL;
		
		
		//printf("b1\n");
	}
	//printf("e\n");
	// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// Any deallocation code goes here!

	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}