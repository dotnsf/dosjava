/*
 * test_socket_runtime.c - Test program for runtime Socket class
 * 
 * Tests the C implementation of java.net.Socket
 */

#include "../../src/runtime/socket.h"
#include "../../src/runtime/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counter */
static int g_test_count = 0;
static int g_test_passed = 0;

/* Test result macro */
#define TEST(name, condition) \
    do { \
        g_test_count++; \
        printf("Test %d: %s - ", g_test_count, name); \
        if (condition) { \
            printf("PASSED\n"); \
            g_test_passed++; \
        } else { \
            printf("FAILED\n"); \
        } \
    } while(0)

/**
 * Test 1: Socket subsystem initialization
 */
void test_subsystem_init(void) {
    int rc;
    
    printf("\n=== Test 1: Socket Subsystem Initialization ===\n");
    
    /* Test initialization */
    rc = socket_subsystem_init();
    TEST("socket_subsystem_init()", rc == 0);
    TEST("socket_subsystem_is_initialized()", socket_subsystem_is_initialized() == 1);
    
    /* Test double initialization (should succeed) */
    rc = socket_subsystem_init();
    TEST("socket_subsystem_init() again", rc == 0);
}

/**
 * Test 2: Socket creation and deletion
 */
void test_socket_creation(void) {
    Socket* sock;
    
    printf("\n=== Test 2: Socket Creation and Deletion ===\n");
    
    /* Create socket */
    sock = socket_runtime_new();
    TEST("socket_runtime_new()", sock != NULL);
    
    if (sock != NULL) {
        TEST("socket is not connected", !socket_runtime_is_connected(sock));
        TEST("socket is not closed", !socket_runtime_is_closed(sock));
        TEST("socket handle is -1", socket_runtime_get_handle(sock) == -1);
        TEST("socket host is NULL", socket_runtime_get_host(sock) == NULL);
        TEST("socket port is 0", socket_runtime_get_port(sock) == 0);
        
        /* Delete socket */
        socket_runtime_delete(sock);
        TEST("socket_runtime_delete()", 1);  /* No crash = pass */
    }
}

/**
 * Test 3: Socket connection
 */
void test_socket_connection(void) {
    Socket* sock;
    String* host;
    int rc;
    
    printf("\n=== Test 3: Socket Connection ===\n");
    
    /* Create socket */
    sock = socket_runtime_new();
    if (sock == NULL) {
        printf("ERROR: Failed to create socket\n");
        return;
    }
    
    /* Create host string */
    host = string_new("www.cloudflare.com");
    if (host == NULL) {
        printf("ERROR: Failed to create host string\n");
        socket_runtime_delete(sock);
        return;
    }
    
    /* Connect to host */
    printf("Connecting to www.cloudflare.com:80...\n");
    rc = socket_runtime_connect(sock, host, 80);
    TEST("socket_runtime_connect()", rc == 0);
    
    if (rc == 0) {
        String* sock_host;
        TEST("socket is connected", socket_runtime_is_connected(sock));
        TEST("socket is not closed", !socket_runtime_is_closed(sock));
        TEST("socket handle >= 0", socket_runtime_get_handle(sock) >= 0);
        
        sock_host = socket_runtime_get_host(sock);
        TEST("socket host is set", sock_host != NULL);
        if (sock_host != NULL) {
            TEST("socket host matches",
                 strcmp(string_tocstr(sock_host), "www.cloudflare.com") == 0);
        }
        
        TEST("socket port is 80", socket_runtime_get_port(sock) == 80);
    }
    
    /* Cleanup */
    string_delete(host);
    socket_runtime_delete(sock);
}

/**
 * Test 4: Socket send/receive
 */
