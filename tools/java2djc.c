/**
 * java2djc - Java .class to .djc converter
 * 
 * Converts standard Java bytecode to DOS Java bytecode format
 */

#include "classfile.h"
#include "../src/format/djc.h"
#include "../src/format/opcodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Java bytecode opcodes (subset we support) */
#define JAVA_NOP            0x00
#define JAVA_ICONST_M1      0x02
#define JAVA_ICONST_0       0x03
#define JAVA_ICONST_1       0x04
#define JAVA_ICONST_2       0x05
#define JAVA_ICONST_3       0x06
#define JAVA_ICONST_4       0x07
#define JAVA_ICONST_5       0x08
#define JAVA_BIPUSH         0x10
#define JAVA_SIPUSH         0x11
#define JAVA_LDC            0x12
#define JAVA_ILOAD          0x15
#define JAVA_ILOAD_0        0x1A
#define JAVA_ILOAD_1        0x1B
#define JAVA_ILOAD_2        0x1C
#define JAVA_ILOAD_3        0x1D
#define JAVA_ISTORE         0x36
#define JAVA_ISTORE_0       0x3B
#define JAVA_ISTORE_1       0x3C
#define JAVA_ISTORE_2       0x3D
#define JAVA_ISTORE_3       0x3E
#define JAVA_POP            0x57
#define JAVA_DUP            0x59
#define JAVA_IADD           0x60
#define JAVA_ISUB           0x64
#define JAVA_IMUL           0x68
#define JAVA_IDIV           0x6C
#define JAVA_IREM           0x70
#define JAVA_INEG           0x74
#define JAVA_IINC           0x84
#define JAVA_IFEQ           0x99
#define JAVA_IFNE           0x9A
#define JAVA_IFLT           0x9B
#define JAVA_IFGE           0x9C
#define JAVA_IFGT           0x9D
#define JAVA_IFLE           0x9E
#define JAVA_IF_ICMPEQ      0x9F
#define JAVA_IF_ICMPNE      0xA0
#define JAVA_IF_ICMPLT      0xA1
#define JAVA_IF_ICMPGE      0xA2
#define JAVA_IF_ICMPGT      0xA3
#define JAVA_IF_ICMPLE      0xA4
#define JAVA_GOTO           0xA7
#define JAVA_IRETURN        0xAC
#define JAVA_RETURN         0xB1
#define JAVA_GETSTATIC      0xB2
#define JAVA_INVOKEVIRTUAL  0xB6
#define JAVA_INVOKESTATIC   0xB8

/* Output buffer for converted bytecode */
typedef struct {
    uint8_t* data;
    uint32_t size;
    uint32_t capacity;
} ByteBuffer;

/* Initialize byte buffer */
static ByteBuffer* buffer_new(void) {
    ByteBuffer* buf = (ByteBuffer*)malloc(sizeof(ByteBuffer));
    if (!buf) return NULL;
    
    buf->capacity = 1024;
    buf->data = (uint8_t*)malloc(buf->capacity);
    if (!buf->data) {
        free(buf);
        return NULL;
    }
    buf->size = 0;
    return buf;
}

/* Write byte to buffer */
static void buffer_write_u1(ByteBuffer* buf, uint8_t value) {
    if (buf->size >= buf->capacity) {
        buf->capacity *= 2;
        buf->data = (uint8_t*)realloc(buf->data, buf->capacity);
    }
    buf->data[buf->size++] = value;
}

/* Write uint16 to buffer */
static void buffer_write_u2(ByteBuffer* buf, uint16_t value) {
    buffer_write_u1(buf, (uint8_t)(value >> 8));
    buffer_write_u1(buf, (uint8_t)(value & 0xFF));
}

/* Free buffer */
static void buffer_free(ByteBuffer* buf) {
    if (buf) {
        free(buf->data);
        free(buf);
    }
}

/**
 * Convert Java bytecode to .djc bytecode
 */
