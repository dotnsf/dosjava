/*
 * test_http.c - HTTP Client Test Program
 * 
 * Tests socket wrapper with real network communication
 * Sends HTTP GET request and receives response
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/network/socket.h"

/* Test configuration */
#define TEST_HOST "93.184.216.34"  /* example.com IP address */
#define TEST_PORT 80
#define RECV_BUFFER_SIZE 1024

/* HTTP request template */
const char* http_request = 
    "GET / HTTP/1.0\r\n"
    "Host: example.com\r\n"
    "User-Agent: dosjava-test/1.0\r\n"
    "Connection: close\r\n"
    "\r\n";

/*
 * Test 1: Basic HTTP GET request
 */
int test_http_get(void) {
    socket_handle_t sock;
    int rc;
    char recv_buffer[RECV_BUFFER_SIZE];
    int total_received = 0;
    int bytes_received;
    
    printf("\n=== Test 1: HTTP GET Request ===\n");
    
    /* Create socket */
    printf("Creating TCP socket...\n");
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) {
        printf("FAIL: Could not create socket\n");
        printf("Error: %s\n", socket_get_error());
        return 0;
    }
    printf("PASS: Socket created (handle=%d)\n", sock);
    
    /* Connect to server */
    printf("Connecting to %s:%d...\n", TEST_HOST, TEST_PORT);
    printf("[DEBUG] Attempting connection...\n");
    rc = socket_connect(sock, TEST_HOST, TEST_PORT);
    printf("[DEBUG] socket_connect returned: %d\n", rc);
    if (rc != SOCKET_OK) {
        printf("FAIL: Could not connect to server\n");
        printf("Error: %s\n", socket_get_error());
        printf("[DEBUG] Connection failed with code %d\n", rc);
        socket_close(sock);
        return 0;
    }
    printf("PASS: Connected to server\n");
    
    /* Send HTTP request */
    printf("Sending HTTP request...\n");
    rc = socket_send(sock, http_request, strlen(http_request));
    if (rc < 0) {
        printf("FAIL: Could not send request\n");
        printf("Error: %s\n", socket_get_error());
        socket_close(sock);
        return 0;
    }
    printf("PASS: Sent %d bytes\n", rc);
    
    /* Receive response */
    printf("Receiving HTTP response...\n");
    while (1) {
        bytes_received = socket_recv(sock, recv_buffer, RECV_BUFFER_SIZE - 1);
        
        if (bytes_received < 0) {
            printf("FAIL: Error receiving data\n");
            printf("Error: %s\n", socket_get_error());
            socket_close(sock);
            return 0;
        }
        
        if (bytes_received == 0) {
            /* Connection closed by server */
            printf("Connection closed by server\n");
            break;
        }
        
        total_received += bytes_received;
        recv_buffer[bytes_received] = '\0';
        
        /* Print first chunk only to avoid flooding output */
        if (total_received == bytes_received) {
            printf("First chunk received:\n");
            printf("---\n%s\n---\n", recv_buffer);
        }
    }
    
    printf("PASS: Received total %d bytes\n", total_received);
    
    /* Close socket */
    printf("Closing socket...\n");
    rc = socket_close(sock);
    if (rc != SOCKET_OK) {
        printf("FAIL: Could not close socket\n");
        printf("Error: %s\n", socket_get_error());
        return 0;
    }
    printf("PASS: Socket closed\n");
    
    return 1;
}

/*
 * Test 2: Multiple sequential connections
 */
int test_multiple_connections(void) {
    socket_handle_t sock;
    int rc;
    int i;
    char recv_buffer[256];
    
    printf("\n=== Test 2: Multiple Sequential Connections ===\n");
    
    for (i = 0; i < 3; i++) {
        printf("\nConnection %d:\n", i + 1);
        
        /* Create socket */
        sock = socket_create(SOCKET_TYPE_TCP);
        if (sock == INVALID_SOCKET) {
            printf("FAIL: Could not create socket\n");
            return 0;
        }
        
        /* Connect */
        rc = socket_connect(sock, TEST_HOST, TEST_PORT);
        if (rc != SOCKET_OK) {
            printf("FAIL: Could not connect\n");
            socket_close(sock);
            return 0;
        }
        printf("  Connected\n");
        
        /* Send request */
        rc = socket_send(sock, http_request, strlen(http_request));
        if (rc < 0) {
            printf("FAIL: Could not send\n");
            socket_close(sock);
            return 0;
        }
        printf("  Sent %d bytes\n", rc);
        
        /* Receive some data */
        rc = socket_recv(sock, recv_buffer, sizeof(recv_buffer) - 1);
        if (rc > 0) {
            printf("  Received %d bytes\n", rc);
        }
        
        /* Close */
        socket_close(sock);
        printf("  Closed\n");
    }
    
    printf("\nPASS: All connections successful\n");
    return 1;
}

