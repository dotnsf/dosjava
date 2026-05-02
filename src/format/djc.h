#ifndef DJC_H
#define DJC_H

#include "../types.h"

/* DOS Java Bytecode (.djc) File Format */

/* Magic number for .djc files: 'DJ' */
#define DJC_MAGIC 0x444A

/* Current format version */
#define DJC_VERSION 0x0001

/* Maximum sizes */
#define DJC_MAX_CONSTANTS 256
#define DJC_MAX_METHODS 64
#define DJC_MAX_FIELDS 64
#define DJC_MAX_CODE_SIZE 32768

/* Constant pool entry types */
#define CONST_UTF8        1
#define CONST_INTEGER     2
#define CONST_STRING_REF  3
#define CONST_CLASS_REF   4
#define CONST_METHOD_REF  5
#define CONST_FIELD_REF   6

/* Method flags */
#define METHOD_STATIC     0x01
#define METHOD_PUBLIC     0x02
#define METHOD_PRIVATE    0x04
#define METHOD_NATIVE     0x08

/* Field flags */
#define FIELD_STATIC      0x01
#define FIELD_PUBLIC      0x02
#define FIELD_PRIVATE     0x04
#define FIELD_FINAL       0x08

/**
 * DJC file header structure
 */
typedef struct {
    uint16_t magic;              /* Magic number (0x444A) */
    uint16_t version;            /* Format version */
    uint16_t constant_pool_count;/* Number of constants */
    uint16_t method_count;       /* Number of methods */
    uint16_t field_count;        /* Number of fields */
    uint16_t code_size;          /* Total bytecode size */
} DJCHeader;

/**
 * Constant pool entry
 */
typedef struct {
    uint8_t tag;                 /* Constant type */
    uint16_t length;             /* Data length */
    union {
        char* utf8_data;         /* UTF8 string */
        int16_t int_value;       /* Integer value */
        uint16_t ref_index;      /* Reference to another constant */
    } data;
} DJCConstant;

/**
 * Method descriptor
 */
typedef struct {
    uint16_t name_index;         /* Index to constant pool (method name) */
    uint16_t descriptor_index;   /* Index to constant pool (type descriptor) */
    uint16_t code_offset;        /* Offset in code section */
    uint16_t code_length;        /* Length of bytecode */
    uint8_t max_stack;           /* Maximum stack depth */
    uint8_t max_locals;          /* Maximum local variables */
    uint8_t flags;               /* Method flags */
} DJCMethod;

/**
 * Field descriptor
 */
typedef struct {
    uint16_t name_index;         /* Index to constant pool (field name) */
    uint16_t descriptor_index;   /* Index to constant pool (type descriptor) */
    uint8_t flags;               /* Field flags */
} DJCField;

/**
 * Complete DJC file structure in memory
 */
typedef struct {
    DJCHeader header;
    DJCConstant* constants;      /* Constant pool array */
    DJCMethod* methods;          /* Method array */
    DJCField* fields;            /* Field array */
    uint8_t* bytecode;           /* Bytecode section */
} DJCFile;

/* Function prototypes */

/**
 * Open and read a .djc file
 * @param filename Path to .djc file
 * @return Pointer to DJCFile structure, or NULL on error
 */
DJCFile* djc_open(const char* filename);

/**
 * Close and free a DJC file
 * @param file DJC file to close
 */
void djc_close(DJCFile* file);

/**
 * Get a constant from the constant pool
 * @param file DJC file
 * @param index Constant index
 * @return Pointer to constant, or NULL if invalid
 */
DJCConstant* djc_get_constant(DJCFile* file, uint16_t index);

/**
 * Find method by index
 * @param file DJC file
 * @param method_index Method index
 * @return Pointer to method, or NULL if not found
 */
DJCMethod* djc_find_method(DJCFile* file, uint16_t method_index);

/**
 * Find method by name
 * @param file DJC file
 * @param method_name Method name to search for
 * @return Pointer to method, or NULL if not found
 */
DJCMethod* djc_find_method_by_name(DJCFile* file, const char* method_name);

/**
 * Get a UTF8 string from the constant pool
 * @param file DJC file
 * @param index Constant index
 * @return Pointer to string, or NULL if invalid
 */
const char* djc_get_utf8(DJCFile* file, uint16_t index);

/**
 * Get bytecode for a method
 * @param file DJC file
 * @param method Method descriptor
 * @return Pointer to bytecode, or NULL if invalid
 */
uint8_t* djc_get_method_code(DJCFile* file, DJCMethod* method);

#endif /* DJC_H */


