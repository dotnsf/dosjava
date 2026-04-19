#ifndef CLASSFILE_H
#define CLASSFILE_H

#include "../src/types.h"

/**
 * Java .class file parser
 * Parses standard Java bytecode and converts to .djc format
 */

/* Java .class file magic number */
#define JAVA_MAGIC 0xCAFEBABE

/* Java constant pool tags */
#define CONSTANT_Utf8               1
#define CONSTANT_Integer            3
#define CONSTANT_Float              4
#define CONSTANT_Long               5
#define CONSTANT_Double             6
#define CONSTANT_Class              7
#define CONSTANT_String             8
#define CONSTANT_Fieldref           9
#define CONSTANT_Methodref          10
#define CONSTANT_InterfaceMethodref 11
#define CONSTANT_NameAndType        12

/* Access flags */
#define ACC_PUBLIC      0x0001
#define ACC_PRIVATE     0x0002
#define ACC_PROTECTED   0x0004
#define ACC_STATIC      0x0008
#define ACC_FINAL       0x0010
#define ACC_SUPER       0x0020
#define ACC_VOLATILE    0x0040
#define ACC_TRANSIENT   0x0080

/**
 * Java constant pool entry
 */
typedef struct {
    uint8_t tag;
    union {
        struct {
            uint16_t length;
            char* bytes;
        } utf8;
        int32_t integer;
        struct {
            uint16_t class_index;
        } class_info;
        struct {
            uint16_t string_index;
        } string_info;
        struct {
            uint16_t class_index;
            uint16_t name_and_type_index;
        } ref_info;
        struct {
            uint16_t name_index;
            uint16_t descriptor_index;
        } name_and_type;
    } info;
} JavaConstant;

/**
 * Java method info
 */
typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    
    /* Code attribute (if present) */
    uint16_t max_stack;
    uint16_t max_locals;
    uint32_t code_length;
    uint8_t* code;
} JavaMethod;

/**
 * Java field info
 */
typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
} JavaField;

/**
 * Java .class file structure
 */
typedef struct {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    JavaConstant* constant_pool;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;
    uint16_t interfaces_count;
    uint16_t* interfaces;
    uint16_t fields_count;
    JavaField* fields;
    uint16_t methods_count;
    JavaMethod* methods;
    uint16_t attributes_count;
} JavaClass;

/**
 * Parse a Java .class file
 * @param filename Path to .class file
 * @return Pointer to JavaClass structure, or NULL on error
 */
JavaClass* class_parse(const char* filename);

/**
 * Free a JavaClass structure
 * @param cls JavaClass to free
 */
void class_free(JavaClass* cls);

/**
 * Get UTF8 string from constant pool
 * @param cls JavaClass
 * @param index Constant pool index
 * @return UTF8 string, or NULL if invalid
 */
const char* class_get_utf8(JavaClass* cls, uint16_t index);

/**
 * Get class name from constant pool
 * @param cls JavaClass
 * @param index Constant pool index
 * @return Class name, or NULL if invalid
 */
const char* class_get_classname(JavaClass* cls, uint16_t index);

/**
 * Find method by name
 * @param cls JavaClass
 * @param name Method name
 * @return Pointer to JavaMethod, or NULL if not found
 */
JavaMethod* class_find_method(JavaClass* cls, const char* name);

#endif /* CLASSFILE_H */

// Made with Bob
