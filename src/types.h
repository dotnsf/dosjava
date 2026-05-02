#ifndef TYPES_H
#define TYPES_H

/**
 * Common type definitions for DOS Java
 * 
 * Provides portable integer types for 16-bit DOS environment
 * Compatible with Open Watcom C compiler
 */

/* Define types manually for Open Watcom compatibility */
typedef signed char        int8_t;
typedef unsigned char      uint8_t;
typedef signed short       int16_t;
typedef unsigned short     uint16_t;
typedef signed long        int32_t;
typedef unsigned long      uint32_t;

/* Pointer-sized integer types */
#if defined(__SMALL__) || defined(__MEDIUM__)
    typedef unsigned short uintptr_t;
    typedef signed short   intptr_t;
#else
    typedef unsigned long  uintptr_t;
    typedef signed long    intptr_t;
#endif

#endif /* TYPES_H */
