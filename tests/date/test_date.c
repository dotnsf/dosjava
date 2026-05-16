#include "../../src/runtime/date.h"
#include "../../src/runtime/dostime.h"
#include <stdio.h>
#include <stdlib.h>

/* Test counter */
static int tests_passed = 0;
static int tests_failed = 0;

/* Test helper macros */
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
 * Test 1: Date creation with current time
 */
void test_date_new(void) {
    Date* date;
    uint32_t time1, time2;
    
    printf("\nTest 1: date_new()\n");
    
    /* Get current time before creating Date */
    time1 = dos_get_timestamp();
    
    /* Create new Date */
    date = date_new();
    TEST_ASSERT(date != NULL, "Date created successfully");
    
    if (date) {
        /* Get time from Date */
        time2 = date_get_time(date);
        
        /* Time should be close to current time (within 2 seconds) */
        TEST_ASSERT(time2 >= time1 && time2 <= time1 + 2, 
                   "Date time is current time");
        
        date_delete(date);
    }
}

/**
 * Test 2: Date creation with specific time
 */
void test_date_new_with_time(void) {
    Date* date;
    uint32_t test_time = 946684800;  /* 2000-01-01 00:00:00 */
    uint32_t retrieved_time;
    
    printf("\nTest 2: date_new_with_time()\n");
    
    /* Create Date with specific time */
    date = date_new_with_time(test_time);
    TEST_ASSERT(date != NULL, "Date created with specific time");
    
    if (date) {
        /* Verify time */
        retrieved_time = date_get_time(date);
        TEST_ASSERT(retrieved_time == test_time, "Time matches");
        
        date_delete(date);
    }
}

/**
 * Test 3: Date component getters
 */
void test_date_getters(void) {
    Date* date;
    uint32_t test_time = 946684800;  /* 2000-01-01 00:00:00 */
    uint16_t year;
    uint8_t month, day, hour, minute, second;
    
    printf("\nTest 3: Date component getters\n");
    
    /* Create Date with known time */
    date = date_new_with_time(test_time);
    TEST_ASSERT(date != NULL, "Date created");
    
    if (date) {
        /* Get components */
        year = date_get_full_year(date);
        month = date_get_month(date);
        day = date_get_date(date);
        hour = date_get_hours(date);
        minute = date_get_minutes(date);
        second = date_get_seconds(date);
        
        printf("  Date: %04u-%02u-%02u %02u:%02u:%02u\n",
               (unsigned int)year, (unsigned int)(month + 1), 
               (unsigned int)day, (unsigned int)hour,
               (unsigned int)minute, (unsigned int)second);
        
        TEST_ASSERT(year == 2000, "Year is 2000");
        TEST_ASSERT(month == 0, "Month is 0 (January)");
        TEST_ASSERT(day == 1, "Day is 1");
        TEST_ASSERT(hour == 0, "Hour is 0");
        TEST_ASSERT(minute == 0, "Minute is 0");
        TEST_ASSERT(second == 0, "Second is 0");
        
        date_delete(date);
    }
}

/**
 * Test 4: date_set_time()
 */
void test_date_set_time(void) {
    Date* date;
    uint32_t time1 = 946684800;  /* 2000-01-01 00:00:00 */
    uint32_t time2 = 1704110400; /* 2024-01-01 12:00:00 */
    uint16_t year;
    uint8_t month, day, hour;
    
    printf("\nTest 4: date_set_time()\n");
    
    /* Create Date with initial time */
    date = date_new_with_time(time1);
    TEST_ASSERT(date != NULL, "Date created");
    
    if (date) {
        /* Verify initial time */
        year = date_get_full_year(date);
        TEST_ASSERT(year == 2000, "Initial year is 2000");
        
        /* Change time */
        date_set_time(date, time2);
        
        /* Verify new time */
        year = date_get_full_year(date);
        month = date_get_month(date);
        day = date_get_date(date);
        hour = date_get_hours(date);
        
        printf("  New date: %04u-%02u-%02u %02u:00:00\n",
               (unsigned int)year, (unsigned int)(month + 1),
               (unsigned int)day, (unsigned int)hour);
        
        TEST_ASSERT(year == 2024, "Year changed to 2024");
        TEST_ASSERT(month == 0, "Month is 0 (January)");
        TEST_ASSERT(day == 1, "Day is 1");
        TEST_ASSERT(hour == 12, "Hour is 12");
        
        date_delete(date);
    }
}

/**
 * Test 5: date_to_string()
 */
void test_date_to_string(void) {
    Date* date;
    uint32_t test_time = 946684800;  /* 2000-01-01 00:00:00 */
    char* str;
    
    printf("\nTest 5: date_to_string()\n");
    
    /* Create Date */
    date = date_new_with_time(test_time);
    TEST_ASSERT(date != NULL, "Date created");
    
    if (date) {
        /* Get string representation */
        str = date_to_string(date);
        TEST_ASSERT(str != NULL, "String created");
        
        if (str) {
            printf("  String: \"%s\"\n", str);
            TEST_ASSERT(str[0] == '2' && str[1] == '0' && 
                       str[2] == '0' && str[3] == '0',
                       "String starts with 2000");
            free(str);
        }
        
        date_delete(date);
    }
}

/**
 * Test 6: Current time test
 */
void test_current_time(void) {
    Date* date;
    uint16_t year;
    uint8_t month, day, hour, minute, second;
    char* str;
    
    printf("\nTest 6: Current time\n");
    
    /* Create Date with current time */
    date = date_new();
    TEST_ASSERT(date != NULL, "Date created with current time");
    
    if (date) {
        /* Get components */
        year = date_get_full_year(date);
        month = date_get_month(date);
        day = date_get_date(date);
        hour = date_get_hours(date);
        minute = date_get_minutes(date);
        second = date_get_seconds(date);
        
        printf("  Current: %04u-%02u-%02u %02u:%02u:%02u\n",
               (unsigned int)year, (unsigned int)(month + 1),
               (unsigned int)day, (unsigned int)hour,
               (unsigned int)minute, (unsigned int)second);
        
        /* Sanity checks */
        TEST_ASSERT(year >= 2000 && year <= 2099, "Year in valid range");
        TEST_ASSERT(month <= 11, "Month in valid range");
        TEST_ASSERT(day >= 1 && day <= 31, "Day in valid range");
        TEST_ASSERT(hour <= 23, "Hour in valid range");
        TEST_ASSERT(minute <= 59, "Minute in valid range");
        TEST_ASSERT(second <= 59, "Second in valid range");
        
        /* Test toString */
        str = date_to_string(date);
        if (str) {
            printf("  String: \"%s\"\n", str);
            free(str);
        }
        
        date_delete(date);
    }
}

/**
 * Main test runner
 */
int main(void) {
    printf("========================================\n");
    printf("Date Class Test Suite\n");
    printf("========================================\n");
    
    test_date_new();
    test_date_new_with_time();
    test_date_getters();
    test_date_set_time();
    test_date_to_string();
    test_current_time();
    
    printf("\n========================================\n");
    printf("Test Summary\n");
    printf("========================================\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("\n");
    
    if (tests_failed == 0) {
        printf("All tests PASSED!\n");
        return 0;
    } else {
        printf("Some tests FAILED!\n");
        return 1;
    }
}

// Made with Bob
