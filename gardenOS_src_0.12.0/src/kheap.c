//-----------------------------------------------
//
// Code From:
//
//-----------------------------------------------

#include "garden.h"

//#include <kheap.h>
//-----------------------------------------------
// show kernel heap memory spec
void kheap_SPEC();

// points to start of kernel heap memory
void *KHEAP_START = (void *)0x0008000;

// points to end of total kernel heap memory
void *KHEAP_END = (void *)0x0080000;

// points to end of heap memory currently allocated
void *cur_heap = (void *)0x0008000;

// Each Memory block is represented with below struct
struct heap_block {
		unsigned size;
		int free;
		struct heap_block *next;
		struct heap_block *prev;
		char data[1]; // points to actual data returned to kernel
};

// BLOCK SIZE contains malloc metadata
// char data[1] in struct heap_block, points to user requested allocation. 
// It will have a size of 4 because of padding. Hence we delete
// 4 from the sizeof struct
#define BLOCK_SIZE (sizeof(struct heap_block)-4)

// align memory requested to 4 bytes
// ((x-1)/4)*4 + 4
#define align(x)  ((((x-1)>>2)<<2)+4)

// extend heap
struct heap_block * extend_heap(struct heap_block *last, unsigned sz);

// allocate_block
struct heap_block * allocate_block(struct heap_block *last, unsigned sz);

// split_block
void split_block(struct heap_block *last, unsigned sz);

// malloc
void* kmalloc(unsigned sz);

// free
void kfree(void *fr);

// join adjacent free blocks
void fuse_block(struct heap_block *last);

//-----------------------------------------------

// points to start of kernel heap
void *base = NULL;
long mem_size = 2;//BLOCK_SIZE;

void kheap_SPEC (void) {
/*
	print("Start of kernel heap memory :: ");
	print_hex(KHEAP_START);
	print("\nEnd of kernel heap memory   :: ");
	print_hex(KHEAP_END);
	print("\ncurrent pointer to kernel heap memory :: ");
	print_hex(cur_heap);
	print("\n");
*/
	printk("Start of kernel heap memory :: %d", KHEAP_START);
//	printk("%d\n", KHEAP_START);
	printk("\nEnd of kernel heap memory   :: %d", KHEAP_END);
//	printk("%d\n",KHEAP_END);
	printk("\ncurrent pointer to kernel heap memory :: %d", cur_heap);
//	printk("%d\n", cur_heap);
	printk("\n");
}

struct heap_block * extend_heap (struct heap_block *last, unsigned sz)
{
	struct heap_block *temp;
	temp = (struct heap_block *)cur_heap;

	if (  ((char *)cur_heap + BLOCK_SIZE + sz) > (char *)KHEAP_END ) // all memory exhausted
		return NULL;

	temp->size = sz;
	temp->free = 0;
	temp->next = NULL;
	temp->prev = last;
	if (last)
		last->next = temp;

	// Update current memory pointer
	cur_heap = (void *)((char *)cur_heap + BLOCK_SIZE + sz);

	return temp;
}

void split_block (struct heap_block *last, unsigned sz) {
		struct heap_block *new_block;

		if ( (last->size - sz) >= (BLOCK_SIZE + 4) ) {// if left over memory big enough, then split
				new_block = (struct heap_block *)(last->data + sz);
				new_block->size = last->size - sz - BLOCK_SIZE;
				new_block->free = 1;
				new_block->next = last->next;
				new_block->prev = last;

				last->next = new_block;
				last->size = sz;
		}
}

struct heap_block * allocate_block (struct heap_block *last, unsigned sz) {
		struct heap_block *temp, *prev;
		temp = last;
		prev = NULL;
		while (temp)	{
				if (temp->free == 1 && temp->size >= sz) {
						split_block (temp, sz); // call split block, split in case large memory
						temp->free = 0;
						return temp;
				}
				prev = temp;
				temp = temp->next;
		}
		return extend_heap(prev, sz); // if no free block large enough found, call extend heap
}

void * kmalloc (unsigned sz) {
		struct heap_block *temp;
		unsigned sz_align = align(sz);

		if (base) {
				temp = allocate_block (base, sz_align);
		} else {
				temp = extend_heap (base, sz_align);
				base = temp; // initialize base pointer
		}

		if (temp) {
				mem_size += sz;
				return temp->data; // data points to user requested space
		} else {
				return NULL;
	}
}

void fuse_block(struct heap_block *last) {
	struct heap_block *temp;

	// fuse the next blocks is free
	temp = last->next;
	while(temp)
	{
		if ( temp->free == 1 )
		{
			last->size += BLOCK_SIZE + (last->next)->size;
			last->next = temp->next;

			temp = last->next;
		}
		else
			break;
	}

	// fuse the prev blocks is free
	temp = last->prev;
	while(temp)
	{
		if ( temp->free == 1 )
		{
			temp->size += BLOCK_SIZE + last->size;
			temp->next = last->next;
			last = temp;

			temp = last->prev;
		}
		else
			break;
	}
}

void kfree (void *fr) {
		struct heap_block *temp;

		// point to start of meta info
		temp = (struct heap_block *)((char *)fr - BLOCK_SIZE);
		temp->free = 1;
		mem_size -= temp->size;
//		printk ("FREE MEM SIZE: %d\n", temp->size);
		printk ("MEM_SIZE: %d\n", mem_size);
		
		// fuse blocks if possible
		fuse_block(temp);
}

