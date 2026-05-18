/*
 * test_memory_profile.c - Memory profiling for socket implementation
 * 
 * Measures memory usage of socket operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "../../src/network/socket.h"

#ifdef __cplusplus
}
#endif

/* Get available memory (DOS specific) */
unsigned long get_available_memory(void) {
    unsigned long total = 0;
    
    /* Use DOS function to get largest free block */
    union REGS regs;
    regs.h.ah = 0x48;  /* Allocate memory */
    regs.x.bx = 0xFFFF; /* Request impossible size to get largest */
    int86(0x21, &regs, &regs);
    
    if (regs.x.cflag) {
        /* BX now contains size of largest free block in paragraphs */
        total = (unsigned long)regs.x.bx * 16;
    }
    
    return total;
}

/* Print memory statistics */
void print_memory_stats(const char* label) {
    unsigned long avail = get_available_memory();
    printf("%s: %lu bytes available\n", label, avail);
}

/* Test memory usage patterns */
int main(void) {
    socket_handle_t socks[MAX_SOCKETS];
    int i, rc;
    unsigned long mem_before, mem_after, mem_used;
    
    printf("=== Socket Memory Profiling ===\n\n");
    
    /* Baseline memory */
    print_memory_stats("Baseline");
    mem_before = get_available_memory();
    
    /* Initialize socket subsystem */
    printf("\n1. Initializing socket subsystem...\n");
    rc = socket_init();
    if (rc != SOCKET_OK) {
        printf("Error: %s\n", socket_get_error());
        return 1;
    }
    mem_after = get_available_memory();
    mem_used = mem_before - mem_after;
    printf("   Memory used by init: %lu bytes\n", mem_used);
    print_memory_stats("After init");
    
    /* Create maximum sockets */
    printf("\n2. Creating %d sockets...\n", MAX_SOCKETS);
    mem_before = get_available_memory();
    for (i = 0; i < MAX_SOCKETS; i++) {
        socks[i] = socket_create(SOCKET_TYPE_TCP);
        if (socks[i] == INVALID_SOCKET) {
            printf("   Failed to create socket %d: %s\n", i, socket_get_error());
            break;
        }
        printf("   Socket %d created (handle=%d)\n", i, socks[i]);
    }
    mem_after = get_available_memory();
    mem_used = mem_before - mem_after;
    printf("   Memory used by %d sockets: %lu bytes\n", i, mem_used);
    if (i > 0) {
        printf("   Average per socket: %lu bytes\n", mem_used / i);
    }
    print_memory_stats("After socket creation");
    
    /* Close all sockets */
    printf("\n3. Closing all sockets...\n");
    mem_before = get_available_memory();
    for (i = 0; i < MAX_SOCKETS; i++) {
        if (socks[i] != INVALID_SOCKET) {
            socket_close(socks[i]);
        }
    }
    mem_after = get_available_memory();
    printf("   Memory freed: %ld bytes\n", (long)(mem_after - mem_before));
    print_memory_stats("After socket close");
    
    /* Shutdown */
    printf("\n4. Shutting down socket subsystem...\n");
    mem_before = get_available_memory();
    socket_shutdown();
    mem_after = get_available_memory();
    printf("   Memory freed: %ld bytes\n", (long)(mem_after - mem_before));
    print_memory_stats("After shutdown");
    
    /* Summary */
    printf("\n=== Memory Profile Summary ===\n");
    printf("Maximum concurrent sockets: %d\n", MAX_SOCKETS);
    printf("TCP receive buffer size: 4096 bytes\n");
    printf("Socket structure size: %u bytes\n", sizeof(void*) * 5 + sizeof(int) * 3);
    
    printf("\nMemory optimization recommendations:\n");
    printf("- Current implementation uses mTCP's internal buffers\n");
    printf("- TCP_RECV_BUFFER = 4096 bytes per socket\n");
    printf("- MAX_SOCKETS = %d (configurable in socket.h)\n", MAX_SOCKETS);
    printf("- Total socket overhead: ~%lu bytes\n", 
           (unsigned long)MAX_SOCKETS * 4096);
    
    return 0;
}

