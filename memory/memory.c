#include <stdio.h>
#include "../memory/memory.h"

static char memory_pool[MEMORY_POOL_SIZE];
static BlockHeader *free_list_head = (void *)0;
static int is_initialized = 0;

void mem_init(void)
{
    free_list_head = (BlockHeader *)memory_pool;
    free_list_head->size = MEMORY_POOL_SIZE - (int)sizeof(BlockHeader);
    free_list_head->is_free = 1;
    free_list_head->next = (void *)0;
    is_initialized = 1;
}

void *mem_alloc(int size)
{
    BlockHeader *current;
    BlockHeader *new_block;
    int remaining;
    int min_split = (int)sizeof(BlockHeader) + 16;

    if (!is_initialized || size <= 0)
        return (void *)0;

    current = free_list_head;
    while (current != (void *)0)
    {
        if (current->is_free && current->size >= size)
        {
            remaining = current->size - size - (int)sizeof(BlockHeader);
            if (remaining >= min_split)
            {
                new_block = (BlockHeader *)((char *)current
                            + (int)sizeof(BlockHeader) + size);
                new_block->size = remaining;
                new_block->is_free = 1;
                new_block->next = current->next;
                current->size = size;
                current->next = new_block;
            }
            current->is_free = 0;
            return (char *)current + sizeof(BlockHeader);
        }
        current = current->next;
    }
    return (void *)0;
}

void mem_free(void *ptr)
{
    BlockHeader *header;
    BlockHeader *current;

    if (ptr == (void *)0 || !is_initialized)
        return;

    header = (BlockHeader *)((char *)ptr - sizeof(BlockHeader));
    if ((char *)header < memory_pool
        || (char *)header >= memory_pool + MEMORY_POOL_SIZE)
        return;

    header->is_free = 1;

    /* Coalesce adjacent free blocks */
    current = free_list_head;
    while (current != (void *)0 && current->next != (void *)0)
    {
        if (current->is_free && current->next->is_free)
        {
            current->size += (int)sizeof(BlockHeader) + current->next->size;
            current->next = current->next->next;
        }
        else
            current = current->next;
    }
}

void mem_print_debug(void)
{
    BlockHeader *current;
    int block_num = 0;

    if (!is_initialized)
    {
        printf("[MEMORY] Pool not initialized.\n");
        return;
    }

    printf("\n===== MEMORY POOL DEBUG =====\n");
    current = free_list_head;
    while (current != (void *)0)
    {
        printf("  Block #%d | Offset: %5ld | Size: %5d | %s\n",
               block_num,
               (long)((char *)current - memory_pool),
               current->size,
               current->is_free ? "FREE" : "USED");
        current = current->next;
        block_num++;
    }
    printf("=============================\n\n");
}
