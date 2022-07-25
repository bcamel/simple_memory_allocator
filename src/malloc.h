#if !defined(MALLOC_H)
#define MALLOC_H

#include <stddef.h>

void *sma_malloc(size_t size);
void *sma_calloc(size_t num, size_t nsize);
void *sma_realloc(void *block, size_t size);
void sma_free(void *block);

#endif