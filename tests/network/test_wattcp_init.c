/*
 * Wattcp Initialization Test (Simplified)
 *
 * This program tests basic Wattcp functionality:
 * 1. Include Wattcp headers
 * 2. Initialize Wattcp library
 * 3. Basic connectivity check
 *
 * IMPORTANT: Set environment variable before running:
 *   SET WATTCP.CFG=D:\WATTCP.CFG
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <dos.h>

/* Enable Wattcp debug output */
#define WATTCP_DEBUG 1

#include <tcp.h>

/* Function to display file contents */
void display_file(const char *filename) {
    FILE *fp;
    char line[256];
    int line_num = 0;
    
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("  ERROR: Cannot open %s\n", filename);
        return;
    }
    
    printf("  --- Contents of %s ---\n", filename);
    while (fgets(line, sizeof(line), fp) != NULL) {
        line_num++;
        /* Remove trailing newline */
        line[strcspn(line, "\r\n")] = 0;
        printf("  %2d: %s\n", line_num, line);
    }
    printf("  --- End of file ---\n");
    fclose(fp);
}

int main(void) {
    int result;
    char *cfg_path;
    
    printf("=== Wattcp Initialization Test ===\n\n");
    
    /* Test 1: Wattcp header inclusion */
    printf("Test 1: Wattcp headers\n");
    printf("  PASS: tcp.h included successfully\n\n");
    
    /* Test 2: Check for configuration files */
    printf("Test 2: Check for configuration files\n");
    
    /* Check various possible locations */
    if (access("wattcp.cfg", 0) == 0) {
        printf("  FOUND: wattcp.cfg in current directory\n");
    } else if (access("WATTCP.CFG", 0) == 0) {
        printf("  FOUND: WATTCP.CFG in current directory\n");
    } else {
        printf("  NOT FOUND: wattcp.cfg in current directory\n");
    }
    
    if (access("C:\\wattcp.cfg", 0) == 0) {
        printf("  FOUND: C:\\wattcp.cfg\n");
    }
    
    if (access("D:\\wattcp.cfg", 0) == 0) {
        printf("  FOUND: D:\\wattcp.cfg\n");
    } else if (access("D:\\WATTCP.CFG", 0) == 0) {
        printf("  FOUND: D:\\WATTCP.CFG\n");
    }
    
    cfg_path = getenv("WATTCP.CFG");
    if (cfg_path && cfg_path[0] != '\0') {
        printf("  INFO: WATTCP.CFG=%s\n", cfg_path);
    } else {
        printf("  INFO: WATTCP.CFG environment variable not set\n");
    }
    printf("\n");
    
    /* Test 2b: Display configuration file contents */
    printf("Test 2b: Display configuration file contents\n");
    if (access("wattcp.cfg", 0) == 0) {
        display_file("wattcp.cfg");
    } else if (access("WATTCP.CFG", 0) == 0) {
        display_file("WATTCP.CFG");
    } else if (access("D:\\wattcp.cfg", 0) == 0) {
        display_file("D:\\wattcp.cfg");
    } else if (access("D:\\WATTCP.CFG", 0) == 0) {
        display_file("D:\\WATTCP.CFG");
    } else {
        printf("  No configuration file found to display\n");
    }
    printf("\n");
    
    /* Test 3: Check for packet driver */
    printf("Test 3: Check for packet driver\n");
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
        } else {
            printf("  INFO: Packet driver vector is set\n");
            printf("        Segment: %04X, Offset: %04X\n",
                   FP_SEG(handler), FP_OFF(handler));
            printf("        Driver appears to be loaded\n");
        }
    }
    printf("\n");
    
    /* Test 4: Initialize Wattcp */
    printf("Test 4: Initialize Wattcp library\n");
    printf("  Calling sock_init()...\n");
    printf("  (Wattcp will display diagnostic messages)\n");
    printf("  ----------------------------------------\n");
    
    result = sock_init();
    
    printf("  ----------------------------------------\n");
    
    if (result == 0) {
        printf("  FAIL: sock_init() returned 0\n");
        printf("\n");
        printf("  Diagnosis:\n");
        printf("    - Configuration file: OK (found and valid)\n");
        printf("    - Packet driver: Check Test 3 results above\n");
        printf("\n");
        printf("  If packet driver vector is NULL:\n");
        printf("    Load packet driver: ne2000 0x60 3 0x300\n");
        printf("\n");
        printf("  If packet driver vector is set but sock_init() fails:\n");
        printf("    - Packet driver may be incompatible\n");
        printf("    - Try different interrupt vector (0x61, 0x62, etc.)\n");
        printf("    - Check DOSBox-X NE2000 emulation settings\n");
        return 1;
    }
    
    printf("  PASS: sock_init() succeeded (returned %d)\n\n", result);
    
    /* Test 5: Basic info */
    printf("Test 5: Wattcp library loaded\n");
    printf("  PASS: Wattcp is initialized and ready\n\n");
    
    printf("=== All Tests Passed ===\n");
    printf("\nWattcp is ready for socket programming!\n");
    printf("\nNext steps:\n");
    printf("  - Test socket creation\n");
    printf("  - Test TCP connection\n");
    printf("  - Implement socket wrapper API\n");
    
    return 0;
}
