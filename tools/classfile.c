/**
 * Java .class file parser implementation
 */

#include "classfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper function to read big-endian uint16 */
static uint16_t read_u2(FILE* fp) {
    uint8_t b1 = fgetc(fp);
    uint8_t b2 = fgetc(fp);
    return (b1 << 8) | b2;
}

/* Helper function to read big-endian uint32 */
static uint32_t read_u4(FILE* fp) {
    uint8_t b1 = fgetc(fp);
    uint8_t b2 = fgetc(fp);
    uint8_t b3 = fgetc(fp);
    uint8_t b4 = fgetc(fp);
    return ((uint32_t)b1 << 24) | ((uint32_t)b2 << 16) | 
           ((uint32_t)b3 << 8) | b4;
}

/* Parse constant pool */
static int parse_constant_pool(FILE* fp, JavaClass* cls) {
    uint16_t i;
    
    cls->constant_pool = (JavaConstant*)calloc(cls->constant_pool_count, 
                                               sizeof(JavaConstant));
    if (!cls->constant_pool) {
        return 0;
    }
    
    /* Note: constant pool indices start at 1, not 0 */
    for (i = 1; i < cls->constant_pool_count; i++) {
        uint8_t tag = fgetc(fp);
        cls->constant_pool[i].tag = tag;
        
        switch (tag) {
            case CONSTANT_Utf8: {
                uint16_t length = read_u2(fp);
                cls->constant_pool[i].info.utf8.length = length;
                cls->constant_pool[i].info.utf8.bytes = (char*)malloc(length + 1);
                if (!cls->constant_pool[i].info.utf8.bytes) {
                    return 0;
                }
                fread(cls->constant_pool[i].info.utf8.bytes, 1, length, fp);
                cls->constant_pool[i].info.utf8.bytes[length] = '\0';
                break;
            }
            
            case CONSTANT_Integer:
                cls->constant_pool[i].info.integer = (int32_t)read_u4(fp);
                break;
            
            case CONSTANT_Float:
                /* Skip float (4 bytes) - not supported in 16-bit */
                read_u4(fp);
                break;
            
            case CONSTANT_Long:
            case CONSTANT_Double:
                /* Skip 8 bytes and increment i (takes 2 slots) */
                read_u4(fp);
                read_u4(fp);
                i++;
                break;
            
            case CONSTANT_Class:
                cls->constant_pool[i].info.class_info.class_index = read_u2(fp);
                break;
            
            case CONSTANT_String:
                cls->constant_pool[i].info.string_info.string_index = read_u2(fp);
                break;
            
            case CONSTANT_Fieldref:
            case CONSTANT_Methodref:
            case CONSTANT_InterfaceMethodref:
                cls->constant_pool[i].info.ref_info.class_index = read_u2(fp);
                cls->constant_pool[i].info.ref_info.name_and_type_index = read_u2(fp);
                break;
            
            case CONSTANT_NameAndType:
                cls->constant_pool[i].info.name_and_type.name_index = read_u2(fp);
                cls->constant_pool[i].info.name_and_type.descriptor_index = read_u2(fp);
                break;
            
            default:
                fprintf(stderr, "Unknown constant pool tag: %d\n", tag);
                return 0;
        }
    }
    
    return 1;
}

/* Parse method code attribute */
static int parse_code_attribute(FILE* fp, JavaMethod* method) {
    uint32_t attr_length;
    uint16_t exception_table_length;
    uint16_t i;
    uint16_t attributes_count;
    uint32_t length;
    
    attr_length = read_u4(fp);
    
    method->max_stack = read_u2(fp);
    method->max_locals = read_u2(fp);
    method->code_length = read_u4(fp);
    
    /* Allocate and read code */
    method->code = (uint8_t*)malloc(method->code_length);
    if (!method->code) {
        return 0;
    }
    fread(method->code, 1, method->code_length, fp);
    
    /* Skip exception table */
    exception_table_length = read_u2(fp);
    for (i = 0; i < exception_table_length; i++) {
        read_u2(fp);  /* start_pc */
        read_u2(fp);  /* end_pc */
        read_u2(fp);  /* handler_pc */
        read_u2(fp);  /* catch_type */
    }
    
    /* Skip code attributes */
    attributes_count = read_u2(fp);
    for (i = 0; i < attributes_count; i++) {
        read_u2(fp);  /* attribute_name_index */
        length = read_u4(fp);
        fseek(fp, length, SEEK_CUR);
    }
    
    return 1;
}

/* Parse methods */
static int parse_methods(FILE* fp, JavaClass* cls) {
    uint16_t i, j;
    uint16_t attr_name_index;
    const char* attr_name;
    uint32_t attr_length;
    
    cls->methods = (JavaMethod*)calloc(cls->methods_count, sizeof(JavaMethod));
    if (!cls->methods) {
        return 0;
    }
    
    for (i = 0; i < cls->methods_count; i++) {
        cls->methods[i].access_flags = read_u2(fp);
        cls->methods[i].name_index = read_u2(fp);
        cls->methods[i].descriptor_index = read_u2(fp);
        cls->methods[i].attributes_count = read_u2(fp);
        
        /* Parse attributes */
        for (j = 0; j < cls->methods[i].attributes_count; j++) {
            attr_name_index = read_u2(fp);
            attr_name = class_get_utf8(cls, attr_name_index);
            
            if (attr_name && strcmp(attr_name, "Code") == 0) {
                if (!parse_code_attribute(fp, &cls->methods[i])) {
                    return 0;
                }
            } else {
                /* Skip other attributes */
                attr_length = read_u4(fp);
                fseek(fp, attr_length, SEEK_CUR);
            }
        }
    }
    
    return 1;
}

