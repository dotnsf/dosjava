#include "djc.h"
#include "../vm/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Read 16-bit value from file (little-endian)
 */
static uint16_t read_uint16(FILE* fp) {
    uint8_t bytes[2];
    if (fread(bytes, 1, 2, fp) != 2) {
        return 0;
    }
    return (uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8);
}

/**
 * Read 8-bit value from file
 */
static uint8_t read_uint8(FILE* fp) {
    uint8_t value;
    if (fread(&value, 1, 1, fp) != 1) {
        return 0;
    }
    return value;
}

/**
 * Read header from .djc file
 */
static int read_header(FILE* fp, DJCHeader* header) {
    header->magic = read_uint16(fp);
    header->version = read_uint16(fp);
    header->constant_pool_count = read_uint16(fp);
    header->method_count = read_uint16(fp);
    header->field_count = read_uint16(fp);
    header->code_size = read_uint16(fp);
    
    /* Verify magic number */
    if (header->magic != DJC_MAGIC) {
        return -1;
    }
    
    /* Verify version */
    if (header->version > DJC_VERSION) {
        return -1;
    }
    
    return 0;
}

/**
 * Read constant pool from file
 */
static int read_constants(FILE* fp, DJCFile* file) {
    uint16_t i;
    DJCConstant* constant;
    
    if (file->header.constant_pool_count == 0) {
        file->constants = NULL;
        return 0;
    }
    
    /* Allocate constant pool */
    file->constants = (DJCConstant*)memory_alloc(
        sizeof(DJCConstant) * file->header.constant_pool_count);
    if (file->constants == NULL) {
        return -1;
    }
    
    /* Read each constant */
    for (i = 0; i < file->header.constant_pool_count; i++) {
        constant = &file->constants[i];
        
        /* Read tag */
        constant->tag = read_uint8(fp);
        
        /* Read length */
        constant->length = read_uint16(fp);
        
        /* Read data based on type */
        switch (constant->tag) {
            case CONST_UTF8:
                /* Allocate and read string data */
                constant->data.utf8_data = (char*)memory_alloc(constant->length + 1);
                if (constant->data.utf8_data == NULL) {
                    return -1;
                }
                if (fread(constant->data.utf8_data, 1, constant->length, fp) != constant->length) {
                    return -1;
                }
                constant->data.utf8_data[constant->length] = '\0';
                break;
                
            case CONST_INTEGER:
                /* Read integer value */
                constant->data.int_value = (int16_t)read_uint16(fp);
                break;
                
            case CONST_STRING_REF:
            case CONST_CLASS_REF:
            case CONST_METHOD_REF:
            case CONST_FIELD_REF:
                /* Read reference index */
                constant->data.ref_index = read_uint16(fp);
                break;
                
            default:
                /* Unknown constant type */
                return -1;
        }
    }
    
    return 0;
}

/**
 * Read methods from file
 */
static int read_methods(FILE* fp, DJCFile* file) {
    uint16_t i;
    DJCMethod* method;
    
    if (file->header.method_count == 0) {
        file->methods = NULL;
        return 0;
    }
    
    /* Allocate method array */
    file->methods = (DJCMethod*)memory_alloc(
        sizeof(DJCMethod) * file->header.method_count);
    if (file->methods == NULL) {
        return -1;
    }
    
    /* Read each method */
    for (i = 0; i < file->header.method_count; i++) {
        method = &file->methods[i];
        
        method->name_index = read_uint16(fp);
        method->descriptor_index = read_uint16(fp);
        method->code_offset = read_uint16(fp);
        method->code_length = read_uint16(fp);
        method->max_stack = read_uint8(fp);
        method->max_locals = read_uint8(fp);
        method->flags = read_uint8(fp);
    }
    
    return 0;
}

/**
 * Read fields from file
 */
static int read_fields(FILE* fp, DJCFile* file) {
    uint16_t i;
    DJCField* field;
    
    if (file->header.field_count == 0) {
        file->fields = NULL;
        return 0;
    }
    
    /* Allocate field array */
    file->fields = (DJCField*)memory_alloc(
        sizeof(DJCField) * file->header.field_count);
    if (file->fields == NULL) {
        return -1;
    }
    
    /* Read each field */
    for (i = 0; i < file->header.field_count; i++) {
        field = &file->fields[i];
        
        field->name_index = read_uint16(fp);
        field->descriptor_index = read_uint16(fp);
        field->flags = read_uint8(fp);
    }
    
    return 0;
}

