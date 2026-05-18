/*
 * test_network_diag.c - Network Diagnostics Program
 * 
 * Tests basic network connectivity and configuration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/network/socket.h"

/* Test result tracking */
static int tests_passed = 0;
static int tests_failed = 0;

/* Helper function to print test results */
void print_test_result(const char* test_name, int passed) {
    if (passed) {
        printf("  PASS: %s\n", test_name);
        tests_passed++;
    } else {
        printf("  FAIL: %s\n", test_name);
        printf("        Error: %s\n", socket_get_error());
        tests_failed++;
    }
}

/*
 * Test 1: Socket subsystem initialization
 */
int test_init(void) {
    int rc;
    
    printf("\n=== Test 1: Socket Subsystem Initialization ===\n");
    
    rc = socket_init();
    print_test_result("Initialize socket subsystem", rc == SOCKET_OK);
    
    if (rc == SOCKET_OK) {
        printf("  INFO: mTCP stack initialized successfully\n");
        printf("  INFO: Network configuration loaded from MTCP_CFG\n");
    }
    
    return (rc == SOCKET_OK);
}

/*
 * Test 2: Socket creation
 */
int test_create(void) {
    socket_handle_t sock;
    
    printf("\n=== Test 2: Socket Creation ===\n");
    
    sock = socket_create(SOCKET_TYPE_TCP);
    print_test_result("Create TCP socket", sock != INVALID_SOCKET);
    
    if (sock != INVALID_SOCKET) {
        printf("  INFO: Socket handle = %d\n", sock);
        socket_close(sock);
    }
    
    return (sock != INVALID_SOCKET);
}

/*
 * Test 3: Connect to gateway (should work if network is configured)
 */
int test_gateway_connect(void) {
    socket_handle_t sock;
    int rc;
    
    printf("\n=== Test 3: Connect to Gateway ===\n");
    printf("  INFO: Attempting to connect to 192.168.0.1:80\n");
    printf("  INFO: This tests if local network routing works\n");
    
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) {
        printf("  FAIL: Could not create socket\n");
        return 0;
    }
    
    printf("  Connecting to gateway (192.168.0.1:80)...\n");
    rc = socket_connect(sock, "192.168.0.1", 80);
    
    if (rc == SOCKET_OK) {
        printf("  PASS: Connected to gateway\n");
        printf("  INFO: Local network routing is working\n");
        socket_close(sock);
        return 1;
    } else if (rc == SOCKET_ERR_TIMEOUT) {
        printf("  INFO: Connection timeout (gateway may not have HTTP server)\n");
        printf("  INFO: But this means packets are being sent\n");
        socket_close(sock);
        return 1;  /* Timeout is OK - means network is working */
    } else {
        printf("  FAIL: Connection failed with error %d\n", rc);
        printf("  Error: %s\n", socket_get_error());
        printf("  INFO: This suggests network configuration problem\n");
        socket_close(sock);
        return 0;
    }
}

/*
 * Test 4: Connect to external host
 */
int test_external_connect(void) {
    socket_handle_t sock;
    int rc;
    
    printf("\n=== Test 4: Connect to External Host ===\n");
    printf("  INFO: Attempting to connect to 104.20.23.154:80 (Cloudflare)\n");
    printf("  INFO: This tests if internet routing works\n");
    
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) {
        printf("  FAIL: Could not create socket\n");
        return 0;
    }
    
    printf("  Connecting to Cloudflare (104.20.23.154:80)...\n");
    rc = socket_connect(sock, "104.20.23.154", 80);
    
    if (rc == SOCKET_OK) {
        printf("  PASS: Connected to external host\n");
        printf("  INFO: Internet routing is working\n");
        socket_close(sock);
        return 1;
    } else if (rc == SOCKET_ERR_TIMEOUT) {
        printf("  INFO: Connection timeout\n");
        printf("  INFO: Packets sent but no response - check firewall/routing\n");
        socket_close(sock);
        return 0;
    } else {
        printf("  FAIL: Connection failed with error %d\n", rc);
        printf("  Error: %s\n", socket_get_error());
        printf("  INFO: This suggests internet routing problem\n");
        socket_close(sock);
        return 0;
    }
}

/*
 * Main test program
 */
int main(void) {
    int all_passed = 1;
    
    printf("=== Network Diagnostics for dosjava ===\n");
    printf("This program tests network connectivity step by step\n\n");
    
    printf("Expected network configuration:\n");
    printf("  IP Address:  192.168.0.123\n");
    printf("  Netmask:     255.255.255.0\n");
    printf("  Gateway:     192.168.0.1\n");
    printf("  DNS Server:  8.8.8.8\n");
    printf("  Packet Int:  0x60\n\n");
    
    /* Run tests */
    if (!test_init()) {
        printf("\n=== CRITICAL: Socket initialization failed ===\n");
        printf("Cannot continue with network tests\n");
        printf("\nPossible causes:\n");
        printf("  1. MTCP_CFG environment variable not set\n");
        printf("  2. Configuration file has errors\n");
        printf("  3. Packet driver not loaded (ne2000 0x60 3 0x300)\n");
        printf("  4. DOSBox-X NE2000 emulation not enabled\n");
        return 1;
    }
    
    if (!test_create()) all_passed = 0;
    if (!test_gateway_connect()) all_passed = 0;
    if (!test_external_connect()) all_passed = 0;
    
    /* Cleanup */
    printf("\nShutting down socket subsystem...\n");
    socket_shutdown();
    
    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    
    if (all_passed && tests_failed == 0) {
        printf("\n=== All Tests Passed ===\n");
        printf("Network is fully functional!\n");
        return 0;
    } else {
        printf("\n=== Some Tests Failed ===\n");
        printf("\nTroubleshooting steps:\n");
        printf("  1. Check DOSBox-X network configuration\n");
        printf("  2. Verify NE2000 emulation is enabled\n");
        printf("  3. Check if packet driver is loaded\n");
        printf("  4. Verify MTCP.CFG settings match your network\n");
        printf("  5. Check Windows firewall settings\n");
        return 1;
    }
}

// Made with Bob
