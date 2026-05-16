/**
 * Test program for InputStream and OutputStream base classes
 * 
 * This tests the basic functionality of the stream base classes
 */

#include "../../src/runtime/inputstream.h"
#include "../../src/runtime/outputstream.h"
#include <stdio.h>
#include <string.h>

/* Test results */
static int tests_passed = 0;
static int tests_failed = 0;

/* Test helper macros */
#define TEST(name) printf("Testing %s... ", name)
#define PASS() do { printf("PASS\n"); tests_passed++; } while(0)
#define FAIL(msg) do { printf("FAIL: %s\n", msg); tests_failed++; } while(0)
#define ASSERT(cond, msg) if (!(cond)) { FAIL(msg); return; } else { PASS(); }

/**
 * Test InputStream initialization
 */
void test_inputstream_init(void) {
    InputStream stream;
    int result;
    
    TEST("InputStream initialization");
    
    result = inputstream_init(&stream);
    
    if (result != 0) {
        FAIL("init returned error");
        return;
    }
    
    if (!stream.is_open) {
        FAIL("stream not marked as open");
        return;
    }
    
    if (stream.position != 0) {
        FAIL("position not initialized to 0");
        return;
    }
    
    if (stream.base.ref_count != 1) {
        FAIL("ref_count not initialized to 1");
        return;
    }
    
    PASS();
}

/**
 * Test InputStream close
 */
void test_inputstream_close(void) {
    InputStream stream;
    
    TEST("InputStream close");
    
    inputstream_init(&stream);
    inputstream_close(&stream);
    
    if (stream.is_open) {
        FAIL("stream still marked as open after close");
        return;
    }
    
    PASS();
}

/**
 * Test InputStream is_open check
 */
void test_inputstream_is_open(void) {
    InputStream stream;
    
    TEST("InputStream is_open check");
    
    inputstream_init(&stream);
    
    if (!inputstream_is_open(&stream)) {
        FAIL("is_open returned false for open stream");
        return;
    }
    
    inputstream_close(&stream);
    
    if (inputstream_is_open(&stream)) {
        FAIL("is_open returned true for closed stream");
        return;
    }
    
    PASS();
}

/**
 * Test InputStream position tracking
 */
void test_inputstream_position(void) {
    InputStream stream;
    
    TEST("InputStream position tracking");
    
    inputstream_init(&stream);
    
    if (inputstream_get_position(&stream) != 0) {
        FAIL("initial position not 0");
        return;
    }
    
    stream.position = 42;
    
    if (inputstream_get_position(&stream) != 42) {
        FAIL("position not updated correctly");
        return;
    }
    
    PASS();
}

/**
 * Test OutputStream initialization
 */
void test_outputstream_init(void) {
    OutputStream stream;
    int result;
    
    TEST("OutputStream initialization");
    
    result = outputstream_init(&stream);
    
    if (result != 0) {
        FAIL("init returned error");
        return;
    }
    
    if (!stream.is_open) {
        FAIL("stream not marked as open");
        return;
    }
    
    if (stream.position != 0) {
        FAIL("position not initialized to 0");
        return;
    }
    
    if (stream.base.ref_count != 1) {
        FAIL("ref_count not initialized to 1");
        return;
    }
    
    PASS();
}

/**
 * Test OutputStream close
 */
void test_outputstream_close(void) {
    OutputStream stream;
    
    TEST("OutputStream close");
    
    outputstream_init(&stream);
    outputstream_close(&stream);
    
    if (stream.is_open) {
        FAIL("stream still marked as open after close");
        return;
    }
    
    PASS();
}

/**
 * Test OutputStream is_open check
 */
void test_outputstream_is_open(void) {
    OutputStream stream;
    
    TEST("OutputStream is_open check");
    
    outputstream_init(&stream);
    
    if (!outputstream_is_open(&stream)) {
        FAIL("is_open returned false for open stream");
        return;
    }
    
    outputstream_close(&stream);
    
    if (outputstream_is_open(&stream)) {
        FAIL("is_open returned true for closed stream");
        return;
    }
    
    PASS();
}

/**
 * Test OutputStream position tracking
 */
void test_outputstream_position(void) {
    OutputStream stream;
    
    TEST("OutputStream position tracking");
    
    outputstream_init(&stream);
    
    if (outputstream_get_position(&stream) != 0) {
        FAIL("initial position not 0");
        return;
    }
    
    stream.position = 42;
    
    if (outputstream_get_position(&stream) != 42) {
        FAIL("position not updated correctly");
        return;
    }
    
    PASS();
}

/**
 * Main test function
 */
int main(void) {
    printf("=== Stream Base Classes Test ===\n\n");
    
    /* InputStream tests */
    printf("--- InputStream Tests ---\n");
    test_inputstream_init();
    test_inputstream_close();
    test_inputstream_is_open();
    test_inputstream_position();
    
    printf("\n");
    
    /* OutputStream tests */
    printf("--- OutputStream Tests ---\n");
    test_outputstream_init();
    test_outputstream_close();
    test_outputstream_is_open();
    test_outputstream_position();
    
    /* Summary */
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    
    if (tests_failed == 0) {
        printf("\nAll tests PASSED!\n");
        return 0;
    } else {
        printf("\nSome tests FAILED!\n");
        return 1;
    }
}