static ByteBuffer* convert_bytecode(JavaClass* cls, JavaMethod* method) {
    ByteBuffer* out;
    uint32_t pc;
    uint8_t* code;
    uint32_t code_length;
    uint8_t opcode;
    
    out = buffer_new();
    if (!out) return NULL;
    
    pc = 0;
    code = method->code;
    code_length = method->code_length;
    
    while (pc < code_length) {
        opcode = code[pc];
        
        switch (opcode) {
            case JAVA_NOP:
                buffer_write_u1(out, OP_NOP);
                pc++;
                break;
            
            case JAVA_ICONST_M1:
                buffer_write_u1(out, OP_PUSH_INT);
                buffer_write_u2(out, (uint16_t)-1);
                pc++;
                break;
            
            case JAVA_ICONST_0:
            case JAVA_ICONST_1:
            case JAVA_ICONST_2:
            case JAVA_ICONST_3:
            case JAVA_ICONST_4:
            case JAVA_ICONST_5:
                buffer_write_u1(out, OP_PUSH_INT);
                buffer_write_u2(out, opcode - JAVA_ICONST_0);
                pc++;
                break;
            
            case JAVA_BIPUSH:
                buffer_write_u1(out, OP_PUSH_INT);
                buffer_write_u2(out, (int16_t)(int8_t)code[pc + 1]);
                pc += 2;
                break;
            
            case JAVA_SIPUSH:
                buffer_write_u1(out, OP_PUSH_INT);
                buffer_write_u2(out, (code[pc + 1] << 8) | code[pc + 2]);
                pc += 3;
                break;
            
            case JAVA_ILOAD:
                buffer_write_u1(out, OP_LOAD_LOCAL);
                buffer_write_u1(out, code[pc + 1]);
                pc += 2;
                break;
            
            case JAVA_ILOAD_0:
                buffer_write_u1(out, OP_LOAD_0);
                pc++;
                break;
            
            case JAVA_ILOAD_1:
                buffer_write_u1(out, OP_LOAD_1);
                pc++;
                break;
            
            case JAVA_ILOAD_2:
                buffer_write_u1(out, OP_LOAD_2);
                pc++;
                break;
            
            case JAVA_ILOAD_3:
                buffer_write_u1(out, OP_LOAD_LOCAL);
                buffer_write_u1(out, 3);
                pc++;
                break;
            
            case JAVA_ISTORE:
                buffer_write_u1(out, OP_STORE_LOCAL);
                buffer_write_u1(out, code[pc + 1]);
                pc += 2;
                break;
            
            case JAVA_ISTORE_0:
                buffer_write_u1(out, OP_STORE_0);
                pc++;
                break;
            
            case JAVA_ISTORE_1:
                buffer_write_u1(out, OP_STORE_1);
                pc++;
                break;
            
            case JAVA_ISTORE_2:
                buffer_write_u1(out, OP_STORE_2);
                pc++;
                break;
            
            case JAVA_ISTORE_3:
                buffer_write_u1(out, OP_STORE_LOCAL);
                buffer_write_u1(out, 3);
                pc++;
                break;
            
            case JAVA_POP:
                buffer_write_u1(out, OP_POP);
                pc++;
                break;
            
            case JAVA_DUP:
                buffer_write_u1(out, OP_DUP);
                pc++;
                break;
            
            case JAVA_IADD:
                buffer_write_u1(out, OP_ADD);
                pc++;
                break;
            
            case JAVA_ISUB:
                buffer_write_u1(out, OP_SUB);
                pc++;
                break;
            
            case JAVA_IMUL:
                buffer_write_u1(out, OP_MUL);
                pc++;
                break;
            
            case JAVA_IDIV:
                buffer_write_u1(out, OP_DIV);
                pc++;
                break;
            
            case JAVA_IREM:
                buffer_write_u1(out, OP_MOD);
                pc++;
                break;
            
            case JAVA_INEG:
                buffer_write_u1(out, OP_NEG);
                pc++;
                break;
            
            case JAVA_IINC: {
                uint8_t index = code[pc + 1];
                int8_t amount = (int8_t)code[pc + 2];
                buffer_write_u1(out, OP_INC_LOCAL);
                buffer_write_u1(out, index);
                buffer_write_u1(out, (uint8_t)amount);
                pc += 3;
                break;
            }
            
            case JAVA_IFEQ:
            case JAVA_IFNE:
            case JAVA_IFLT:
            case JAVA_IFGE:
            case JAVA_IFGT:
            case JAVA_IFLE: {
                int16_t offset = (int16_t)((code[pc + 1] << 8) | code[pc + 2]);
                
                /* Convert to compare + conditional jump */
                buffer_write_u1(out, OP_PUSH_INT);
                buffer_write_u2(out, 0);
                
                switch (opcode) {
                    case JAVA_IFEQ: buffer_write_u1(out, OP_IF_EQ); break;
                    case JAVA_IFNE: buffer_write_u1(out, OP_IF_NE); break;
                    case JAVA_IFLT: buffer_write_u1(out, OP_IF_LT); break;
                    case JAVA_IFGE: buffer_write_u1(out, OP_IF_GE); break;
                    case JAVA_IFGT: buffer_write_u1(out, OP_IF_GT); break;
                    case JAVA_IFLE: buffer_write_u1(out, OP_IF_LE); break;
                }
                buffer_write_u2(out, offset);
                pc += 3;
                break;
            }
            
            case JAVA_IF_ICMPEQ:
            case JAVA_IF_ICMPNE:
            case JAVA_IF_ICMPLT:
            case JAVA_IF_ICMPGE:
            case JAVA_IF_ICMPGT:
            case JAVA_IF_ICMPLE: {
                int16_t offset = (int16_t)((code[pc + 1] << 8) | code[pc + 2]);
                
                switch (opcode) {
                    case JAVA_IF_ICMPEQ: buffer_write_u1(out, OP_IF_EQ); break;
                    case JAVA_IF_ICMPNE: buffer_write_u1(out, OP_IF_NE); break;
                    case JAVA_IF_ICMPLT: buffer_write_u1(out, OP_IF_LT); break;
                    case JAVA_IF_ICMPGE: buffer_write_u1(out, OP_IF_GE); break;
                    case JAVA_IF_ICMPGT: buffer_write_u1(out, OP_IF_GT); break;
                    case JAVA_IF_ICMPLE: buffer_write_u1(out, OP_IF_LE); break;
                }
                buffer_write_u2(out, offset);
                pc += 3;
                break;
            }
            
            case JAVA_GOTO: {
                int16_t offset = (int16_t)((code[pc + 1] << 8) | code[pc + 2]);
                buffer_write_u1(out, OP_GOTO);
                buffer_write_u2(out, offset);
                pc += 3;
                break;
            }
            
            case JAVA_IRETURN:
                buffer_write_u1(out, OP_RETURN_VALUE);
                pc++;
                break;
            
            case JAVA_RETURN:
                buffer_write_u1(out, OP_RETURN);
                pc++;
                break;
            
            default:
                fprintf(stderr, "Unsupported opcode: 0x%02X at pc=%u\n", opcode, pc);
                buffer_free(out);
                return NULL;
        }
    }
    
    return out;
}

