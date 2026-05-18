#include <stdio.h>
#include <string.h>
#include "../../src/runtime/fileoutputstream.h"
#include "../../src/vm/memory.h"

/* Test macros */
#define TEST_START(name) printf("Testing %s... ", name)
#define TEST_PASS() printf("PASS\n")
#define TEST_FAIL(msg) do { printf("FAIL: %s\n", msg); return; } while(0)
#define ASSERT(cond, msg) if (!(cond)) TEST_FAIL(msg)

/* Helper: Read file content */
static int read_file_content(const char* filename, unsigned char* buffer, int max_len) {
    FILE* f = fopen(filename, "rb");
    int len;
    
    if (!f) {
        return -1;
    }
    
    len = fread(buffer, 1, max_len, f);
    fclose(f);
    return len;
}

/* Test 1: FileOutputStream creation and open */
static void test_fileoutputstream_create(void) {
    FileOutputStream* stream;
    const char* test_file = "test1.out";
    
    TEST_START("FileOutputStream creation");
    
    /* Create stream */
    stream = fileoutputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileOutputStream");
    ASSERT(stream->base.is_open == 1, "Stream should be open");
    
    /* Clean up */
    fileoutputstream_delete(stream);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 2: Write single byte */
static void test_fileoutputstream_write_byte(void) {
    FileOutputStream* stream;
    const char* test_file = "test2.out";
    unsigned char buffer[10];
    int len;
    
    TEST_START("FileOutputStream write single byte");
    
    /* Create and write */
    stream = fileoutputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileOutputStream");
    
    fileoutputstream_write(stream, 'A');
    fileoutputstream_write(stream, 'B');
    fileoutputstream_write(stream, 'C');
    
    /* Close to flush */
    fileoutputstream_delete(stream);
    
    /* Verify content */
    len = read_file_content(test_file, buffer, sizeof(buffer));
    ASSERT(len == 3, "Should have written 3 bytes");
    ASSERT(memcmp(buffer, "ABC", 3) == 0, "Content mismatch");
    
    /* Clean up */
    remove(test_file);
    
    TEST_PASS();
}

/* Test 3: Write multiple bytes */
static void test_fileoutputstream_write_bytes(void) {
    FileOutputStream* stream;
    const char* test_file = "test3.out";
    const char* test_data = "Hello, World!";
    unsigned char buffer[20];
    int len;
    
    TEST_START("FileOutputStream write multiple bytes");
    
    /* Create and write */
    stream = fileoutputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileOutputStream");
    
    fileoutputstream_write_bytes(stream, (const unsigned char*)test_data, strlen(test_data));
    
    /* Close to flush */
    fileoutputstream_delete(stream);
    
    /* Verify content */
    len = read_file_content(test_file, buffer, sizeof(buffer));
    ASSERT(len == (int)strlen(test_data), "Length mismatch");
    ASSERT(memcmp(buffer, test_data, strlen(test_data)) == 0, "Content mismatch");
    
    /* Clean up */
    remove(test_file);
    
    TEST_PASS();
}

/* Test 4: Explicit flush */
static void test_fileoutputstream_flush(void) {
    FileOutputStream* stream;
    const char* test_file = "test4.out";
    unsigned char buffer[10];
    int len;
    
    TEST_START("FileOutputStream flush");
    
    /* Create and write */
    stream = fileoutputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileOutputStream");
    
    fileoutputstream_write(stream, 'X');
    fileoutputstream_write(stream, 'Y');
    fileoutputstream_write(stream, 'Z');
    
    /* Flush explicitly */
    fileoutputstream_flush(stream);
    
    /* Close without writing more */
    fileoutputstream_close(stream);
    fileoutputstream_delete(stream);
    
    /* Verify content */
    len = read_file_content(test_file, buffer, sizeof(buffer));
    ASSERT(len == 3, "Should have written 3 bytes");
    ASSERT(memcmp(buffer, "XYZ", 3) == 0, "Content mismatch");
    
    /* Clean up */
    remove(test_file);
    
    TEST_PASS();
}

/* Test 5: Large write (buffer overflow handling) */
static void test_fileoutputstream_large_write(void) {
    FileOutputStream* stream;
    const char* test_file = "test5.out";
    unsigned char write_buffer[1024];
    unsigned char read_buffer[1024];
    int i;
    int len;
    
    TEST_START("FileOutputStream large write");
    
    /* Prepare test data */
    for (i = 0; i < 1024; i++) {
        write_buffer[i] = (unsigned char)(i % 256);
    }
    
    /* Create and write */
    stream = fileoutputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileOutputStream");
    
    fileoutputstream_write_bytes(stream, write_buffer, 1024);
    
    /* Close to flush */
    fileoutputstream_delete(stream);
    
    /* Verify content */
    len = read_file_content(test_file, read_buffer, sizeof(read_buffer));
    ASSERT(len == 1024, "Should have written 1024 bytes");
    ASSERT(memcmp(read_buffer, write_buffer, 1024) == 0, "Content mismatch");
    
    /* Clean up */
    remove(test_file);
    
    TEST_PASS();
}

/* Test 6: Multiple writes */
static void test_fileoutputstream_multiple_writes(void) {
    FileOutputStream* stream;
    const char* test_file = "test6.out";
    unsigned char buffer[50];
    int len;
    
    TEST_START("FileOutputStream multiple writes");
    
    /* Create and write */
    stream = fileoutputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileOutputStream");
    
    fileoutputstream_write_bytes(stream, (const unsigned char*)"Line1\n", 6);
    fileoutputstream_write_bytes(stream, (const unsigned char*)"Line2\n", 6);
    fileoutputstream_write_bytes(stream, (const unsigned char*)"Line3\n", 6);
    
    /* Close to flush */
    fileoutputstream_delete(stream);
    
    /* Verify content */
    len = read_file_content(test_file, buffer, sizeof(buffer));
    ASSERT(len == 18, "Should have written 18 bytes");
    ASSERT(memcmp(buffer, "Line1\nLine2\nLine3\n", 18) == 0, "Content mismatch");
    
    /* Clean up */
    remove(test_file);
    
    TEST_PASS();
}

/* Test 7: Close and reopen */
static void test_fileoutputstream_reopen(void) {
    FileOutputStream* stream;
    const char* test_file = "test7.out";
    unsigned char buffer[20];
    int len;
    
    TEST_START("FileOutputStream close and reopen");
    
    /* First write */
    stream = fileoutputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileOutputStream");
    fileoutputstream_write_bytes(stream, (const unsigned char*)"First", 5);
    fileoutputstream_delete(stream);
    
    /* Reopen (should truncate) */
    stream = fileoutputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to reopen FileOutputStream");
    fileoutputstream_write_bytes(stream, (const unsigned char*)"Second", 6);
    fileoutputstream_delete(stream);
    
    /* Verify content (should only have "Second") */
    len = read_file_content(test_file, buffer, sizeof(buffer));
    ASSERT(len == 6, "Should have written 6 bytes");
    ASSERT(memcmp(buffer, "Second", 6) == 0, "Content mismatch");
    
    /* Clean up */
    remove(test_file);
    
    TEST_PASS();
}

/* Test 8: Mixed byte and bytes writes */
static void test_fileoutputstream_mixed_writes(void) {
    FileOutputStream* stream;
    const char* test_file = "test8.out";
    unsigned char buffer[20];
    int len;
    
    TEST_START("FileOutputStream mixed writes");
    
    /* Create and write */
    stream = fileoutputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileOutputStream");
    
    fileoutputstream_write(stream, 'A');
    fileoutputstream_write_bytes(stream, (const unsigned char*)"BC", 2);
    fileoutputstream_write(stream, 'D');
    fileoutputstream_write_bytes(stream, (const unsigned char*)"EF", 2);
    
    /* Close to flush */
    fileoutputstream_delete(stream);
    
    /* Verify content */
    len = read_file_content(test_file, buffer, sizeof(buffer));
    ASSERT(len == 6, "Should have written 6 bytes");
    ASSERT(memcmp(buffer, "ABCDEF", 6) == 0, "Content mismatch");
    
    /* Clean up */
    remove(test_file);
    
    TEST_PASS();
}

/* Main test runner */
int main(void) {
    int passed = 0;
    int failed = 0;
    
    printf("=== FileOutputStream Test ===\n\n");
    
    /* Initialize memory manager */
    printf("Initializing memory manager...\n");
    if (memory_init(0) != 0) {
        printf("Failed to initialize memory manager\n");
        return 1;
    }
    printf("Memory manager initialized successfully\n");
    printf("Available memory: %u bytes\n\n", memory_available());
    
    printf("--- FileOutputStream Tests ---\n");
    
    /* Run tests */
    test_fileoutputstream_create();
    test_fileoutputstream_write_byte();
    test_fileoutputstream_write_bytes();
    test_fileoutputstream_flush();
    test_fileoutputstream_large_write();
    test_fileoutputstream_multiple_writes();
    test_fileoutputstream_reopen();
    test_fileoutputstream_mixed_writes();
    
    /* Count results */
    passed = 8;  /* Update if tests fail */
    failed = 0;
    
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    
    /* Shutdown memory manager */
    printf("\nShutting down memory manager...\n");
    memory_shutdown();
    
    if (failed == 0) {
        printf("\nAll tests PASSED!\n");
        return 0;
    } else {
        printf("\nSome tests FAILED!\n");
        return 1;
    }
}
