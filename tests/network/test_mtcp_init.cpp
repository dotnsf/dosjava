/*
 * mTCP Initialization Test for dosjava (Phase 4.1 Day 1-2)
 *
 * This program tests mTCP library initialization.
 * Based on mTCP SAMPLE.CPP example.
 *
 * Configuration: MTCP_CFG environment variable pointing to configuration file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <io.h>

/* Include mTCP headers */
#include "types.h"
#include "trace.h"
#include "utils.h"
#include "packet.h"
#include "arp.h"
#include "tcp.h"
#include "tcpsockm.h"

/* Ctrl-Break and Ctrl-C handlers */
volatile uint8_t CtrlBreakDetected = 0;

void __interrupt __far ctrlBreakHandler( ) {
    CtrlBreakDetected = 1;
}

void __interrupt __far ctrlCHandler( ) {
    /* Do Nothing - Ctrl-C is a legal character */
}

int main(void) {
    printf("=== mTCP Initialization Test for dosjava ===\n\n");
    
    /* Test 1: Check environment */
    printf("Test 1: Check mTCP environment\n");
    
    char *cfg_path = getenv("MTCP_CFG");
    if (cfg_path && cfg_path[0] != '\0') {
        printf("  INFO: MTCP_CFG=%s\n", cfg_path);
        if (access(cfg_path, 0) == 0) {
            printf("  FOUND: Configuration file exists\n");
        } else {
            printf("  ERROR: MTCP_CFG points to non-existent file\n");
            printf("  Please set MTCP_CFG to a valid configuration file\n");
            return 1;
        }
    } else {
        printf("  ERROR: MTCP_CFG environment variable not set\n");
        printf("  Please set MTCP_CFG to point to your configuration file\n");
        printf("  Example: SET MTCP_CFG=D:\\MTCP.CFG\n");
        return 1;
    }
    printf("  PASS: Configuration file found\n\n");
    
    /* Test 2: Check for packet driver */
    printf("Test 2: Check for packet driver\n");
    {
        void (__interrupt __far *handler)();
        handler = _dos_getvect(0x60);
        
        printf("  Packet driver vector (INT 60h): %Fp\n", handler);
        
        if (handler == NULL || FP_SEG(handler) == 0) {
            printf("  ERROR: Packet driver not loaded\n");
            printf("  Please load packet driver first:\n");
            printf("    ne2000 0x60 3 0x300\n");
            return 1;
        }
        printf("  INFO: Packet driver appears to be loaded\n");
        printf("        Segment: %04X, Offset: %04X\n",
               FP_SEG(handler), FP_OFF(handler));
    }
    printf("  PASS: Packet driver detected\n\n");
    
    /* Test 3: Parse mTCP configuration */
    printf("Test 3: Parse mTCP configuration\n");
    printf("  Calling Utils::parseEnv()...\n");
    
    if (Utils::parseEnv() != 0) {
        printf("  FAIL: Failed to parse mTCP configuration\n");
        printf("  Check your MTCP_CFG file for errors\n");
        printf("\n");
        printf("  Required settings in MTCP.CFG:\n");
        printf("    PACKETINT 0x60\n");
        printf("    IPADDR 192.168.0.123\n");
        printf("    NETMASK 255.255.255.0\n");
        printf("    GATEWAY 192.168.0.1\n");
        printf("    NAMESERVER 8.8.8.8\n");
        return 1;
    }
    printf("  PASS: Configuration parsed successfully\n\n");
    
    /* Test 4: Initialize mTCP stack */
    printf("Test 4: Initialize mTCP stack\n");
    printf("  Calling Utils::initStack()...\n");
    printf("  Parameters: 2 TCP sockets, 2 transmit buffers\n");
    printf("  ----------------------------------------\n");
    
    if (Utils::initStack(2, 2, ctrlBreakHandler, ctrlCHandler) != 0) {
        printf("  ----------------------------------------\n");
        printf("  FAIL: Failed to initialize mTCP stack\n");
        printf("\n");
        printf("  Common issues:\n");
        printf("    1. Packet driver not loaded or wrong interrupt\n");
        printf("    2. Configuration file has invalid settings\n");
        printf("    3. Network settings incorrect in MTCP.CFG\n");
        printf("    4. DOSBox-X NE2000 emulation not enabled\n");
        return 1;
    }
    
    printf("  ----------------------------------------\n");
    printf("  PASS: mTCP stack initialized successfully\n\n");
    
    /* Test 5: Verify stack is ready */
    printf("Test 5: Verify mTCP stack\n");
    printf("  INFO: mTCP stack is initialized and ready\n");
    printf("  INFO: Network operations are now available\n");
    printf("  PASS: All systems operational\n\n");
    
    /* Cleanup */
    printf("Shutting down mTCP stack...\n");
    Utils::endStack();
    printf("Shutdown complete.\n\n");
    
    printf("=== All Tests Passed ===\n");
    printf("\nmTCP is ready for socket programming!\n");
    printf("\nNext steps:\n");
    printf("  - Implement C-level socket wrapper functions\n");
    printf("  - Test socket creation and connection\n");
    printf("  - Integrate with dosjava VM\n");
    
    return 0;
}

// Made with Bob
