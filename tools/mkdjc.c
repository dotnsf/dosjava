/**
 * Simple .djc file generator
 * 
 * Creates a minimal .djc file for testing the interpreter
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Include types from parent directory */
#include "../src/types.h"
#include "../src/format/djc.h"
#include "../src/format/opcodes.h"

/**
 * Write 16-bit value (little-endian)
 */
static void write_u16(FILE* fp, uint16_t value) {
    uint8_t bytes[2];
    bytes[0] = (uint8_t)(value & 0xFF);
    bytes[1] = (uint8_t)((value >> 8) & 0xFF);
    fwrite(bytes, 1, 2, fp);
}

/**
 * Write 8-bit value
 */
static void write_u8(FILE* fp, uint8_t value) {
    fwrite(&value, 1, 1, fp);
}

/**
 * Create a simple test program:
 * 
 * int main() {
 *     int a = 10;
 *     int b = 20;
 *     int c = a + b;
 *     print(c);  // Should print 30
 *     return;
 * }
 */
void create_arithmetic_test(const char* filename) {
    FILE* fp;
    
    printf("Creating arithmetic test: %s\n", filename);
    
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("ERROR: Cannot create file\n");
        return;
    }
    
    /* Write header */
    write_u16(fp, DJC_MAGIC);           /* Magic: 0x444A */
    write_u16(fp, DJC_VERSION);         /* Version */
    write_u16(fp, 1);                   /* Constant pool count */
    write_u16(fp, 1);                   /* Method count */
    write_u16(fp, 0);                   /* Field count */
    write_u16(fp, 20);                  /* Code size */
    
    /* Write constant pool */
    /* Constant 0: "main" (UTF8) */
    write_u8(fp, CONST_UTF8);
    write_u16(fp, 4);                   /* Length */
    fwrite("main", 1, 4, fp);
    
    /* Write method descriptor */
    write_u16(fp, 0);                   /* name_index: "main" */
    write_u16(fp, 0);                   /* descriptor_index */
    write_u16(fp, 0);                   /* code_offset */
    write_u16(fp, 20);                  /* code_length */
    write_u8(fp, 10);                   /* max_stack */
    write_u8(fp, 3);                    /* max_locals (a, b, c) */
    write_u8(fp, METHOD_STATIC);        /* flags */
    
    /* Write bytecode */
    /* a = 10 */
    write_u8(fp, OP_PUSH_INT);
    write_u16(fp, 10);
    write_u8(fp, OP_STORE_0);
    
    /* b = 20 */
    write_u8(fp, OP_PUSH_INT);
    write_u16(fp, 20);
    write_u8(fp, OP_STORE_1);
    
    /* c = a + b */
    write_u8(fp, OP_LOAD_0);
    write_u8(fp, OP_LOAD_1);
    write_u8(fp, OP_ADD);
    write_u8(fp, OP_STORE_2);
    
    /* print(c) */
    write_u8(fp, OP_LOAD_2);
    write_u8(fp, OP_PRINT_INT);
    
    /* return */
    write_u8(fp, OP_RETURN);
    
    fclose(fp);
    printf("Created successfully!\n");
    printf("Expected output: 30\n");
}

/**
 * Create a loop test program:
 * 
 * int main() {
 *     int i = 0;
 *     while (i < 5) {
 *         print(i);
 *         i = i + 1;
 *     }
 *     return;
 * }
 */
void create_loop_test(const char* filename) {
    FILE* fp;
    
    printf("Creating loop test: %s\n", filename);
    
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("ERROR: Cannot create file\n");
        return;
    }
    
    /* Write header */
    write_u16(fp, DJC_MAGIC);
    write_u16(fp, DJC_VERSION);
    write_u16(fp, 1);                   /* Constant pool count */
    write_u16(fp, 1);                   /* Method count */
    write_u16(fp, 0);                   /* Field count */
    write_u16(fp, 25);                  /* Code size */
    
    /* Write constant pool */
    write_u8(fp, CONST_UTF8);
    write_u16(fp, 4);
    fwrite("main", 1, 4, fp);
    
    /* Write method descriptor */
    write_u16(fp, 0);
    write_u16(fp, 0);
    write_u16(fp, 0);
    write_u16(fp, 25);
    write_u8(fp, 10);
    write_u8(fp, 1);                    /* max_locals (i) */
    write_u8(fp, METHOD_STATIC);
    
    /* Write bytecode */
    /* i = 0 */
    write_u8(fp, OP_PUSH_INT);
    write_u16(fp, 0);
    write_u8(fp, OP_STORE_0);
    
    /* loop_start: (offset 4) */
    /* if (i >= 5) goto loop_end */
    write_u8(fp, OP_LOAD_0);
    write_u8(fp, OP_PUSH_INT);
    write_u16(fp, 5);
    write_u8(fp, OP_CMP_GE);
    write_u8(fp, OP_IF_TRUE);
    write_u16(fp, 24);                  /* Jump to offset 24 (return) */
    
    /* print(i) */
    write_u8(fp, OP_LOAD_0);
    write_u8(fp, OP_PRINT_INT);
    
    /* i = i + 1 */
    write_u8(fp, OP_LOAD_0);
    write_u8(fp, OP_PUSH_INT);
    write_u16(fp, 1);
    write_u8(fp, OP_ADD);
    write_u8(fp, OP_STORE_0);
    
    /* goto loop_start */
    write_u8(fp, OP_GOTO);
    write_u16(fp, 4);
    
    /* loop_end: (offset 24) */
    write_u8(fp, OP_RETURN);
    
    fclose(fp);
    printf("Created successfully!\n");
    printf("Expected output: 01234\n");
}

int main(int argc, char* argv[]) {
    printf("=== DOS Java .djc File Generator ===\n\n");
    
    if (argc < 2) {
        printf("Usage: %s <test_name>\n", argv[0]);
        printf("\nAvailable tests:\n");
        printf("  arithmetic - Simple arithmetic test (10 + 20 = 30)\n");
        printf("  loop       - Loop test (print 0-4)\n");
        printf("  all        - Create all tests\n");
        return 1;
    }
    
    if (strcmp(argv[1], "arithmetic") == 0 || strcmp(argv[1], "all") == 0) {
        create_arithmetic_test("test_arithmetic.djc");
        printf("\n");
    }
    
    if (strcmp(argv[1], "loop") == 0 || strcmp(argv[1], "all") == 0) {
        create_loop_test("test_loop.djc");
        printf("\n");
    }
    
    printf("Done!\n");
    return 0;
}

// Made with Bob
