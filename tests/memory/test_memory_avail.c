/*
 * test_memory_avail.c - Test memory availability in DOS environment
 * 
 * This program checks available memory before any VM initialization
 * to establish a baseline for comparison with djvm.
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int main(void) {
    void *p1, *p2, *p3, *p4;
    
    printf("=== Memory Availability Test ===\n\n");
    
    /* Initial memory state */
    printf("Initial state:\n");
    printf("  Available memory: %lu bytes\n", (unsigned long)_memavl());
    printf("  Largest free block: %lu bytes\n\n", (unsigned long)_memmax());
    
    /* Test 1KB allocation */
    printf("Test 1: Allocating 1KB...\n");
    p1 = malloc(1024);
    if (p1) {
        printf("  Success! Address: %p\n", p1);
        printf("  Available memory: %lu bytes\n", (unsigned long)_memavl());
        printf("  Largest free block: %lu bytes\n\n", (unsigned long)_memmax());
        free(p1);
    } else {
        printf("  FAILED!\n\n");
    }
    
    /* Test 8KB allocation */
    printf("Test 2: Allocating 8KB...\n");
    p2 = malloc(8192);
    if (p2) {
        printf("  Success! Address: %p\n", p2);
        printf("  Available memory: %lu bytes\n", (unsigned long)_memavl());
        printf("  Largest free block: %lu bytes\n\n", (unsigned long)_memmax());
        free(p2);
    } else {
        printf("  FAILED!\n\n");
    }
    
    /* Test 16KB allocation */
    printf("Test 3: Allocating 16KB...\n");
    p3 = malloc(16384);
    if (p3) {
        printf("  Success! Address: %p\n", p3);
        printf("  Available memory: %lu bytes\n", (unsigned long)_memavl());
        printf("  Largest free block: %lu bytes\n\n", (unsigned long)_memmax());
        free(p3);
    } else {
        printf("  FAILED!\n\n");
    }
    
    /* Test 32KB allocation */
    printf("Test 4: Allocating 32KB...\n");
    p4 = malloc(32768);
    if (p4) {
        printf("  Success! Address: %p\n", p4);
        printf("  Available memory: %lu bytes\n", (unsigned long)_memavl());
        printf("  Largest free block: %lu bytes\n\n", (unsigned long)_memmax());
        free(p4);
    } else {
        printf("  FAILED!\n\n");
    }
    
    /* Final memory state */
    printf("Final state (after all frees):\n");
    printf("  Available memory: %lu bytes\n", (unsigned long)_memavl());
    printf("  Largest free block: %lu bytes\n", (unsigned long)_memmax());
    
    return 0;
}

// Made with Bob
