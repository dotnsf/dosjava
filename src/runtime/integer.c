/**
 * Integer class implementation
 * C implementation of java.lang.Integer
 */

#include "integer.h"
#include "../vm/memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Class ID for Integer */
#define CLASS_INTEGER 3

/* Integer constants */
#define INT16_MAX_VALUE 32767
#define INT16_MIN_VALUE -32768

/**
 * Create a new Integer from an int value
 */
Integer* integer_new(int16_t value) {
    Integer* i = (Integer*)memory_alloc(sizeof(Integer));
    if (!i) {
        return NULL;
    }
    
    i->base.class_id = CLASS_INTEGER;
    i->base.ref_count = 1;
    i->value = value;
    
    return i;
}

/**
 * Delete an Integer instance
 */
void integer_delete(Integer* i) {
    if (!i) {
        return;
    }
    
    memory_free(i);
}

/**
 * Get the int value
 */
int16_t integer_value(Integer* i) {
    if (!i) {
        return 0;
    }
    return i->value;
}

/**
 * Compare two Integers for equality
 */
uint8_t integer_equals(Integer* i1, Integer* i2) {
    if (!i1 || !i2) {
        return 0;
    }
    return i1->value == i2->value;
}

/**
 * Compare two Integers
 */
int16_t integer_compare(Integer* i1, Integer* i2) {
    if (!i1 || !i2) {
        return 0;
    }
    
    if (i1->value < i2->value) {
        return -1;
    } else if (i1->value > i2->value) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Convert Integer to String
 */
String* integer_tostring(Integer* i) {
    if (!i) {
        return NULL;
    }
    return integer_tostring_static(i->value);
}

/**
 * Parse string to Integer
 */
Integer* integer_parse(const char* str) {
    int16_t value;
    if (!integer_parseint(str, &value)) {
        return NULL;
    }
    return integer_new(value);
}

/**
 * Get hash code for Integer
 */
uint16_t integer_hashcode(Integer* i) {
    if (!i) {
        return 0;
    }
    /* Simple hash: use the value itself */
    return (uint16_t)i->value;
}

/**
 * Convert int to String (static utility)
 */
String* integer_tostring_static(int16_t value) {
    char buffer[8];  /* Enough for -32768 to 32767 */
    
    /* Convert int to string */
    sprintf(buffer, "%d", value);
    
    return string_new(buffer);
}

/**
 * Parse string to int (static utility)
 */
uint8_t integer_parseint(const char* str, int16_t* result) {
    int32_t value;
    int8_t sign;
    const char* p;
    
    if (!str || !result) {
        return 0;
    }
    
    value = 0;
    sign = 1;
    p = str;
    
    /* Skip whitespace */
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    
    /* Check for sign */
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }
    
    /* Parse digits */
    if (*p < '0' || *p > '9') {
        return 0;  /* No digits found */
    }
    
    while (*p >= '0' && *p <= '9') {
        value = value * 10 + (*p - '0');
        
        /* Check for overflow */
        if (value > 32767) {
            return 0;
        }
        
        p++;
    }
    
    /* Skip trailing whitespace */
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    
    /* Check if we consumed the entire string */
    if (*p != '\0') {
        return 0;  /* Invalid characters */
    }
    
    value *= sign;
    
    /* Check range */
    if (value < INT16_MIN_VALUE || value > INT16_MAX_VALUE) {
        return 0;
    }
    
    *result = (int16_t)value;
    return 1;
}

/**
 * Get maximum int value
 */
int16_t integer_max_value(void) {
    return INT16_MAX_VALUE;
}

/**
 * Get minimum int value
 */
int16_t integer_min_value(void) {
    return INT16_MIN_VALUE;
}

// Made with Bob
