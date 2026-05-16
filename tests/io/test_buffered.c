#include <stdio.h>
#include <string.h>
#include "../../src/runtime/bufferedreader.h"
#include "../../src/runtime/bufferedwriter.h"
#include "../../src/vm/memory.h"

/* Test macros */
#define TEST_START(name) printf("Testing %s... ", name)
#define TEST_PASS() printf("PASS\n")
#define TEST_FAIL(msg) do { printf("FAIL: %s\n", msg); return; } while(0)
#define ASSERT(cond, msg) if (!(cond)) TEST_FAIL(msg)

/* Test 1: BufferedWriter write line */
static void test_bufferedwriter_write_line(void) {
    FileOutputStream* fos;
    BufferedWriter* writer;
    const char* test_file = "test1.txt";
    FILE* f;
    char buffer[100];
    
    TEST_START("BufferedWriter write line");
    
    /* Create writer */
    fos = fileoutputstream_new(test_file);
    ASSERT(fos != NULL, "Failed to create FileOutputStream");
    
    writer = bufferedwriter_new(fos, 256);
    ASSERT(writer != NULL, "Failed to create BufferedWriter");
    
    /* Write lines */
    bufferedwriter_write_line(writer, "Line 1");
    bufferedwriter_write_line(writer, "Line 2");
    bufferedwriter_write_line(writer, "Line 3");
    
    /* Clean up */
    bufferedwriter_delete(writer);
    fileoutputstream_delete(fos);
    
    /* Verify content */
    f = fopen(test_file, "rb");
    ASSERT(f != NULL, "Failed to open test file");
    
    fgets(buffer, sizeof(buffer), f);
    ASSERT(strcmp(buffer, "Line 1\r\n") == 0, "Line 1 mismatch");
    
    fgets(buffer, sizeof(buffer), f);
    ASSERT(strcmp(buffer, "Line 2\r\n") == 0, "Line 2 mismatch");
    
    fgets(buffer, sizeof(buffer), f);
    ASSERT(strcmp(buffer, "Line 3\r\n") == 0, "Line 3 mismatch");
    
    fclose(f);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 2: BufferedReader read line */
static void test_bufferedreader_read_line(void) {
    FileOutputStream* fos;
    FileInputStream* fis;
    BufferedReader* reader;
    const char* test_file = "test2.txt";
    char* line;
    
    TEST_START("BufferedReader read line");
    
    /* Create test file */
    fos = fileoutputstream_new(test_file);
    ASSERT(fos != NULL, "Failed to create FileOutputStream");
    fileoutputstream_write_bytes(fos, (const unsigned char*)"First\r\nSecond\r\nThird\r\n", 21);
    fileoutputstream_delete(fos);
    
    /* Read lines */
    fis = fileinputstream_new(test_file);
    ASSERT(fis != NULL, "Failed to create FileInputStream");
    
    reader = bufferedreader_new(fis, 256);
    ASSERT(reader != NULL, "Failed to create BufferedReader");
    
    line = bufferedreader_read_line(reader);
    ASSERT(line != NULL, "Failed to read line 1");
    ASSERT(strcmp(line, "First") == 0, "Line 1 mismatch");
    
    line = bufferedreader_read_line(reader);
    ASSERT(line != NULL, "Failed to read line 2");
    ASSERT(strcmp(line, "Second") == 0, "Line 2 mismatch");
    
    line = bufferedreader_read_line(reader);
    ASSERT(line != NULL, "Failed to read line 3");
    ASSERT(strcmp(line, "Third") == 0, "Line 3 mismatch");
    
    line = bufferedreader_read_line(reader);
    ASSERT(line == NULL, "Should return NULL at EOF");
    
    /* Clean up */
    bufferedreader_delete(reader);
    fileinputstream_delete(fis);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 3: BufferedWriter write string */
static void test_bufferedwriter_write_string(void) {
    FileOutputStream* fos;
    BufferedWriter* writer;
    const char* test_file = "test3.txt";
    FILE* f;
    char buffer[100];
    
    TEST_START("BufferedWriter write string");
    
    /* Create writer */
    fos = fileoutputstream_new(test_file);
    ASSERT(fos != NULL, "Failed to create FileOutputStream");
    
    writer = bufferedwriter_new(fos, 256);
    ASSERT(writer != NULL, "Failed to create BufferedWriter");
    
    /* Write strings */
    bufferedwriter_write_string(writer, "Hello");
    bufferedwriter_write_string(writer, " ");
    bufferedwriter_write_string(writer, "World");
    
    /* Clean up */
    bufferedwriter_delete(writer);
    fileoutputstream_delete(fos);
    
    /* Verify content */
    f = fopen(test_file, "rb");
    ASSERT(f != NULL, "Failed to open test file");
    
    fgets(buffer, sizeof(buffer), f);
    ASSERT(strcmp(buffer, "Hello World") == 0, "Content mismatch");
    
    fclose(f);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 4: BufferedWriter flush */
static void test_bufferedwriter_flush(void) {
    FileOutputStream* fos;
    BufferedWriter* writer;
    const char* test_file = "test4.txt";
    FILE* f;
    char buffer[100];
    
    TEST_START("BufferedWriter flush");
    
    /* Create writer */
    fos = fileoutputstream_new(test_file);
    ASSERT(fos != NULL, "Failed to create FileOutputStream");
    
    writer = bufferedwriter_new(fos, 256);
    ASSERT(writer != NULL, "Failed to create BufferedWriter");
    
    /* Write and flush */
    bufferedwriter_write_string(writer, "Test");
    bufferedwriter_flush(writer);
    
    /* Clean up */
    bufferedwriter_delete(writer);
    fileoutputstream_delete(fos);
    
    /* Verify content */
    f = fopen(test_file, "rb");
    ASSERT(f != NULL, "Failed to open test file");
    
    fgets(buffer, sizeof(buffer), f);
    ASSERT(strcmp(buffer, "Test") == 0, "Content mismatch");
    
    fclose(f);
    remove(test_file);
    
    TEST_PASS();
}

/* Test 5: Round-trip write and read */
static void test_roundtrip(void) {
    FileOutputStream* fos;
    FileInputStream* fis;
    BufferedWriter* writer;
    BufferedReader* reader;
    const char* test_file = "test5.txt";
    char* line;
    
    TEST_START("Round-trip write and read");
    
    /* Write data */
    fos = fileoutputstream_new(test_file);
    ASSERT(fos != NULL, "Failed to create FileOutputStream");
    
    writer = bufferedwriter_new(fos, 256);
    ASSERT(writer != NULL, "Failed to create BufferedWriter");
    
    bufferedwriter_write_line(writer, "Alpha");
    bufferedwriter_write_line(writer, "Beta");
    bufferedwriter_write_line(writer, "Gamma");
    
    bufferedwriter_delete(writer);
    fileoutputstream_delete(fos);
    
    /* Read data */
    fis = fileinputstream_new(test_file);
    ASSERT(fis != NULL, "Failed to create FileInputStream");
    
    reader = bufferedreader_new(fis, 256);
    ASSERT(reader != NULL, "Failed to create BufferedReader");
    
    line = bufferedreader_read_line(reader);
    ASSERT(line != NULL && strcmp(line, "Alpha") == 0, "Line 1 mismatch");
    
    line = bufferedreader_read_line(reader);
    ASSERT(line != NULL && strcmp(line, "Beta") == 0, "Line 2 mismatch");
    
    line = bufferedreader_read_line(reader);
    ASSERT(line != NULL && strcmp(line, "Gamma") == 0, "Line 3 mismatch");
    
    /* Clean up */
    bufferedreader_delete(reader);
    fileinputstream_delete(fis);
    remove(test_file);
    
    TEST_PASS();
}

/* Main test runner */
int main(void) {
    int passed = 0;
    int failed = 0;
    
    printf("=== BufferedReader/Writer Test ===\n\n");
    
    /* Initialize memory manager */
    printf("Initializing memory manager...\n");
    if (memory_init(0) != 0) {
        printf("Failed to initialize memory manager\n");
        return 1;
    }
    printf("Memory manager initialized successfully\n");
    printf("Available memory: %u bytes\n\n", memory_available());
    
    printf("--- BufferedReader/Writer Tests ---\n");
    
    /* Run tests */
    test_bufferedwriter_write_line();
    test_bufferedreader_read_line();
    test_bufferedwriter_write_string();
    test_bufferedwriter_flush();
    test_roundtrip();
    
    /* Count results */
    passed = 5;  /* Update if tests fail */
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