/**
 * Write .djc file
 */
static int write_djc_file(const char* filename, JavaClass* cls) {
    FILE* fp;
    DJCHeader header;
    uint16_t i;
    uint16_t code_offset;
    uint8_t flags;
    ByteBuffer* converted;
    
    fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Cannot create file: %s\n", filename);
        return 0;
    }
    
    /* Prepare header */
    header.magic = DJC_MAGIC;
    header.version = DJC_VERSION;
    header.constant_pool_count = 0;  /* Simplified: no constant pool for now */
    header.method_count = cls->methods_count;
    header.field_count = cls->fields_count;
    header.code_size = 0;
    
    /* Calculate total code size */
    for (i = 0; i < cls->methods_count; i++) {
        converted = convert_bytecode(cls, &cls->methods[i]);
        if (converted) {
            header.code_size += converted->size;
            buffer_free(converted);
        }
    }
    
    /* Write header */
    fwrite(&header.magic, 2, 1, fp);
    fwrite(&header.version, 2, 1, fp);
    fwrite(&header.constant_pool_count, 2, 1, fp);
    fwrite(&header.method_count, 2, 1, fp);
    fwrite(&header.field_count, 2, 1, fp);
    fwrite(&header.code_size, 2, 1, fp);
    
    /* Write methods and code */
    code_offset = 0;
    for (i = 0; i < cls->methods_count; i++) {
        converted = convert_bytecode(cls, &cls->methods[i]);
        if (!converted) {
            fclose(fp);
            return 0;
        }
        
        /* Write method descriptor */
        fwrite(&cls->methods[i].name_index, 2, 1, fp);
        fwrite(&cls->methods[i].descriptor_index, 2, 1, fp);
        fwrite(&code_offset, 2, 1, fp);
        fwrite(&converted->size, 2, 1, fp);
        fwrite(&cls->methods[i].max_stack, 1, 1, fp);
        fwrite(&cls->methods[i].max_locals, 1, 1, fp);
        
        flags = 0;
        if (cls->methods[i].access_flags & ACC_STATIC) flags |= METHOD_STATIC;
        if (cls->methods[i].access_flags & ACC_PUBLIC) flags |= METHOD_PUBLIC;
        if (cls->methods[i].access_flags & ACC_PRIVATE) flags |= METHOD_PRIVATE;
        fwrite(&flags, 1, 1, fp);
        
        code_offset += converted->size;
        buffer_free(converted);
    }
    
    /* Write bytecode */
    for (i = 0; i < cls->methods_count; i++) {
        converted = convert_bytecode(cls, &cls->methods[i]);
        if (converted) {
            fwrite(converted->data, 1, converted->size, fp);
            buffer_free(converted);
        }
    }
    
    fclose(fp);
    return 1;
}

/**
 * Main program
 */
int main(int argc, char* argv[]) {
    JavaClass* cls;
    char output_filename[256];
    char* dot;
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.class> [output.djc]\n", argv[0]);
        return 1;
    }
    
    printf("Java to DJC Converter\n");
    printf("Input: %s\n", argv[1]);
    
    /* Parse .class file */
    cls = class_parse(argv[1]);
    if (!cls) {
        fprintf(stderr, "Failed to parse .class file\n");
        return 1;
    }
    
    printf("Class version: %d.%d\n", cls->major_version, cls->minor_version);
    printf("Methods: %d\n", cls->methods_count);
    printf("Fields: %d\n", cls->fields_count);
    
    /* Determine output filename */
    if (argc >= 3) {
        strcpy(output_filename, argv[2]);
    } else {
        strcpy(output_filename, argv[1]);
        dot = strrchr(output_filename, '.');
        if (dot) {
            strcpy(dot, ".djc");
        } else {
            strcat(output_filename, ".djc");
        }
    }
    
    printf("Output: %s\n", output_filename);
    
    /* Convert and write */
    if (!write_djc_file(output_filename, cls)) {
        fprintf(stderr, "Failed to write .djc file\n");
        class_free(cls);
        return 1;
    }
    
    printf("Conversion successful!\n");
    
    class_free(cls);
    return 0;
}

// Made with Bob