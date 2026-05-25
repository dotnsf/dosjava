#ifndef MEMORY_H
#define MEMORY_H

#include "../types.h"

/**
 * Memory Manager for DOS Java VM
 *
 * Simple heap allocator for Large memory model
 * Uses a free list for memory management
 */

/* Default heap size (in bytes) */
#define DEFAULT_HEAP_SIZE 49152  /* 48KB for heap - expanded for string operations */

/* Minimum allocation size */
#define MIN_ALLOC_SIZE 4

/* Memory block header */
#pragma pack(push, 1)
typedef struct MemBlock {
    uint16_t size;           /* Block size (including header) */
    uint8_t used;            /* 1 if allocated, 0 if free */
    struct MemBlock* next;   /* Next block in list */
} MemBlock;
#pragma pack(pop)

/**
 * Memory manager state
 */
typedef struct {
    uint8_t* heap_start;     /* Start of heap */
    uint16_t heap_size;      /* Total heap size */
    uint16_t bytes_used;     /* Bytes currently allocated */
    uint16_t bytes_free;     /* Bytes available */
    MemBlock* free_list;     /* Free block list */
    uint16_t alloc_count;    /* Number of allocations */
    uint16_t free_count;     /* Number of frees */
} MemoryManager;

/* Handle table for Large memory model compatibility */
#define MAX_ARRAY_HANDLES 256
#define MAX_OBJECT_HANDLES 256

typedef struct {
    void* array_ptr;         /* Pointer to array data */
    uint8_t in_use;          /* 1 if handle is active, 0 if free */
} ArrayHandle;

typedef struct {
    void* object_ptr;        /* Pointer to object data */
    uint8_t in_use;          /* 1 if handle is active, 0 if free */
} ObjectHandle;

/**
 * Initialize memory manager
 * @param heap_size Size of heap in bytes (0 = use default)
 * @return 0 on success, -1 on error
 */
int memory_init(uint16_t heap_size);

/**
 * Shutdown memory manager and free all resources
 */
void memory_shutdown(void);

/**
 * Allocate memory from heap
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on error
 */
void* memory_alloc(uint16_t size);

/**
 * Free previously allocated memory
 * @param ptr Pointer to memory to free
 */
void memory_free(void* ptr);

/**
 * Reallocate memory block
 * @param ptr Pointer to existing block (or NULL)
 * @param new_size New size in bytes
 * @return Pointer to reallocated memory, or NULL on error
 */
void* memory_realloc(void* ptr, uint16_t new_size);

/**
 * Get amount of free memory
 * @return Number of free bytes
 */
uint16_t memory_available(void);

/**
 * Get amount of used memory
 * @return Number of used bytes
 */
uint16_t memory_used(void);

/**
 * Get total heap size
 * @return Total heap size in bytes
 */
uint16_t memory_total(void);

/**
 * Print memory statistics (for debugging)
 */
void memory_stats(void);

/**
 * Defragment heap by coalescing adjacent free blocks
 * @return Number of blocks coalesced
 */
uint16_t memory_defrag(void);

/**
 * Check heap integrity (for debugging)
 * @return 1 if heap is valid, 0 if corrupted
 */
uint8_t memory_check(void);

/**
 * Get memory manager instance
 * @return Pointer to memory manager
 */
MemoryManager* memory_get_manager(void);

/**
 * Allocate array handle for pointer
 * @param array_ptr Pointer to array data
 * @return Handle (1-255), or 0 on error
 */
uint16_t memory_alloc_array_handle(void* array_ptr);

/**
 * Get array pointer from handle
 * @param handle Array handle (1-255)
 * @return Pointer to array data, or NULL if invalid
 */
void* memory_get_array_ptr(uint16_t handle);

/**
 * Free array handle
 * @param handle Array handle to free
 */
void memory_free_array_handle(uint16_t handle);

/**
 * Allocate object handle for pointer
 * @param object_ptr Pointer to object data
 * @return Handle (1-255), or 0 on error
 */
uint16_t memory_alloc_object_handle(void* object_ptr);

/**
 * Get object pointer from handle
 * @param handle Object handle (1-255)
 * @return Pointer to object data, or NULL if invalid
 */
void* memory_get_object_ptr(uint16_t handle);

/**
 * Free object handle
 * @param handle Object handle to free
 */
void memory_free_object_handle(uint16_t handle);

#endif /* MEMORY_H */


