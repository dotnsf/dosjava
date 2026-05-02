#include "system.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* DOS-specific includes for direct console I/O */
#ifdef __WATCOMC__
#include <conio.h>
#include <dos.h>
#endif

/**
 * Initialize System class
 */
int system_init(void) {
    /* Nothing to initialize for now */
    return 0;
}

/**
 * Shutdown System class
 */
void system_shutdown(void) {
    /* Flush any pending output */
    fflush(stdout);
}

/**
 * Print a string to standard output
 */
void system_print(String* str) {
    if (str == NULL || str->data == NULL) {
        return;
    }
    
    printf("%s", str->data);
}

/**
 * Print a C string to standard output
 */
void system_print_cstr(const char* cstr) {
    if (cstr == NULL) {
        return;
    }
    
    printf("%s", cstr);
}

/**
 * Print a string to standard output with newline
 */
void system_println(String* str) {
    if (str == NULL || str->data == NULL) {
        printf("\n");
        return;
    }
    
    printf("%s\n", str->data);
}

/**
 * Print a C string to standard output with newline
 */
void system_println_cstr(const char* cstr) {
    if (cstr == NULL) {
        printf("\n");
        return;
    }
    
    printf("%s\n", cstr);
}

/**
 * Print an integer to standard output
 */
void system_print_int(int16_t value) {
    printf("%d", value);
}

/**
 * Print an integer to standard output with newline
 */
void system_println_int(int16_t value) {
    printf("%d\n", value);
}

/**
 * Print a character to standard output
 */
void system_print_char(char c) {
    putchar(c);
}

/**
 * Print a newline to standard output
 */
void system_println_empty(void) {
    printf("\n");
}

/**
 * Flush output buffer
 */
void system_flush(void) {
    fflush(stdout);
}

/**
 * Get current time in milliseconds
 * Note: DOS has limited time resolution
 */
uint32_t system_currenttimemillis(void) {
#ifdef __WATCOMC__
    /* Use DOS timer tick count (18.2 ticks per second) */
    union REGS regs;
    uint32_t ticks;
    
    regs.h.ah = 0x00;
    int86(0x1A, &regs, &regs);
    
    /* Combine CX:DX to get tick count */
    ticks = ((uint32_t)regs.w.cx << 16) | regs.w.dx;
    
    /* Convert ticks to milliseconds (approximately) */
    return (ticks * 1000) / 18;
#else
    /* Fallback for non-DOS systems */
    return (uint32_t)(clock() * 1000 / CLOCKS_PER_SEC);
#endif
}

/**
 * Exit the program
 */
void system_exit(int status) {
    exit(status);
}