/*
 * Test 3: Connection timeout test
 */
int test_connection_timeout(void) {
    socket_handle_t sock;
    int rc;
    
    printf("\n=== Test 3: Connection Timeout Test ===\n");
    printf("Attempting to connect to non-existent host...\n");
    
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) {
        printf("FAIL: Could not create socket\n");
        return 0;
    }
    
    /* Try to connect to non-routable IP (should timeout) */
    rc = socket_connect(sock, "192.0.2.1", 80);
    if (rc == SOCKET_OK) {
        printf("UNEXPECTED: Connection succeeded to non-existent host\n");
        socket_close(sock);
        return 0;
    }
    
    printf("PASS: Connection failed as expected\n");
    printf("Error: %s\n", socket_get_error());
    
    socket_close(sock);
    return 1;
}

/*
 * Test 4: Send/Receive with small chunks
 */
int test_small_chunks(void) {
    socket_handle_t sock;
    int rc;
    char recv_buffer[64];
    int total_received = 0;
    int chunks = 0;
    const char* req;
    int req_len;
    int sent;
    int chunk_size;
    
    printf("\n=== Test 4: Small Chunk Send/Receive ===\n");
    
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) {
        printf("FAIL: Could not create socket\n");
        return 0;
    }
    
    rc = socket_connect(sock, TEST_HOST, TEST_PORT);
    if (rc != SOCKET_OK) {
        printf("FAIL: Could not connect\n");
        socket_close(sock);
        return 0;
    }
    
    /* Send request in small chunks */
    req = http_request;
    req_len = strlen(req);
    sent = 0;
    
    printf("Sending request in small chunks...\n");
    while (sent < req_len) {
        chunk_size = (req_len - sent > 16) ? 16 : (req_len - sent);
        rc = socket_send(sock, req + sent, chunk_size);
        if (rc < 0) {
            printf("FAIL: Send error\n");
            socket_close(sock);
            return 0;
        }
        if (rc == 0) {
            printf("FAIL: Send returned 0 (no data sent)\n");
            socket_close(sock);
            return 0;
        }
        sent += rc;
        printf("  Sent chunk: %d bytes (total: %d/%d)\n", rc, sent, req_len);
    }
    
    /* Receive in small chunks */
    printf("Receiving response in small chunks...\n");
    while (chunks < 10) {
        rc = socket_recv(sock, recv_buffer, sizeof(recv_buffer) - 1);
        if (rc < 0) {
            printf("FAIL: Receive error\n");
            socket_close(sock);
            return 0;
        }
        if (rc == 0) {
            break;
        }
        total_received += rc;
        chunks++;
        printf("  Received chunk %d: %d bytes\n", chunks, rc);
    }
    
    printf("PASS: Received %d bytes in %d chunks\n", total_received, chunks);
    
    socket_close(sock);
    return 1;
}

/*
 * Main test program
 */
int main(void) {
    int tests_passed = 0;
    int tests_failed = 0;
    
    printf("=== Socket Network Communication Test ===\n");
    printf("Testing real network operations with HTTP\n");
    printf("\n");
    
    /* Initialize socket subsystem */
    printf("Initializing socket subsystem...\n");
    if (socket_init() != SOCKET_OK) {
        printf("ERROR: Socket initialization failed\n");
        printf("Make sure:\n");
        printf("  1. MTCP_CFG environment variable is set\n");
        printf("  2. Packet driver is loaded\n");
        printf("  3. Network is configured\n");
        return 1;
    }
    printf("Socket subsystem initialized\n");
    
    /* Run tests */
    printf("\n");
    if (test_http_get()) {
        tests_passed++;
        printf("\n[DEBUG] Test 1 passed, continuing to Test 2\n");
    } else {
        tests_failed++;
        printf("\n[DEBUG] Test 1 failed, skipping remaining tests\n");
        goto cleanup;
    }
    
    if (test_multiple_connections()) {
        tests_passed++;
        printf("\n[DEBUG] Test 2 passed, continuing to Test 3\n");
    } else {
        tests_failed++;
        printf("\n[DEBUG] Test 2 failed, skipping remaining tests\n");
        goto cleanup;
    }
    
    if (test_connection_timeout()) {
        tests_passed++;
        printf("\n[DEBUG] Test 3 passed, continuing to Test 4\n");
    } else {
        tests_failed++;
        printf("\n[DEBUG] Test 3 failed, skipping remaining tests\n");
        goto cleanup;
    }
    
    if (test_small_chunks()) {
        tests_passed++;
        printf("\n[DEBUG] Test 4 passed\n");
    } else {
        tests_failed++;
        printf("\n[DEBUG] Test 4 failed\n");
    }

cleanup:
    
    /* Shutdown */
    printf("\n");
    printf("Shutting down socket subsystem...\n");
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