/* Parse fields */
static int parse_fields(FILE* fp, JavaClass* cls) {
    uint16_t i, j;
    uint32_t attr_length;
    
    cls->fields = (JavaField*)calloc(cls->fields_count, sizeof(JavaField));
    if (!cls->fields) {
        return 0;
    }
    
    for (i = 0; i < cls->fields_count; i++) {
        cls->fields[i].access_flags = read_u2(fp);
        cls->fields[i].name_index = read_u2(fp);
        cls->fields[i].descriptor_index = read_u2(fp);
        cls->fields[i].attributes_count = read_u2(fp);
        
        /* Skip field attributes */
        for (j = 0; j < cls->fields[i].attributes_count; j++) {
            read_u2(fp);  /* attribute_name_index */
            attr_length = read_u4(fp);
            fseek(fp, attr_length, SEEK_CUR);
        }
    }
    
    return 1;
}

/**
 * Parse a Java .class file
 */
JavaClass* class_parse(const char* filename) {
    FILE* fp;
    JavaClass* cls;
    uint16_t i;
    uint32_t attr_length;
    
    fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        return NULL;
    }
    
    cls = (JavaClass*)calloc(1, sizeof(JavaClass));
    if (!cls) {
        fclose(fp);
        return NULL;
    }
    
    /* Read header */
    cls->magic = read_u4(fp);
    if (cls->magic != JAVA_MAGIC) {
        fprintf(stderr, "Invalid magic number: 0x%08lX\n", (unsigned long)cls->magic);
        free(cls);
        fclose(fp);
        return NULL;
    }
    
    cls->minor_version = read_u2(fp);
    cls->major_version = read_u2(fp);
    cls->constant_pool_count = read_u2(fp);
    
    /* Parse constant pool */
    if (!parse_constant_pool(fp, cls)) {
        class_free(cls);
        fclose(fp);
        return NULL;
    }
    
    /* Read class info */
    cls->access_flags = read_u2(fp);
    cls->this_class = read_u2(fp);
    cls->super_class = read_u2(fp);
    
    /* Skip interfaces */
    cls->interfaces_count = read_u2(fp);
    if (cls->interfaces_count > 0) {
        cls->interfaces = (uint16_t*)malloc(cls->interfaces_count * sizeof(uint16_t));
        for (i = 0; i < cls->interfaces_count; i++) {
            cls->interfaces[i] = read_u2(fp);
        }
    }
    
    /* Parse fields */
    cls->fields_count = read_u2(fp);
    if (cls->fields_count > 0) {
        if (!parse_fields(fp, cls)) {
            class_free(cls);
            fclose(fp);
            return NULL;
        }
    }
    
    /* Parse methods */
    cls->methods_count = read_u2(fp);
    if (cls->methods_count > 0) {
        if (!parse_methods(fp, cls)) {
            class_free(cls);
            fclose(fp);
            return NULL;
        }
    }
    
    /* Skip class attributes */
    cls->attributes_count = read_u2(fp);
    for (i = 0; i < cls->attributes_count; i++) {
        read_u2(fp);  /* attribute_name_index */
        attr_length = read_u4(fp);
        fseek(fp, attr_length, SEEK_CUR);
    }
    
    fclose(fp);
    return cls;
}

/**
 * Free a JavaClass structure
 */
void class_free(JavaClass* cls) {
    uint16_t i;
    
    if (!cls) {
        return;
    }
    
    /* Free constant pool */
    if (cls->constant_pool) {
        for (i = 1; i < cls->constant_pool_count; i++) {
            if (cls->constant_pool[i].tag == CONSTANT_Utf8) {
                free(cls->constant_pool[i].info.utf8.bytes);
            }
        }
        free(cls->constant_pool);
    }
    
    /* Free methods */
    if (cls->methods) {
        for (i = 0; i < cls->methods_count; i++) {
            free(cls->methods[i].code);
        }
        free(cls->methods);
    }
    
    /* Free fields */
    free(cls->fields);
    
    /* Free interfaces */
    free(cls->interfaces);
    
    free(cls);
}

/**
 * Get UTF8 string from constant pool
 */
const char* class_get_utf8(JavaClass* cls, uint16_t index) {
    if (!cls || index == 0 || index >= cls->constant_pool_count) {
        return NULL;
    }
    
    if (cls->constant_pool[index].tag != CONSTANT_Utf8) {
        return NULL;
    }
    
    return cls->constant_pool[index].info.utf8.bytes;
}

/**
 * Get class name from constant pool
 */
const char* class_get_classname(JavaClass* cls, uint16_t index) {
    uint16_t name_index;
    
    if (!cls || index == 0 || index >= cls->constant_pool_count) {
        return NULL;
    }
    
    if (cls->constant_pool[index].tag != CONSTANT_Class) {
        return NULL;
    }
    
    name_index = cls->constant_pool[index].info.class_info.class_index;
    return class_get_utf8(cls, name_index);
}

/**
 * Find method by name
 */
JavaMethod* class_find_method(JavaClass* cls, const char* name) {
    uint16_t i;
    const char* method_name;
    
    if (!cls || !name) {
        return NULL;
    }
    
    for (i = 0; i < cls->methods_count; i++) {
        method_name = class_get_utf8(cls, cls->methods[i].name_index);
        if (method_name && strcmp(method_name, name) == 0) {
            return &cls->methods[i];
        }
    }
    
    return NULL;
}

// Made with Bob
