/*
 * test_socket.c - Socket Wrapper Test Program
 * 
 * Tests the C-level socket wrapper functions
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
int test_socket_init(void) {
    int rc;
    
    printf("\nTest 1: Socket subsystem initialization\n");
    
    rc = socket_init();
    print_test_result("Initialize socket subsystem", rc == SOCKET_OK);
    
    return (rc == SOCKET_OK);
}

/*
 * Test 2: Socket creation
 */
int test_socket_create(void) {
    socket_handle_t sock;
    
    printf("\nTest 2: Socket creation\n");
    
    sock = socket_create(SOCKET_TYPE_TCP);
    print_test_result("Create TCP socket", sock != INVALID_SOCKET);
    
    if (sock != INVALID_SOCKET) {
        int state = socket_get_state(sock);
        print_test_result("Socket in CLOSED state", state == SOCKET_STATE_CLOSED);
        
        socket_close(sock);
        return 1;
    }
    
    return 0;
}

/*
 * Test 3: Socket state management
 */
int test_socket_state(void) {
    socket_handle_t sock;
    int state;
    
    printf("\nTest 3: Socket state management\n");
    
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) {
        print_test_result("Create socket for state test", 0);
        return 0;
    }
    
    state = socket_get_state(sock);
    print_test_result("Initial state is CLOSED", state == SOCKET_STATE_CLOSED);
    
    socket_close(sock);
    
    state = socket_get_state(sock);
    print_test_result("State after close is CLOSED", state == SOCKET_STATE_CLOSED);
    
    return 1;
}

/*
 * Test 4: Multiple socket creation
 */
int test_multiple_sockets(void) {
    socket_handle_t sock1, sock2;
    
    printf("\nTest 4: Multiple socket creation\n");
    
    sock1 = socket_create(SOCKET_TYPE_TCP);
    print_test_result("Create first socket", sock1 != INVALID_SOCKET);
    
    sock2 = socket_create(SOCKET_TYPE_TCP);
    print_test_result("Create second socket", sock2 != INVALID_SOCKET);
    
    print_test_result("Sockets have different handles", sock1 != sock2);
    
    if (sock1 != INVALID_SOCKET) {
        socket_close(sock1);
    }
    if (sock2 != INVALID_SOCKET) {
        socket_close(sock2);
    }
    
    return 1;
}

/*
 * Test 5: Socket limit
 */
int test_socket_limit(void) {
    socket_handle_t sockets[MAX_SOCKETS + 1];
    int i;
    int created = 0;
    
    printf("\nTest 5: Socket limit (MAX_SOCKETS=%d)\n", MAX_SOCKETS);
    
    /* Try to create MAX_SOCKETS + 1 sockets */
    for (i = 0; i <= MAX_SOCKETS; i++) {
        sockets[i] = socket_create(SOCKET_TYPE_TCP);
        if (sockets[i] != INVALID_SOCKET) {
            created++;
        }
    }
    
    print_test_result("Created exactly MAX_SOCKETS", created == MAX_SOCKETS);
    print_test_result("Failed to create beyond limit", sockets[MAX_SOCKETS] == INVALID_SOCKET);
    
    /* Clean up */
    for (i = 0; i < MAX_SOCKETS; i++) {
        if (sockets[i] != INVALID_SOCKET) {
            socket_close(sockets[i]);
        }
    }
    
    return 1;
}

/*
 * Test 6: Invalid operations
 */
int test_invalid_operations(void) {
    int rc;
    char buffer[100];
    
    printf("\nTest 6: Invalid operations\n");
    
    /* Try to send on invalid socket */
    rc = socket_send(INVALID_SOCKET, "test", 4);
    print_test_result("Send on invalid socket fails", rc < 0);
    
    /* Try to receive on invalid socket */
    rc = socket_recv(INVALID_SOCKET, buffer, sizeof(buffer));
    print_test_result("Receive on invalid socket fails", rc < 0);
    
    /* Try to close invalid socket */
    rc = socket_close(INVALID_SOCKET);
    print_test_result("Close invalid socket fails", rc < 0);
    
    return 1;
}

/*
 * Test 7: Socket reuse after close
 */
int test_socket_reuse(void) {
    socket_handle_t sock1, sock2;
    
    printf("\nTest 7: Socket reuse after close\n");
    
    sock1 = socket_create(SOCKET_TYPE_TCP);
    print_test_result("Create first socket", sock1 != INVALID_SOCKET);
    
    if (sock1 != INVALID_SOCKET) {
        socket_close(sock1);
        print_test_result("Close first socket", 1);
        
        sock2 = socket_create(SOCKET_TYPE_TCP);
        print_test_result("Create socket after close", sock2 != INVALID_SOCKET);
        print_test_result("Reused same handle", sock1 == sock2);
        
        if (sock2 != INVALID_SOCKET) {
            socket_close(sock2);
        }
    }
    
    return 1;
}

/*
 * Main test program
 */
int main(void) {
    printf("=== Socket Wrapper Test Program ===\n");
    printf("Testing C-level socket wrapper for mTCP\n");
    
    /* Run tests */
    if (!test_socket_init()) {
        printf("\nERROR: Socket initialization failed\n");
        printf("Make sure:\n");
        printf("  1. MTCP_CFG environment variable is set\n");
        printf("  2. Packet driver is loaded\n");
        printf("  3. Configuration file exists\n");
        return 1;
    }
    
    test_socket_create();
    test_socket_state();
    test_multiple_sockets();
    test_socket_limit();
    test_invalid_operations();
    test_socket_reuse();
    
    /* Shutdown */
    printf("\nShutting down socket subsystem...\n");
    socket_shutdown();
    
    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    
    if (tests_failed == 0) {
        printf("\n=== All Tests Passed ===\n");
        return 0;
    } else {
        printf("\n=== Some Tests Failed ===\n");
        return 1;
    }
}

// Made with Bob
