#ifndef SYSTEM_H
#define SYSTEM_H

#include "string.h"

/**
 * System class - C representation of java.lang.System
 * 
 * Provides system-level functionality, primarily console I/O
 */

/**
 * Initialize System class
 * @return 0 on success, -1 on error
 */
int system_init(void);

/**
 * Shutdown System class
 */
void system_shutdown(void);

/**
 * Print a string to standard output
 * @param str String to print
 */
void system_print(String* str);

/**
 * Print a C string to standard output
 * @param cstr C string to print
 */
void system_print_cstr(const char* cstr);

/**
 * Print a string to standard output with newline
 * @param str String to print
 */
void system_println(String* str);

/**
 * Print a C string to standard output with newline
 * @param cstr C string to print
 */
void system_println_cstr(const char* cstr);

/**
 * Print an integer to standard output
 * @param value Integer value to print
 */
void system_print_int(int16_t value);

/**
 * Print an integer to standard output with newline
 * @param value Integer value to print
 */
void system_println_int(int16_t value);

/**
 * Print a character to standard output
 * @param c Character to print
 */
void system_print_char(char c);

/**
 * Print a newline to standard output
 */
void system_println_empty(void);

/**
 * Flush output buffer
 */
void system_flush(void);

/**
 * Get current time in milliseconds (if available)
 * @return Time in milliseconds, or 0 if not available
 */
uint32_t system_currenttimemillis(void);

/**
 * Exit the program
 * @param status Exit status code
 */
void system_exit(int status);

#endif /* SYSTEM_H */

// Made with Bob
