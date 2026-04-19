#ifndef INTEGER_H
#define INTEGER_H

#include "../types.h"
#include "object.h"
#include "string.h"

/**
 * Integer structure - C representation of java.lang.Integer
 * 
 * Wrapper class for primitive int values
 */
typedef struct Integer {
    Object base;             /* Base Object structure */
    int16_t value;           /* Integer value */
} Integer;

/**
 * Create a new Integer from an int value
 * @param value Integer value
 * @return Pointer to new Integer, or NULL on error
 */
Integer* integer_new(int16_t value);

/**
 * Delete an Integer instance
 * @param i Integer to delete
 */
void integer_delete(Integer* i);

/**
 * Get the int value
 * @param i Integer
 * @return Integer value
 */
int16_t integer_value(Integer* i);

/**
 * Compare two Integers for equality
 * @param i1 First Integer
 * @param i2 Second Integer
 * @return 1 if equal, 0 otherwise
 */
uint8_t integer_equals(Integer* i1, Integer* i2);

/**
 * Compare two Integers
 * @param i1 First Integer
 * @param i2 Second Integer
 * @return <0 if i1<i2, 0 if i1==i2, >0 if i1>i2
 */
int16_t integer_compare(Integer* i1, Integer* i2);

/**
 * Convert Integer to String
 * @param i Integer
 * @return String representation, or NULL on error
 */
String* integer_tostring(Integer* i);

/**
 * Parse string to Integer
 * @param str String to parse
 * @return New Integer, or NULL on error
 */
Integer* integer_parse(const char* str);

/**
 * Get hash code for Integer
 * @param i Integer
 * @return Hash code
 */
uint16_t integer_hashcode(Integer* i);

/* Static utility methods */

/**
 * Convert int to String
 * @param value Integer value
 * @return String representation, or NULL on error
 */
String* integer_tostring_static(int16_t value);

/**
 * Parse string to int
 * @param str String to parse
 * @param result Pointer to store result
 * @return 1 on success, 0 on error
 */
uint8_t integer_parseint(const char* str, int16_t* result);

/**
 * Get maximum int value
 * @return Maximum int16_t value (32767)
 */
int16_t integer_max_value(void);

/**
 * Get minimum int value
 * @return Minimum int16_t value (-32768)
 */
int16_t integer_min_value(void);

#endif /* INTEGER_H */

// Made with Bob
