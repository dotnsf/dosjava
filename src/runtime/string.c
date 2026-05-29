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
/**
 * Check if string is empty
 */
uint8_t string_isempty(String* str) {
    if (str == NULL) {
        return 1;  /* NULL is considered empty */
    }
    return (str->length == 0) ? 1 : 0;
}

/**
 * Check if character is whitespace
 */
static int is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

/**
 * Trim leading and trailing whitespace
 */
String* string_trim(String* str) {
    String* result;
    uint16_t start, end;
    uint16_t new_len;
    uint16_t i;
    
    if (str == NULL) {
        return NULL;
    }
    
    /* Find first non-whitespace character */
    start = 0;
    while (start < str->length && is_whitespace(str->data[start])) {
        start++;
    }
    
    /* If all whitespace, return empty string */
    if (start >= str->length) {
        return string_alloc(0);
    }
    
    /* Find last non-whitespace character */
    end = str->length - 1;
    while (end > start && is_whitespace(str->data[end])) {
        end--;
    }
    
    /* Calculate new length */
    new_len = end - start + 1;
    
    /* Allocate result string */
    result = string_alloc(new_len);
    if (result == NULL) {
        return NULL;
    }
    
    /* Copy trimmed content */
    for (i = 0; i < new_len; i++) {
        result->data[i] = str->data[start + i];
    }
    result->data[new_len] = '\0';
    result->length = new_len;
    return result;
}

/**
 * Replace all occurrences of a substring with another substring
 */
String* string_replace(String* str, String* old_str, String* new_str) {
    char result_buf[256];
    uint16_t result_len;
    uint16_t i, j;
    uint16_t old_len, new_len;
    uint8_t match;
    String* result;
    
    if (str == NULL || old_str == NULL || new_str == NULL) {
        return NULL;
    }
    
    old_len = old_str->length;
    new_len = new_str->length;
    
    /* Empty old string - return original */
    if (old_len == 0) {
        return string_alloc(str->length);
    }
    
    result_len = 0;
    i = 0;
    
    while (i < str->length) {
        /* Check if we have a match at current position */
        match = 1;
        if (i + old_len <= str->length) {
            for (j = 0; j < old_len; j++) {
                if (str->data[i + j] != old_str->data[j]) {
                    match = 0;
                    break;
                }
            }
        } else {
            match = 0;
        }
        
        if (match) {
            /* Replace: copy new string */
            if (result_len + new_len >= sizeof(result_buf)) {
                return NULL;  /* Buffer overflow */
            }
            for (j = 0; j < new_len; j++) {
                result_buf[result_len++] = new_str->data[j];
            }
            i += old_len;  /* Skip old string */
        } else {
            /* No match: copy original character */
            if (result_len >= sizeof(result_buf)) {
                return NULL;  /* Buffer overflow */
            }
            result_buf[result_len++] = str->data[i];
            i++;
        }
    }
    
    result_buf[result_len] = '\0';
    
    /* Create result string */
    result = string_alloc(result_len);
    if (result == NULL) {
        return NULL;
    }
    
    for (i = 0; i < result_len; i++) {
        result->data[i] = result_buf[i];
    }
    result->data[result_len] = '\0';
    result->length = result_len;
    
    return result;
}

/**
 * Find last occurrence of substring
 */
int16_t string_lastindexof(String* str, String* search) {
    int16_t i;
    uint16_t j;
    int16_t last_pos;
    int match;
    
    if (str == NULL || search == NULL) {
        return -1;
    }
    
    /* Empty search string returns string length */
    if (search->length == 0) {
        return (int16_t)str->length;
    }
    
    /* Search string longer than source string */
    if (search->length > str->length) {
        return -1;
    }
    
    last_pos = -1;
    
    /* Search from end to beginning */
    for (i = (int16_t)(str->length - search->length); i >= 0; i--) {
        match = 1;
        for (j = 0; j < search->length; j++) {
            if (str->data[i + j] != search->data[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            return i;  /* Return first match from end */
        }
    }
    return -1;
}

/**
 * Check if string contains substring
 * Uses strstr() for efficient substring search
 */
uint8_t string_contains(String* str, String* search) {
    if (str == NULL || search == NULL) {
        return 0;
    }
    
    /* Empty search string is always contained */
    if (search->length == 0) {
        return 1;
    }
    /* Use strstr to check if substring exists */
    return (strstr(str->data, search->data) != NULL) ? 1 : 0;
}

/**
 * Repeat string n times
 */
String* string_repeat(String* str, uint16_t count) {
    String* result;
    uint16_t total_len;
    uint16_t i, j;
    uint16_t pos;
    
    if (str == NULL) {
        return NULL;
    }
    
    /* count = 0 returns empty string */
    if (count == 0) {
        return string_alloc(0);
    }
    
    /* count = 1 returns copy of original */
    if (count == 1) {
        return string_substring(str, 0, str->length);
    }
    
    /* Calculate total length */
    total_len = str->length * count;
    
    /* Check for overflow or excessive length */
    if (total_len / count != str->length || total_len > 255) {
        return NULL;  /* Overflow or too long */
    }
    
    /* Allocate result string */
    result = string_alloc(total_len);
    if (result == NULL) {
        return NULL;
    }
    
    /* Copy string count times */
    pos = 0;
    for (i = 0; i < count; i++) {
        for (j = 0; j < str->length; j++) {
            result->data[pos++] = str->data[j];
        }
    }
    result->data[total_len] = '\0';
    result->length = total_len;
    
    return result;
}








