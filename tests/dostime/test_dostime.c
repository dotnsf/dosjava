/**
 * DOS Time API Test Program
 */

#include <stdio.h>
#include <stdlib.h>
#include "../../src/runtime/dostime.h"

/* Test result tracking */
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("  PASS: %s\n", message); \
            tests_passed++; \
        } else { \
            printf("  FAIL: %s\n", message); \
            tests_failed++; \
        } \
    } while(0)

/**
 * Test 1: dos_get_datetime()
 */
void test_get_datetime(void) {
    DOSDateTime dt;
    int result;
    
    printf("\nTest 1: dos_get_datetime()\n");
    
    result = dos_get_datetime(&dt);
    TEST_ASSERT(result == 0, "Function returns success");
    TEST_ASSERT(dt.year >= 1980 && dt.year <= 2099, "Year in valid range");
    TEST_ASSERT(dt.month >= 1 && dt.month <= 12, "Month in valid range");
    TEST_ASSERT(dt.day >= 1 && dt.day <= 31, "Day in valid range");
    TEST_ASSERT(dt.hour <= 23, "Hour in valid range");
    TEST_ASSERT(dt.minute <= 59, "Minute in valid range");
    TEST_ASSERT(dt.second <= 59, "Second in valid range");
    
    printf("  Current time: %04u-%02u-%02u %02u:%02u:%02u\n",
           dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}

/**
 * Test 2: dos_datetime_to_timestamp()
 */
void test_datetime_to_timestamp(void) {
    DOSDateTime dt;
    uint32_t timestamp;
    
    printf("\nTest 2: dos_datetime_to_timestamp()\n");
    
    /* Test known date: 2000-01-01 00:00:00 */
    dt.year = 2000;
    dt.month = 1;
    dt.day = 1;
    dt.hour = 0;
    dt.minute = 0;
    dt.second = 0;
    dt.hundredths = 0;
    
    timestamp = dos_datetime_to_timestamp(&dt);
    printf("  2000-01-01 00:00:00 = %lu seconds\n", timestamp);
    TEST_ASSERT(timestamp == 946684800UL, "2000-01-01 timestamp correct");
    
    /* Test another date: 1980-01-01 00:00:00 (DOS epoch) */
    dt.year = 1980;
    dt.month = 1;
    dt.day = 1;
    dt.hour = 0;
    dt.minute = 0;
    dt.second = 0;
    
    timestamp = dos_datetime_to_timestamp(&dt);
    printf("  1980-01-01 00:00:00 = %lu seconds\n", timestamp);
    TEST_ASSERT(timestamp == 315532800UL, "1980-01-01 timestamp correct");
}

/**
 * Test 3: dos_timestamp_to_datetime()
 */
void test_timestamp_to_datetime(void) {
    DOSDateTime dt;
    int result;
    
    printf("\nTest 3: dos_timestamp_to_datetime()\n");
    
    /* Test known timestamp: 946684800 = 2000-01-01 00:00:00 */
    result = dos_timestamp_to_datetime(946684800UL, &dt);
    TEST_ASSERT(result == 0, "Function returns success");
    TEST_ASSERT(dt.year == 2000, "Year correct");
    TEST_ASSERT(dt.month == 1, "Month correct");
    TEST_ASSERT(dt.day == 1, "Day correct");
    TEST_ASSERT(dt.hour == 0, "Hour correct");
    TEST_ASSERT(dt.minute == 0, "Minute correct");
    TEST_ASSERT(dt.second == 0, "Second correct");
    
    printf("  946684800 = %04u-%02u-%02u %02u:%02u:%02u\n",
           dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}

/**
 * Test 4: Round-trip conversion
 */
void test_roundtrip(void) {
    DOSDateTime dt1, dt2;
    uint32_t timestamp;
    int result;
    
    printf("\nTest 4: Round-trip conversion\n");
    
    /* Get current time */
    result = dos_get_datetime(&dt1);
    TEST_ASSERT(result == 0, "Get current time");
    
    /* Convert to timestamp */
    timestamp = dos_datetime_to_timestamp(&dt1);
    printf("  Original: %04u-%02u-%02u %02u:%02u:%02u\n",
           dt1.year, dt1.month, dt1.day, dt1.hour, dt1.minute, dt1.second);
    printf("  Timestamp: %lu\n", timestamp);
    
    /* Convert back to datetime */
    result = dos_timestamp_to_datetime(timestamp, &dt2);
    TEST_ASSERT(result == 0, "Convert back to datetime");
    
    printf("  Converted: %04u-%02u-%02u %02u:%02u:%02u\n",
           dt2.year, dt2.month, dt2.day, dt2.hour, dt2.minute, dt2.second);
    
    /* Check if values match (ignore hundredths) */
    TEST_ASSERT(dt1.year == dt2.year, "Year matches");
    TEST_ASSERT(dt1.month == dt2.month, "Month matches");
    TEST_ASSERT(dt1.day == dt2.day, "Day matches");
    TEST_ASSERT(dt1.hour == dt2.hour, "Hour matches");
    TEST_ASSERT(dt1.minute == dt2.minute, "Minute matches");
    TEST_ASSERT(dt1.second == dt2.second, "Second matches");
}

/**
 * Main test runner
 */
int main(void) {
    printf("=== DOS Time API Tests ===\n");
    
    test_get_datetime();
    test_datetime_to_timestamp();
    test_timestamp_to_datetime();
    test_roundtrip();
    
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

// Made with Bob
