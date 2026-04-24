#ifndef CUSTOM_MEMORY_H
#define CUSTOM_MEMORY_H

#define MEMORY_POOL_SIZE (64 * 1024)

typedef struct BlockHeader
{
    int                  size;
    int                  is_free;
    struct BlockHeader  *next;
} BlockHeader;

void  mem_init(void);
void *mem_alloc(int size);
void  mem_free(void *ptr);
void  mem_print_debug(void);

#endif
