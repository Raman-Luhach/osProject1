/* ========================================================================== */
/*                        CUSTOM MEMORY LIBRARY                              */
/*                                                                           */
/*  Provides a virtual RAM region and custom memory allocation.              */
/*  Replaces the default malloc()/free() which are FORBIDDEN.               */
/*                                                                           */
/*  How it works:                                                            */
/*    - A static array acts as our "virtual RAM" (64 KB).                   */
/*    - mem_init() creates one large free block spanning the entire pool.    */
/*    - mem_alloc() uses a first-fit algorithm to find a free block.        */
/*    - mem_free() marks a block as free and merges adjacent free blocks.   */
/* ========================================================================== */

#ifndef CUSTOM_MEMORY_H
#define CUSTOM_MEMORY_H

/* Total size of our virtual RAM in bytes (64 KB). */
#define MEMORY_POOL_SIZE (64 * 1024)

/* -------------------------------------------------------------------------- */
/*  BlockHeader                                                              */
/*  Each allocated or free region is preceded by this header.                */
/*  The headers form a singly-linked list so we can walk the entire pool.   */
/* -------------------------------------------------------------------------- */
typedef struct BlockHeader
{
    int                  size;     /* Usable payload size (bytes, excl. header) */
    int                  is_free;  /* 1 = available, 0 = in use                */
    struct BlockHeader  *next;     /* Pointer to the next block in the list    */
} BlockHeader;

/* Initializes the memory pool. MUST be called before any mem_alloc().       */
void  mem_init(void);

/* Allocates 'size' bytes from the pool. Returns a pointer to the usable    */
/* payload, or NULL (0) if no suitable block is found.                       */
void *mem_alloc(int size);

/* Frees a previously allocated block. Automatically merges adjacent free    */
/* blocks to reduce fragmentation.                                           */
void  mem_free(void *ptr);

/* Prints the current state of all blocks in the pool (for debugging).       */
void  mem_print_debug(void);

#endif /* CUSTOM_MEMORY_H */
