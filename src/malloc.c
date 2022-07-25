#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "malloc.h"
#include "memhdr.h"
#include "logger.h"

header_t *head = NULL;
header_t *tail = NULL;
pthread_mutex_t global_malloc_lock;

static void print_mem_list()
{
    header_t *curr = head;
    log_trace("\thead = %p, tail = %p", (void*)head, (void*)tail);
    while(curr) 
    {
        log_trace("\t\ttaddr = %p, size = %zu, is_free=%u, next=%p",
                  (void*)curr,
                  curr->s.size,
                  curr->s.is_free,
                  (void*)curr->s.next);
        curr = curr->s.next;
    }
}

static header_t *get_free_block(size_t size)
{
    print_mem_list();
    header_t *curr = head;
    while(curr)
    {
        if (curr->s.is_free && curr->s.size >= size)
        {
            return curr;
        }
        curr = curr->s.next;
    }
    return NULL;
}

void *sma_malloc(size_t size)
{
    size_t total_size;
    void *block;
    header_t *header;

    log_trace("requested \x1b[92m%lu\x1b[0m length block", size);

    if (!size)
    {
        return NULL;
    }
    
    pthread_mutex_lock(&global_malloc_lock);
    header = get_free_block(size);
    if (header)
    {
        header->s.is_free = false;
        pthread_mutex_unlock(&global_malloc_lock);
        /* We have to hide the very existence of the header to an outside party.
           When we do (header + 1), it points to the byte right after the end of 
           the header. This is incidentally also the first byte of the actual memory
           block, the one the caller is interested in. This is cast to (void*) 
           and returned. */
        return (void*)(header + 1);
    }

    total_size = sizeof(header_t) + size;
    block = sbrk(total_size);
    if (block == (void*) - 1)
    {
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }
    
    /* In C, there is no need to cast a void* to any other pointer type, 
       it is always safely promoted. */
    header = block;
    header->s.size = size;
    header->s.is_free = false;
    header->s.next = NULL;

    if (!head)
    {
        head = header;
    }
    if (tail)
    {
        tail->s.next = header;
    }
    tail = header;
    pthread_mutex_unlock(&global_malloc_lock);
    return (void*)(header + 1);
}

void *sma_calloc(size_t num, size_t nsize)
{
    size_t size;
    void *block;

    if (!num || !nsize)
    {
        return NULL;
    }    

    size = num * nsize;
    /* check overflow */
    if (nsize != size / num)
    {
        return NULL;
    }
    block = sma_malloc(size);
    if (!block)
    {
        return NULL;
    }
    memset(block, 0, size);
    return block;
}

void *sma_realloc(void *block, size_t size)
{
    header_t *header;
    void *ret;

    if (!block || !size)
    {
        return sma_malloc(size);
    }

    header = (header_t*)block - 1;
    if (header->s.size >= size)
    {
        return block;
    }
    ret = sma_malloc(size);
    if (ret)
    {
        memcpy(ret, block, header->s.size);
        sma_free(block);
    }
    return ret;
}

void sma_free(void *block)
{
    header_t *header;
    header_t *tmp;
    void *prg_brk;

    log_trace("free block at \x1b[90m\x1b[102m%p\x1b[0m", block);

    if(!block)
    {
        return;
    }

    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t*)block - 1;

    prg_brk = sbrk(0);
    if((char*)block + header->s.size == prg_brk)
    {
        if (head == tail)
        {
            head = tail = NULL;
        }
        else
        {
            tmp = head;
            while(tmp)
            {
                if (tmp->s.next == tail)
                {
                    tmp->s.next = NULL;
                    tail = tmp;
                }
                tmp = tmp->s.next;
            }    
        }
        sbrk(0 - sizeof(header_t) - header->s.size);
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    header->s.is_free = true;
    pthread_mutex_unlock(&global_malloc_lock);
}
