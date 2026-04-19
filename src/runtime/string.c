#include "string.h"
#include "../vm/memory.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Class ID for String (arbitrary value) */
#define STRING_CLASS_ID 2

/**
 * Create a new String from a C string
 */
String* string_new(const char* cstr) {
    String* str;
    uint16_t len;
    
    if (cstr == NULL) {
        return NULL;
    }
    
    len = (uint16_t)strlen(cstr);
    
    /* Allocate String structure */
    str = (String*)memory_alloc(sizeof(String));
    if (str == NULL) {
        return NULL;
    }
    
    /* Initialize base Object */
    str->base.class_id = STRING_CLASS_ID;
    str->base.ref_count = 1;
    
    /* Allocate and copy string data */
    str->length = len;
    str->data = (char*)memory_alloc(len + 1);
    if (str->data == NULL) {
        memory_free(str);
        return NULL;
    }
    
    strcpy(str->data, cstr);
    
    return str;
}

/**
 * Create a new String with specified length
 */
String* string_alloc(uint16_t length) {
    String* str;
    
    /* Allocate String structure */
    str = (String*)memory_alloc(sizeof(String));
    if (str == NULL) {
        return NULL;
    }
    
    /* Initialize base Object */
    str->base.class_id = STRING_CLASS_ID;
    str->base.ref_count = 1;
    
    /* Allocate string data */
    str->length = length;
    str->data = (char*)memory_alloc(length + 1);
    if (str->data == NULL) {
        memory_free(str);
        return NULL;
    }
    
    /* Initialize to empty string */
    str->data[0] = '\0';
    
    return str;
}

/**
 * Delete a String instance
 */
void string_delete(String* str) {
    if (str == NULL) {
        return;
    }
    
    /* Free string data */
    if (str->data != NULL) {
        memory_free(str->data);
    }
    
    /* Free String structure */
    memory_free(str);
}

/**
 * Get string length
 */
uint16_t string_length(String* str) {
    if (str == NULL) {
        return 0;
    }
    return str->length;
}

/**
 * Get character at index
 */
char string_charat(String* str, uint16_t index) {
    if (str == NULL || index >= str->length) {
        return 0;
    }
    return str->data[index];
}

/**
 * Concatenate two strings
 */
String* string_concat(String* s1, String* s2) {
    String* result;
    uint16_t total_len;
    
    if (s1 == NULL || s2 == NULL) {
        return NULL;
    }
    
    total_len = s1->length + s2->length;
    
    /* Allocate result string */
    result = string_alloc(total_len);
    if (result == NULL) {
        return NULL;
    }
    
    /* Copy strings */
    strcpy(result->data, s1->data);
    strcat(result->data, s2->data);
    result->length = total_len;
    
    return result;
}

/**
 * Compare two strings for equality
 */
uint8_t string_equals(String* s1, String* s2) {
    if (s1 == NULL || s2 == NULL) {
        return (s1 == s2) ? 1 : 0;
    }
    
    if (s1->length != s2->length) {
        return 0;
    }
    
    return (strcmp(s1->data, s2->data) == 0) ? 1 : 0;
}

/**
 * Compare two strings lexicographically
 */
int16_t string_compare(String* s1, String* s2) {
    if (s1 == NULL || s2 == NULL) {
        if (s1 == s2) return 0;
        return (s1 == NULL) ? -1 : 1;
    }
    
    return (int16_t)strcmp(s1->data, s2->data);
}

/**
 * Get substring
 */
String* string_substring(String* str, uint16_t start, uint16_t end) {
    String* result;
    uint16_t len;
    uint16_t i;
    
    if (str == NULL || start >= str->length || end > str->length || start >= end) {
        return NULL;
    }
    
    len = end - start;
    
    /* Allocate result string */
    result = string_alloc(len);
    if (result == NULL) {
        return NULL;
    }
    
    /* Copy substring */
    for (i = 0; i < len; i++) {
        result->data[i] = str->data[start + i];
    }
    result->data[len] = '\0';
    result->length = len;
    
    return result;
}

/**
 * Convert string to C string
 */
const char* string_tocstr(String* str) {
    if (str == NULL) {
        return NULL;
    }
    return str->data;
}

/**
 * Create string from integer
 */
String* string_fromint(int16_t value) {
    char buffer[8];  /* Enough for 16-bit int */
    
    sprintf(buffer, "%d", value);
    return string_new(buffer);
}

/**
 * Get hash code for string
 */
uint16_t string_hashcode(String* str) {
    uint16_t hash;
    uint16_t i;
    
    if (str == NULL) {
        return 0;
    }
    
    /* Simple hash function */
    hash = 0;
    for (i = 0; i < str->length; i++) {
        hash = (hash * 31) + (uint8_t)str->data[i];
    }
    
    return hash;
}

// Made with Bob
