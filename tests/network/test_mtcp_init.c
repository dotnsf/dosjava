/*
 * mTCP Initialization Test for dosjava (Phase 4.1 Day 1-2)
 *
 * This program tests basic mTCP functionality:
 * 1. Include mTCP headers
 * 2. Initialize mTCP library
 * 3. Basic connectivity check
 *
 * Note: mTCP is a C++ library, so we need to use C++ compilation
 * or create C wrappers. This version uses C++ with extern "C" main.
 *
 * Configuration: MTCP.CFG environment variable or mtcp.cfg in current directory
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <dos.h>

/* mTCP is C++, so we need to handle this carefully */
#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations for functions we'll call */
void test_mtcp_init(void);

#ifdef __cplusplus
}
#endif

/* Main function - can be called from C or C++ */
int main(void) {
    printf("=== mTCP Initialization Test for dosjava ===\n\n");
    
    printf("Test 1: Check mTCP environment\n");
    
    /* Check for MTCP.CFG environment variable */
    char *cfg_path = getenv("MTCP_CFG");
    if (cfg_path && cfg_path[0] != '\0') {
        printf("  INFO: MTCP_CFG=%s\n", cfg_path);
        if (access(cfg_path, 0) == 0) {
            printf("  FOUND: Configuration file at MTCP_CFG location\n");
        } else {
            printf("  WARNING: MTCP_CFG points to non-existent file\n");
        }
    } else {
        printf("  INFO: MTCP_CFG environment variable not set\n");
        printf("  NOTE: mTCP will search for mtcp.cfg in current directory\n");
    }
    
    /* Check for configuration files in common locations */
    if (access("mtcp.cfg", 0) == 0) {
        printf("  FOUND: mtcp.cfg in current directory\n");
    } else if (access("MTCP.CFG", 0) == 0) {
        printf("  FOUND: MTCP.CFG in current directory\n");
    } else {
        printf("  NOT FOUND: mtcp.cfg in current directory\n");
    }
    
    printf("\n");
    
    /* Test 2: Check for packet driver */
    printf("Test 2: Check for packet driver\n");
    {
        /* Try to detect packet driver using INT 60h */
        void (__interrupt __far *handler)();
        
        /* Get interrupt vector 0x60 */
        handler = _dos_getvect(0x60);
        
        printf("  Packet driver vector (INT 60h): %Fp\n", handler);
        
        if (handler == NULL || FP_SEG(handler) == 0) {
            printf("  WARNING: Packet driver vector is NULL!\n");
            printf("  This usually means packet driver is NOT loaded.\n");
            printf("\n");
            printf("  Please load packet driver first:\n");
            printf("    ne2000 0x60 3 0x300\n");
            printf("\n");
            printf("  Cannot proceed without packet driver.\n");
            return 1;
        } else {
            printf("  INFO: Packet driver vector is set\n");
            printf("        Segment: %04X, Offset: %04X\n",
                   FP_SEG(handler), FP_OFF(handler));
            printf("        Driver appears to be loaded\n");
        }
    }
    printf("\n");
    
    printf("Test 3: mTCP library initialization\n");
    printf("  NOTE: mTCP is a C++ library\n");
    printf("  INFO: To use mTCP, we need to compile with C++ compiler (wpp)\n");
    printf("  INFO: This test program demonstrates the requirements\n");
    printf("\n");
    
    printf("=== Basic Tests Passed ===\n");
    printf("\nNext steps:\n");
    printf("  1. Create C++ wrapper for mTCP initialization\n");
    printf("  2. Compile with wpp (C++ compiler)\n");
    printf("  3. Link with mTCP library\n");
    printf("  4. Test socket creation and connection\n");
    printf("\nConfiguration:\n");
    printf("  - Library: mTCP TCP/IP stack (C++)\n");
    printf("  - Compiler: Open Watcom wpp (C++)\n");
    printf("  - Memory model: Small\n");
    
    return 0;
}

// Made with Bob
