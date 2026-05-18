#include "../../src/runtime/fileinputstream.h"
#include "../../src/vm/memory.h"
#include <stdio.h>
#include <string.h>

/* Test result counters */
static int tests_passed = 0;
static int tests_failed = 0;

/* Test helper macros */
#define TEST_START(name) printf("Testing %s... ", name)
#define TEST_PASS() do { printf("PASS\n"); tests_passed++; } while(0)
#define TEST_FAIL(msg) do { printf("FAIL: %s\n", msg); tests_failed++; } while(0)
#define ASSERT(cond, msg) if (!(cond)) { TEST_FAIL(msg); return; }

/* Create a test file for reading */
static int create_test_file(const char* filename, const char* content) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        return -1;
    }
    fputs(content, f);
    fclose(f);
    return 0;
}

/* Test 1: FileInputStream creation and open */
static void test_fileinputstream_open(void) {
    FileInputStream* stream;
    const char* test_file = "test1.txt";
    const char* test_content = "Hello, World!";
    
    TEST_START("FileInputStream open");
    
    /* Create test file */
    if (create_test_file(test_file, test_content) != 0) {
        TEST_FAIL("Could not create test file");
        return;
    }
    
    /* Open file */
    stream = fileinputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileInputStream");
    ASSERT(stream->base.is_open == 1, "Stream not marked as open");
    ASSERT(stream->handle >= 0, "Invalid file handle");
    
    /* Clean up */
    fileinputstream_delete(stream);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 2: Read single byte */
static void test_fileinputstream_read_byte(void) {
    FileInputStream* stream;
    const char* test_file = "test2.txt";
    const char* test_content = "ABC";
    int byte;
    
    TEST_START("FileInputStream read single byte");
    
    /* Create test file */
    if (create_test_file(test_file, test_content) != 0) {
        TEST_FAIL("Could not create test file");
        return;
    }
    
    /* Open and read */
    stream = fileinputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileInputStream");
    
    byte = fileinputstream_read(stream);
    ASSERT(byte == 'A', "First byte incorrect");
    
    byte = fileinputstream_read(stream);
    ASSERT(byte == 'B', "Second byte incorrect");
    
    byte = fileinputstream_read(stream);
    ASSERT(byte == 'C', "Third byte incorrect");
    
    byte = fileinputstream_read(stream);
    ASSERT(byte == -1, "Should return -1 at EOF");
    
    /* Clean up */
    fileinputstream_delete(stream);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 3: Read multiple bytes */
static void test_fileinputstream_read_bytes(void) {
    FileInputStream* stream;
    const char* test_file = "test3.txt";
    const char* test_content = "Hello, World!";
    unsigned char buffer[20];
    int bytes_read;
    
    TEST_START("FileInputStream read multiple bytes");
    
    /* Create test file */
    if (create_test_file(test_file, test_content) != 0) {
        TEST_FAIL("Could not create test file");
        return;
    }
    
    /* Open and read */
    stream = fileinputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileInputStream");
    
    memset(buffer, 0, sizeof(buffer));
    bytes_read = fileinputstream_read_bytes(stream, buffer, 5);
    ASSERT(bytes_read == 5, "Should read 5 bytes");
    ASSERT(memcmp(buffer, "Hello", 5) == 0, "Content mismatch");
    
    memset(buffer, 0, sizeof(buffer));
    bytes_read = fileinputstream_read_bytes(stream, buffer, 8);
    ASSERT(bytes_read == 8, "Should read 8 bytes");
    ASSERT(memcmp(buffer, ", World!", 8) == 0, "Content mismatch");
    
    /* Clean up */
    fileinputstream_delete(stream);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 4: Available bytes */
static void test_fileinputstream_available(void) {
    FileInputStream* stream;
    const char* test_file = "test4.txt";
    const char* test_content = "1234567890";
    int available;
    
    TEST_START("FileInputStream available");
    
    /* Create test file */
    if (create_test_file(test_file, test_content) != 0) {
        TEST_FAIL("Could not create test file");
        return;
    }
    
    /* Open and check available */
    stream = fileinputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileInputStream");
    
    available = fileinputstream_available(stream);
    ASSERT(available == 10, "Should have 10 bytes available");
    
    /* Read 3 bytes */
    fileinputstream_read(stream);
    fileinputstream_read(stream);
    fileinputstream_read(stream);
    
    available = fileinputstream_available(stream);
    ASSERT(available == 7, "Should have 7 bytes available");
    
    /* Clean up */
    fileinputstream_delete(stream);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 5: Skip bytes */
static void test_fileinputstream_skip(void) {
    FileInputStream* stream;
    const char* test_file = "test5.txt";
    const char* test_content = "ABCDEFGHIJ";
    long skipped;
    int byte;
    
    TEST_START("FileInputStream skip");
    
    /* Create test file */
    if (create_test_file(test_file, test_content) != 0) {
        TEST_FAIL("Could not create test file");
        return;
    }
    
    /* Open and skip */
    stream = fileinputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileInputStream");
    
    /* Skip 3 bytes */
    skipped = fileinputstream_skip(stream, 3);
    ASSERT(skipped == 3, "Should skip 3 bytes");
    
    /* Read next byte (should be 'D') */
    byte = fileinputstream_read(stream);
    ASSERT(byte == 'D', "Should read 'D' after skip");
    
    /* Clean up */
    fileinputstream_delete(stream);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 6: EOF detection */
static void test_fileinputstream_eof(void) {
    FileInputStream* stream;
    const char* test_file = "test6.txt";
    const char* test_content = "ABC";
    unsigned char buffer[10];
    
    TEST_START("FileInputStream EOF detection");
    
    /* Create test file */
    if (create_test_file(test_file, test_content) != 0) {
        TEST_FAIL("Could not create test file");
        return;
    }
    
    /* Open and read all */
    stream = fileinputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileInputStream");
    
    ASSERT(fileinputstream_is_eof(stream) == 0, "Should not be at EOF initially");
    
    /* Read all bytes */
    fileinputstream_read_bytes(stream, buffer, 3);
    
    ASSERT(fileinputstream_is_eof(stream) == 1, "Should be at EOF after reading all");
    
    /* Clean up */
    fileinputstream_delete(stream);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 7: File size */
static void test_fileinputstream_file_size(void) {
    FileInputStream* stream;
    const char* test_file = "test7.txt";
    const char* test_content = "1234567890ABCDEF";
    long size;
    
    TEST_START("FileInputStream file size");
    
    /* Create test file */
    if (create_test_file(test_file, test_content) != 0) {
        TEST_FAIL("Could not create test file");
        return;
    }
    
    /* Open and check size */
    stream = fileinputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileInputStream");
    
    size = fileinputstream_get_file_size(stream);
    ASSERT(size == 16, "File size should be 16 bytes");
    
    /* Clean up */
    fileinputstream_delete(stream);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 8: Close and reopen */
static void test_fileinputstream_close_reopen(void) {
    FileInputStream* stream;
    const char* test_file = "test8.txt";
    const char* test_content = "Test";
    int byte;
    
    TEST_START("FileInputStream close and reopen");
    
    /* Create test file */
    if (create_test_file(test_file, test_content) != 0) {
        TEST_FAIL("Could not create test file");
        return;
    }
    
    /* Open, close, reopen */
    stream = fileinputstream_new(test_file);
    ASSERT(stream != NULL, "Failed to create FileInputStream");
    
    fileinputstream_close(stream);
    ASSERT(stream->base.is_open == 0, "Stream should be closed");
    
    ASSERT(fileinputstream_open(stream, test_file) == 0, "Failed to reopen");
    ASSERT(stream->base.is_open == 1, "Stream should be open");
    
    byte = fileinputstream_read(stream);
    ASSERT(byte == 'T', "Should read first byte after reopen");
    
    /* Clean up */
    fileinputstream_delete(stream);
    remove(test_file);
    
    TEST_PASS();
}

/* Main test runner */
int main(void) {
    int result;
    
    printf("=== FileInputStream Test ===\n\n");
    
    /* Initialize memory manager */
    printf("Initializing memory manager...\n");
    if (memory_init(0) != 0) {
        printf("Error: Failed to initialize memory manager\n");
        return 1;
    }
    printf("Memory manager initialized successfully\n");
    printf("Available memory: %u bytes\n\n", memory_available());
    
    printf("--- FileInputStream Tests ---\n");
    test_fileinputstream_open();
    test_fileinputstream_read_byte();
    test_fileinputstream_read_bytes();
    test_fileinputstream_available();
    test_fileinputstream_skip();
    test_fileinputstream_eof();
    test_fileinputstream_file_size();
    test_fileinputstream_close_reopen();
    
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    
    /* Shutdown memory manager */
    printf("\nShutting down memory manager...\n");
    memory_shutdown();
    
    if (tests_failed == 0) {
        printf("\nAll tests PASSED!\n");
        result = 0;
    } else {
        printf("\nSome tests FAILED!\n");
        result = 1;
    }
    
    return result;
}
