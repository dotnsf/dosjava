#ifndef STRING_H
#define STRING_H

#include "../types.h"
#include "object.h"

/**
 * String structure - C representation of java.lang.String
 * 
 * Strings are immutable objects containing character data
 */
typedef struct String {
    Object base;             /* Base Object structure */
    uint16_t length;         /* String length */
    char* data;              /* Character data (null-terminated) */
} String;

/**
 * Create a new String from a C string
 * @param cstr C string (null-terminated)
 * @return Pointer to new String, or NULL on error
 */
String* string_new(const char* cstr);

/**
 * Create a new String with specified length
 * @param length String length
 * @return Pointer to new String, or NULL on error
 */
String* string_alloc(uint16_t length);

/**
 * Delete a String instance
 * @param str String to delete
 */
void string_delete(String* str);

/**
 * Get string length
 * @param str String
 * @return Length of string
 */
uint16_t string_length(String* str);

/**
 * Get character at index
 * @param str String
 * @param index Character index
 * @return Character at index, or 0 if out of bounds
 */
char string_charat(String* str, uint16_t index);

/**
 * Concatenate two strings
 * @param s1 First string
 * @param s2 Second string
 * @return New string containing s1 + s2, or NULL on error
 */
String* string_concat(String* s1, String* s2);

/**
 * Compare two strings for equality
 * @param s1 First string
 * @param s2 Second string
 * @return 1 if equal, 0 otherwise
 */
uint8_t string_equals(String* s1, String* s2);

/**
 * Compare two strings lexicographically
 * @param s1 First string
 * @param s2 Second string
 * @return <0 if s1<s2, 0 if s1==s2, >0 if s1>s2
 */
int16_t string_compare(String* s1, String* s2);

/**
 * Get substring
 * @param str Source string
 * @param start Start index (inclusive)
 * @param end End index (exclusive)
 * @return New string containing substring, or NULL on error
 */
String* string_substring(String* str, uint16_t start, uint16_t end);

/**
 * Convert string to C string
 * @param str String
 * @return Pointer to internal C string (do not free)
 */
const char* string_tocstr(String* str);

/**
 * Create string from integer
 * @param value Integer value
 * @return New string representation, or NULL on error
 */
String* string_fromint(int16_t value);

/**
 * Get hash code for string
 * @param str String
 * @return Hash code
 */
uint16_t string_hashcode(String* str);

#endif /* STRING_H */