void test_socket_send_recv(void) {
    Socket* sock;
    String* host;
    const char* request = "GET / HTTP/1.0\r\nHost: www.cloudflare.com\r\n\r\n";
    uint8_t buffer[1024];
    int sent, received;
    int i;
    
    printf("\n=== Test 4: Socket Send/Receive ===\n");
    
    /* Create and connect socket */
    host = string_new("www.cloudflare.com");
    sock = socket_runtime_new_connected(host, 80);
    TEST("socket_runtime_new_connected()", sock != NULL);
    
    if (sock == NULL) {
        string_delete(host);
        return;
    }
    
    /* Send HTTP request */
    printf("Sending HTTP GET request...\n");
    sent = socket_runtime_send(sock, (const uint8_t*)request, strlen(request));
    TEST("socket_runtime_send()", sent > 0);
    printf("  Sent %d bytes\n", sent);
    
    /* Receive HTTP response */
    printf("Receiving HTTP response...\n");
    received = socket_runtime_recv(sock, buffer, sizeof(buffer) - 1);
    TEST("socket_runtime_recv()", received > 0);
    
    if (received > 0) {
        buffer[received] = '\0';
        printf("  Received %d bytes\n", received);
        printf("  First line: ");
        
        /* Print first line */
        for (i = 0; i < received && buffer[i] != '\r' && buffer[i] != '\n'; i++) {
            putchar(buffer[i]);
        }
        printf("\n");
        
        /* Check for HTTP response */
        TEST("response starts with HTTP", 
             received >= 4 && buffer[0] == 'H' && buffer[1] == 'T' && 
             buffer[2] == 'T' && buffer[3] == 'P');
    }
    
    /* Cleanup */
    string_delete(host);
    socket_runtime_delete(sock);
}

/**
 * Test 5: Socket close
 */
void test_socket_close(void) {
    Socket* sock;
    String* host;
    int rc;
    
    printf("\n=== Test 5: Socket Close ===\n");
    
    /* Create and connect socket */
    host = string_new("www.cloudflare.com");
    sock = socket_runtime_new_connected(host, 80);
    
    if (sock == NULL) {
        printf("ERROR: Failed to create connected socket\n");
        string_delete(host);
        return;
    }
    
    TEST("socket is connected before close", socket_runtime_is_connected(sock));
    TEST("socket is not closed before close", !socket_runtime_is_closed(sock));
    
    /* Close socket */
    rc = socket_runtime_close(sock);
    TEST("socket_runtime_close()", rc == 0);
    TEST("socket is not connected after close", !socket_runtime_is_connected(sock));
    TEST("socket is closed after close", socket_runtime_is_closed(sock));
    
    /* Try to close again (should succeed) */
    rc = socket_runtime_close(sock);
    TEST("socket_runtime_close() again", rc == 0);
    
    /* Cleanup */
    string_delete(host);
    socket_runtime_delete(sock);
}

/**
 * Test 6: SocketException
 */
void test_socket_exception(void) {
    SocketException* ex;
    String* msg;
    
    printf("\n=== Test 6: SocketException ===\n");
    
    /* Create exception */
    ex = socket_exception_new("Test error", -1);
    TEST("socket_exception_new()", ex != NULL);
    
    if (ex != NULL) {
        msg = socket_exception_get_message(ex);
        TEST("exception message is set", msg != NULL);
        
        if (msg != NULL) {
            TEST("exception message matches", 
                 strcmp(string_tocstr(msg), "Test error") == 0);
        }
        
        TEST("exception error code is -1", socket_exception_get_error_code(ex) == -1);
        
        /* Delete exception */
        socket_exception_delete(ex);
        TEST("socket_exception_delete()", 1);  /* No crash = pass */
    }
}

/**
 * Test 7: Socket subsystem shutdown
 */
void test_subsystem_shutdown(void) {
    printf("\n=== Test 7: Socket Subsystem Shutdown ===\n");
    
    /* Shutdown subsystem */
    socket_subsystem_shutdown();
    TEST("socket_subsystem_shutdown()", 1);  /* No crash = pass */
    TEST("socket_subsystem_is_initialized() after shutdown", 
         !socket_subsystem_is_initialized());
    
    /* Shutdown again (should be safe) */
    socket_subsystem_shutdown();
    TEST("socket_subsystem_shutdown() again", 1);  /* No crash = pass */
}

/**
 * Main function
 */
int main(void) {
    printf("=================================================\n");
    printf("Runtime Socket Class Test Program\n");
    printf("=================================================\n");
    
    /* Run tests */
    test_subsystem_init();
    test_socket_creation();
    test_socket_connection();
    test_socket_send_recv();
    test_socket_close();
    test_socket_exception();
    test_subsystem_shutdown();
    
    /* Print summary */
    printf("\n=================================================\n");
    printf("Test Summary\n");
    printf("=================================================\n");
    printf("Total tests:  %d\n", g_test_count);
    printf("Passed:       %d\n", g_test_passed);
    printf("Failed:       %d\n", g_test_count - g_test_passed);
    printf("Success rate: %.1f%%\n", 
           (g_test_count > 0) ? (g_test_passed * 100.0 / g_test_count) : 0.0);
    printf("=================================================\n");
    
    return (g_test_passed == g_test_count) ? 0 : 1;
}

// Made with Bob
