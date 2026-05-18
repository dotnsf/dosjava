/*
 * test_send_recv.c - Test socket send/recv operations
 * 
 * Tests non-blocking I/O with HTTP GET request
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "../../src/network/socket.h"

#ifdef __cplusplus
}
#endif

#define TEST_HOST "104.20.23.154"  /* Cloudflare IP that works */
#define TEST_PORT 80
#define RECV_BUFFER_SIZE 2048

/* Test results */
static int tests_passed = 0;
static int tests_failed = 0;

void print_test_result(const char* test_name, int passed) {
    if (passed) {
        printf("[PASS] %s\n", test_name);
        tests_passed++;
    } else {
        printf("[FAIL] %s\n", test_name);
        tests_failed++;
    }
}

/*
 * Test 1: Send HTTP GET request
 */
int test_send_request(socket_handle_t sock) {
    const char* request = 
        "GET / HTTP/1.0\r\n"
        "Host: 104.20.23.154\r\n"
        "User-Agent: dosjava-test/1.0\r\n"
        "Connection: close\r\n"
        "\r\n";
    
    int len = strlen(request);
    int sent = socket_send(sock, request, len);
    
    if (sent != len) {
        printf("  Error: Expected to send %d bytes, sent %d\n", len, sent);
        printf("  Error message: %s\n", socket_get_error());
        return 0;
    }
    
    printf("  Sent %d bytes\n", sent);
    return 1;
}

/*
 * Test 2: Receive HTTP response
 */
int test_recv_response(socket_handle_t sock) {
    char buffer[RECV_BUFFER_SIZE];
    int total_received = 0;
    int loop_count = 0;
    int max_loops = 1000;  /* Prevent infinite loop */
    int no_data_count = 0;
    int max_no_data = 50;  /* Exit after 50 consecutive empty reads */
    
    printf("  Receiving response...\n");
    
    while (loop_count < max_loops) {
        int received = socket_recv(sock, buffer, RECV_BUFFER_SIZE - 1);
        
        if (received > 0) {
            total_received += received;
            no_data_count = 0;  /* Reset no-data counter */
            
            /* Print first 200 bytes of response */
            if (total_received <= 200) {
                buffer[received] = '\0';
                printf("%s", buffer);
            }
        } else if (received == 0) {
            /* Connection closed or no data */
            if (total_received > 0) {
                printf("\n  Connection closed by remote\n");
                break;
            }
            no_data_count++;
            if (no_data_count >= max_no_data) {
                printf("\n  No more data after %d attempts\n", max_no_data);
                break;
            }
        } else {
            /* Error */
            printf("\n  Receive error: %s\n", socket_get_error());
            return 0;
        }
        
        loop_count++;
    }
    
    if (loop_count >= max_loops) {
        printf("\n  Warning: Reached maximum loop count\n");
    }
    
    printf("  Total received: %d bytes in %d loops\n", total_received, loop_count);
    
    /* Success if we received at least some data */
    return (total_received > 0);
}

/*
 * Test 3: Check socket_can_read()
 */
int test_can_read(socket_handle_t sock) {
    int can_read = socket_can_read(sock);
    
    if (can_read < 0) {
        printf("  Error checking can_read: %s\n", socket_get_error());
        return 0;
    }
    
    printf("  socket_can_read() returned: %d\n", can_read);
    return 1;
}

/*
 * Test 4: Check socket_can_write()
 */
int test_can_write(socket_handle_t sock) {
    int can_write = socket_can_write(sock);
    
    if (can_write < 0) {
        printf("  Error checking can_write: %s\n", socket_get_error());
        return 0;
    }
    
    printf("  socket_can_write() returned: %d\n", can_write);
    return 1;
}

int main(void) {
    socket_handle_t sock;
    int rc;
    
    printf("=== Socket Send/Recv Test ===\n\n");
    
    /* Initialize socket subsystem */
    printf("Test 1: Initialize socket subsystem\n");
    rc = socket_init();
    print_test_result("Socket initialization", rc == SOCKET_OK);
    if (rc != SOCKET_OK) {
        printf("Error: %s\n", socket_get_error());
        return 1;
    }
    
    /* Create socket */
    printf("\nTest 2: Create TCP socket\n");
    sock = socket_create(SOCKET_TYPE_TCP);
    print_test_result("Socket creation", sock != INVALID_SOCKET);
    if (sock == INVALID_SOCKET) {
        printf("Error: %s\n", socket_get_error());
        socket_shutdown();
        return 1;
    }
    
    /* Connect to server */
    printf("\nTest 3: Connect to %s:%d\n", TEST_HOST, TEST_PORT);
    rc = socket_connect(sock, TEST_HOST, TEST_PORT);
    print_test_result("Socket connection", rc == SOCKET_OK);
    if (rc != SOCKET_OK) {
        printf("Error: %s\n", socket_get_error());
        socket_close(sock);
        socket_shutdown();
        return 1;
    }
    
    /* Test can_write before sending */
    printf("\nTest 4: Check socket_can_write() before send\n");
    print_test_result("socket_can_write()", test_can_write(sock));
    
    /* Send HTTP GET request */
    printf("\nTest 5: Send HTTP GET request\n");
    print_test_result("Send request", test_send_request(sock));
    
    /* Test can_read before receiving */
    printf("\nTest 6: Check socket_can_read() before recv\n");
    print_test_result("socket_can_read()", test_can_read(sock));
    
    /* Receive HTTP response */
    printf("\nTest 7: Receive HTTP response\n");
    print_test_result("Receive response", test_recv_response(sock));
    
    /* Close socket */
    printf("\nTest 8: Close socket\n");
    rc = socket_close(sock);
    print_test_result("Socket close", rc == SOCKET_OK);
    
    /* Shutdown */
    printf("\nTest 9: Shutdown socket subsystem\n");
    socket_shutdown();
    print_test_result("Socket shutdown", 1);
    
    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    
    return (tests_failed == 0) ? 0 : 1;
}

