#include "memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Global memory manager instance */
static MemoryManager g_memory_manager;
static uint8_t g_memory_initialized = 0;

/**
 * Align size to 2-byte boundary for 16-bit efficiency
 */
static uint16_t align_size(uint16_t size) {
    return (size + 1) & ~1;
}

/**
 * Initialize memory manager
 */
int memory_init(uint16_t heap_size) {
    if (g_memory_initialized) {
        return 0;  /* Already initialized */
    }
    
    /* Use default size if not specified */
    if (heap_size == 0) {
        heap_size = DEFAULT_HEAP_SIZE;
    }
    
    /* Allocate heap */
    g_memory_manager.heap_start = (uint8_t*)malloc(heap_size);
    if (g_memory_manager.heap_start == NULL) {
        return -1;
    }
    
    /* Initialize manager state */
    g_memory_manager.heap_size = heap_size;
    g_memory_manager.bytes_used = sizeof(MemBlock);
    g_memory_manager.bytes_free = heap_size - sizeof(MemBlock);
    g_memory_manager.alloc_count = 0;
    g_memory_manager.free_count = 0;
    
    /* Create initial free block */
    g_memory_manager.free_list = (MemBlock*)g_memory_manager.heap_start;
    g_memory_manager.free_list->size = heap_size;
    g_memory_manager.free_list->used = 0;
    g_memory_manager.free_list->next = NULL;
    
    g_memory_initialized = 1;
    return 0;
}

/**
 * Shutdown memory manager
 */
void memory_shutdown(void) {
    if (!g_memory_initialized) {
        return;
    }
    
    if (g_memory_manager.heap_start != NULL) {
        free(g_memory_manager.heap_start);
        g_memory_manager.heap_start = NULL;
    }
    
    g_memory_initialized = 0;
}

/**
 * Allocate memory from heap
 */
void* memory_alloc(uint16_t size) {
    MemBlock* block;
    MemBlock* prev;
    MemBlock* new_block;
    uint16_t total_size;
    
    if (!g_memory_initialized || size == 0) {
        return NULL;
    }
    
    /* Align size and add header */
    size = align_size(size);
    total_size = size + sizeof(MemBlock);
    
    /* Ensure minimum allocation size */
    if (total_size < MIN_ALLOC_SIZE + sizeof(MemBlock)) {
        total_size = MIN_ALLOC_SIZE + sizeof(MemBlock);
    }
    
    /* Find first fit in free list */
    prev = NULL;
    block = g_memory_manager.free_list;
    
    while (block != NULL) {
        if (!block->used && block->size >= total_size) {
            /* Found suitable block */
            
            /* Split block if large enough */
            if (block->size >= total_size + sizeof(MemBlock) + MIN_ALLOC_SIZE) {
                new_block = (MemBlock*)((uint8_t*)block + total_size);
                new_block->size = block->size - total_size;
                new_block->used = 0;
                new_block->next = block->next;
                
                block->size = total_size;
                block->next = new_block;
            }
            
            /* Mark block as used */
            block->used = 1;
            
            /* Update statistics */
            g_memory_manager.bytes_used += block->size;
            g_memory_manager.bytes_free -= block->size;
            g_memory_manager.alloc_count++;
            
            /* Return pointer after header */
            return (void*)((uint8_t*)block + sizeof(MemBlock));
        }
        
        prev = block;
        block = block->next;
    }
    
    /* No suitable block found */
    return NULL;
}

/**
 * Free previously allocated memory
 */
void memory_free(void* ptr) {
    MemBlock* block;
    
    if (!g_memory_initialized || ptr == NULL) {
        return;
    }
    
    /* Get block header */
    block = (MemBlock*)((uint8_t*)ptr - sizeof(MemBlock));
    
    /* Verify block is within heap */
    if ((uint8_t*)block < g_memory_manager.heap_start ||
        (uint8_t*)block >= g_memory_manager.heap_start + g_memory_manager.heap_size) {
        return;  /* Invalid pointer */
    }
    
    /* Mark block as free */
    if (block->used) {
        block->used = 0;
        
        /* Update statistics */
        g_memory_manager.bytes_used -= block->size;
        g_memory_manager.bytes_free += block->size;
        g_memory_manager.free_count++;
    }
}

/**
 * Reallocate memory block
 */
