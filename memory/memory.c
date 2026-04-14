/* ========================================================================== */
/*                        CUSTOM MEMORY LIBRARY                              */
/*                                                                           */
/*  A first-fit memory allocator backed by a static 64 KB array.            */
/*  The pool is divided into blocks, each preceded by a BlockHeader.         */
/*  Free blocks are coalesced on deallocation to reduce fragmentation.       */
/* ========================================================================== */

#include <stdio.h>
#include "../memory/memory.h"

/* -------------------------------------------------------------------------- */
/*  The Virtual RAM                                                          */
/*  This static array IS our entire heap.  No malloc() is ever called.      */
/* -------------------------------------------------------------------------- */
static char memory_pool[MEMORY_POOL_SIZE];

/* Head of the block linked list (always points into memory_pool). */
static BlockHeader *free_list_head = (void *)0;

/* Tracks whether mem_init() has been called. */
static int is_initialized = 0;

/* -------------------------------------------------------------------------- */
/*  mem_init                                                                 */
/*  Sets up the pool with a single free block covering all usable space.    */
/* -------------------------------------------------------------------------- */
void mem_init(void)
{
    /* Place the first header at the very start of the pool */
    free_list_head = (BlockHeader *)memory_pool;

    /* The usable payload is everything after the header */
    free_list_head->size    = MEMORY_POOL_SIZE - (int)sizeof(BlockHeader);
    free_list_head->is_free = 1;
    free_list_head->next    = (void *)0;

    is_initialized = 1;
}

/* -------------------------------------------------------------------------- */
/*  mem_alloc                                                                */
/*  Finds the first free block large enough to hold 'size' bytes.           */
/*                                                                           */
/*  If the found block is significantly larger than needed, it is split     */
/*  into two blocks: one for this allocation, and one for the remainder.    */
/*                                                                           */
/*  Returns a pointer to the usable payload (just past the header),         */
/*  or NULL if no suitable block exists.                                     */
/* -------------------------------------------------------------------------- */
void *mem_alloc(int size)
{
    BlockHeader *current;
    BlockHeader *new_block;
    int          remaining;
    /* Minimum leftover to justify splitting (header + at least 16 bytes). */
    int          min_split = (int)sizeof(BlockHeader) + 16;

    if (!is_initialized || size <= 0)
        return (void *)0;

    /* Walk the linked list looking for the first fit */
    current = free_list_head;
    while (current != (void *)0)
    {
        if (current->is_free && current->size >= size)
        {
            /* Check if we should split this block */
            remaining = current->size - size - (int)sizeof(BlockHeader);

            if (remaining >= min_split)
            {
                /* Create a new free block after our allocation */
                new_block = (BlockHeader *)((char *)current
                            + (int)sizeof(BlockHeader) + size);

                new_block->size    = remaining;
                new_block->is_free = 1;
                new_block->next    = current->next;

                /* Shrink the current block to the requested size */
                current->size = size;
                current->next = new_block;
            }

            /* Mark the block as allocated */
            current->is_free = 0;

            /* Return a pointer to the payload (just past the header) */
            return (char *)current + sizeof(BlockHeader);
        }
        current = current->next;
    }

    /* No suitable block found */
    return (void *)0;
}

/* -------------------------------------------------------------------------- */
/*  mem_free                                                                 */
/*  Marks the given block as free, then walks the entire list to merge      */
/*  any adjacent free blocks (coalescing).  This prevents fragmentation.    */
/* -------------------------------------------------------------------------- */
void mem_free(void *ptr)
{
    BlockHeader *header;
    BlockHeader *current;

    if (ptr == (void *)0 || !is_initialized)
        return;

    /* The header is located just before the payload pointer */
    header = (BlockHeader *)((char *)ptr - sizeof(BlockHeader));

    /* Basic sanity check: is the pointer inside our pool? */
    if ((char *)header < memory_pool
        || (char *)header >= memory_pool + MEMORY_POOL_SIZE)
    {
        return;  /* Not our memory -- ignore silently */
    }

    /* Mark the block as free */
    header->is_free = 1;

    /* ---- Coalesce adjacent free blocks ---- */
    /* Walk the list and merge any consecutive free blocks.                  */
    /* This keeps the pool from fragmenting over time.                       */
    current = free_list_head;
    while (current != (void *)0 && current->next != (void *)0)
    {
        if (current->is_free && current->next->is_free)
        {
            /* Absorb the next block into the current one */
            current->size += (int)sizeof(BlockHeader) + current->next->size;
            current->next  = current->next->next;
            /* Don't advance -- check if the *new* next is also free */
        }
        else
        {
            current = current->next;
        }
    }
}

/* -------------------------------------------------------------------------- */
/*  mem_print_debug                                                          */
/*  Prints every block in the pool: its offset, size, and free status.      */
/*  Useful for verifying that alloc/free are working correctly.             */
/* -------------------------------------------------------------------------- */
void mem_print_debug(void)
{
    BlockHeader *current;
    int          block_num = 0;

    if (!is_initialized)
    {
        printf("[MEMORY] Pool not initialized.\n");
        return;
    }

    printf("\n===== MEMORY POOL DEBUG =====\n");
    current = free_list_head;
    while (current != (void *)0)
    {
        printf("  Block #%d | Offset: %5ld | Size: %5d bytes | %s\n",
               block_num,
               (long)((char *)current - memory_pool),
               current->size,
               current->is_free ? "FREE" : "USED");
        current = current->next;
        block_num++;
    }
    printf("=============================\n\n");
}