/**
 * Read bytecode section from file
 */
static int read_bytecode(FILE* fp, DJCFile* file) {
    if (file->header.code_size == 0) {
        file->bytecode = NULL;
        return 0;
    }
    
    /* Allocate bytecode buffer */
    file->bytecode = (uint8_t*)memory_alloc(file->header.code_size);
    if (file->bytecode == NULL) {
        return -1;
    }
    
    /* Read bytecode */
    if (fread(file->bytecode, 1, file->header.code_size, fp) != file->header.code_size) {
        return -1;
    }
    
    return 0;
}

/**
 * Open and read a .djc file
 */
DJCFile* djc_open(const char* filename) {
    FILE* fp;
    DJCFile* file;
    
    if (filename == NULL) {
        return NULL;
    }
    
    /* Open file */
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return NULL;
    }
    
    /* Allocate DJCFile structure */
    file = (DJCFile*)memory_alloc(sizeof(DJCFile));
    if (file == NULL) {
        fclose(fp);
        return NULL;
    }
    
    /* Initialize structure */
    memset(file, 0, sizeof(DJCFile));
    
    /* Read header */
    if (read_header(fp, &file->header) != 0) {
        fclose(fp);
        memory_free(file);
        return NULL;
    }
    
    /* Read constant pool */
    if (read_constants(fp, file) != 0) {
        fclose(fp);
        djc_close(file);
        return NULL;
    }
    
    /* Read methods */
    if (read_methods(fp, file) != 0) {
        fclose(fp);
        djc_close(file);
        return NULL;
    }
    
    /* Read fields */
    if (read_fields(fp, file) != 0) {
        fclose(fp);
        djc_close(file);
        return NULL;
    }
    
    /* Read bytecode */
    if (read_bytecode(fp, file) != 0) {
        fclose(fp);
        djc_close(file);
        return NULL;
    }
    
    fclose(fp);
    return file;
}

/**
 * Close and free a DJC file
 */
void djc_close(DJCFile* file) {
    uint16_t i;
    
    if (file == NULL) {
        return;
    }
    
    /* Free constant pool */
    if (file->constants != NULL) {
        for (i = 0; i < file->header.constant_pool_count; i++) {
            if (file->constants[i].tag == CONST_UTF8 &&
                file->constants[i].data.utf8_data != NULL) {
                memory_free(file->constants[i].data.utf8_data);
            }
        }
        memory_free(file->constants);
    }
    
    /* Free methods */
    if (file->methods != NULL) {
        memory_free(file->methods);
    }
    
    /* Free fields */
    if (file->fields != NULL) {
        memory_free(file->fields);
    }
    
    /* Free bytecode */
    if (file->bytecode != NULL) {
        memory_free(file->bytecode);
    }
    
    /* Free file structure */
    memory_free(file);
}

/**
 * Get a constant from the constant pool
 */
DJCConstant* djc_get_constant(DJCFile* file, uint16_t index) {
    if (file == NULL || index >= file->header.constant_pool_count) {
        return NULL;
    }
    
    return &file->constants[index];
}

/**
 * Get a UTF8 string from the constant pool
 */
const char* djc_get_utf8(DJCFile* file, uint16_t index) {
    DJCConstant* constant;
    
    constant = djc_get_constant(file, index);
    if (constant == NULL || constant->tag != CONST_UTF8) {
        return NULL;
    }
    
    return constant->data.utf8_data;
}

/**
 * Find a method by name
 */
DJCMethod* djc_find_method(DJCFile* file, const char* name) {
    uint16_t i;
    const char* method_name;
    
    if (file == NULL || name == NULL) {
        return NULL;
    }
    
    for (i = 0; i < file->header.method_count; i++) {
        method_name = djc_get_utf8(file, file->methods[i].name_index);
        if (method_name != NULL && strcmp(method_name, name) == 0) {
            return &file->methods[i];
        }
    }
    
    return NULL;
}

/**
 * Get bytecode for a method
 */
uint8_t* djc_get_method_code(DJCFile* file, DJCMethod* method) {
    if (file == NULL || method == NULL || file->bytecode == NULL) {
        return NULL;
    }
    
    if (method->code_offset >= file->header.code_size) {
        return NULL;
    }
    
    return &file->bytecode[method->code_offset];
}

// Made with Bob