void* memory_realloc(void* ptr, uint16_t new_size) {
    MemBlock* block;
    void* new_ptr;
    uint16_t copy_size;
    
    if (!g_memory_initialized) {
        return NULL;
    }
    
    /* Handle special cases */
    if (ptr == NULL) {
        return memory_alloc(new_size);
    }
    
    if (new_size == 0) {
        memory_free(ptr);
        return NULL;
    }
    
    /* Get current block */
    block = (MemBlock*)((uint8_t*)ptr - sizeof(MemBlock));
    
    /* If new size fits in current block, just return it */
    if (new_size + sizeof(MemBlock) <= block->size) {
        return ptr;
    }
    
    /* Allocate new block */
    new_ptr = memory_alloc(new_size);
    if (new_ptr == NULL) {
        return NULL;
    }
    
    /* Copy data */
    copy_size = block->size - sizeof(MemBlock);
    if (copy_size > new_size) {
        copy_size = new_size;
    }
    memcpy(new_ptr, ptr, copy_size);
    
    /* Free old block */
    memory_free(ptr);
    
    return new_ptr;
}

/**
 * Get amount of free memory
 */
uint16_t memory_available(void) {
    if (!g_memory_initialized) {
        return 0;
    }
    return g_memory_manager.bytes_free;
}

/**
 * Get amount of used memory
 */
uint16_t memory_used(void) {
    if (!g_memory_initialized) {
        return 0;
    }
    return g_memory_manager.bytes_used;
}

/**
 * Get total heap size
 */
uint16_t memory_total(void) {
    if (!g_memory_initialized) {
        return 0;
    }
    return g_memory_manager.heap_size;
}

/**
 * Print memory statistics
 */
void memory_stats(void) {
    if (!g_memory_initialized) {
        printf("Memory manager not initialized\n");
        return;
    }
    
    printf("Memory Statistics:\n");
    printf("  Total heap:  %u bytes\n", g_memory_manager.heap_size);
    printf("  Used:        %u bytes\n", g_memory_manager.bytes_used);
    printf("  Free:        %u bytes\n", g_memory_manager.bytes_free);
    printf("  Allocations: %u\n", g_memory_manager.alloc_count);
    printf("  Frees:       %u\n", g_memory_manager.free_count);
}

/**
 * Defragment heap by coalescing adjacent free blocks
 */
uint16_t memory_defrag(void) {
    MemBlock* block;
    MemBlock* next;
    uint16_t coalesced = 0;
    
    if (!g_memory_initialized) {
        return 0;
    }
    
    block = g_memory_manager.free_list;
    
    while (block != NULL && block->next != NULL) {
        next = block->next;
        
        /* If both blocks are free and adjacent, merge them */
        if (!block->used && !next->used) {
            if ((uint8_t*)block + block->size == (uint8_t*)next) {
                block->size += next->size;
                block->next = next->next;
                coalesced++;
                continue;  /* Check again with same block */
            }
        }
        
        block = block->next;
    }
    
    return coalesced;
}

/**
 * Check heap integrity
 */
uint8_t memory_check(void) {
    MemBlock* block;
    uint16_t total_size;
    
    if (!g_memory_initialized) {
        return 0;
    }
    
    total_size = 0;
    block = g_memory_manager.free_list;
    
    while (block != NULL) {
        /* Check block is within heap */
        if ((uint8_t*)block < g_memory_manager.heap_start ||
            (uint8_t*)block >= g_memory_manager.heap_start + g_memory_manager.heap_size) {
            printf("ERROR: Block outside heap bounds\n");
            return 0;
        }
        
        /* Check block size is reasonable */
        if (block->size < sizeof(MemBlock) || block->size > g_memory_manager.heap_size) {
            printf("ERROR: Invalid block size: %u\n", block->size);
            return 0;
        }
        
        total_size += block->size;
        block = block->next;
    }
    
    /* Check total size matches heap size */
    if (total_size != g_memory_manager.heap_size) {
        printf("ERROR: Total block size (%u) != heap size (%u)\n", 
               total_size, g_memory_manager.heap_size);
        return 0;
    }
    
    return 1;
}

/**
 * Get memory manager instance
 */
MemoryManager* memory_get_manager(void) {
    if (!g_memory_initialized) {
        return NULL;
    }
    return &g_memory_manager;
}


