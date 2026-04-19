#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * DOS Java Compiler - Code Generator Test Program
 * 
 * Tests code generation from AST and symbol table to .djc bytecode.
 * Usage: test_codegen <ast_file> <symbol_file> <output_file>
 */

static void print_usage(const char* program_name) {
    printf("DOS Java Compiler - Code Generator Test\n");
    printf("Usage: %s <ast_file> <symbol_file> <output_file>\n", program_name);
    printf("\n");
    printf("Arguments:\n");
    printf("  ast_file     - Input AST file (from parser)\n");
    printf("  symbol_file  - Input symbol table file (from semantic analyzer)\n");
    printf("  output_file  - Output .djc bytecode file\n");
    printf("\n");
    printf("Example:\n");
    printf("  %s test.ast test.sym test.djc\n", program_name);
}

static void print_djc_info(CodeGenerator* codegen) {
    uint16_t i;
    
    printf("\nGenerated .djc file information:\n");
    printf("================================\n");
    printf("Constants: %u\n", codegen->constants->count);
    printf("Methods:   %u\n", codegen->method_count);
    printf("Fields:    %u\n", codegen->field_count);
    printf("Code size: %u bytes\n", codegen->bytecode->size);
    printf("\n");
    
    /* Print constant pool */
    if (codegen->constants->count > 0) {
        printf("Constant Pool:\n");
        printf("--------------\n");
        for (i = 0; i < codegen->constants->count; i++) {
            DJCConstant* c = &codegen->constants->constants[i];
            printf("[%3u] ", i);
            
            switch (c->tag) {
                case CONST_UTF8:
                    printf("UTF8: \"%s\"\n", c->data.utf8_data);
                    break;
                case CONST_INTEGER:
                    printf("INTEGER: %d\n", c->data.int_value);
                    break;
                default:
                    printf("Unknown type %u\n", c->tag);
                    break;
            }
        }
        printf("\n");
    }
    
    /* Print method table */
    if (codegen->method_count > 0) {
        printf("Method Table:\n");
        printf("-------------\n");
        for (i = 0; i < codegen->method_count; i++) {
            DJCMethod* m = &codegen->methods[i];
            const char* name = codegen->constants->constants[m->name_index].data.utf8_data;
            
            printf("[%2u] %s\n", i, name);
            printf("     Offset: %u, Length: %u\n", m->code_offset, m->code_length);
            printf("     Stack: %u, Locals: %u\n", m->max_stack, m->max_locals);
            printf("     Flags: %s%s\n",
                   (m->flags & METHOD_STATIC) ? "static " : "",
                   (m->flags & METHOD_PUBLIC) ? "public" : "private");
        }
        printf("\n");
    }
    
    /* Print field table */
    if (codegen->field_count > 0) {
        printf("Field Table:\n");
        printf("------------\n");
        for (i = 0; i < codegen->field_count; i++) {
            DJCField* f = &codegen->fields[i];
            const char* name = codegen->constants->constants[f->name_index].data.utf8_data;
            
            printf("[%2u] %s\n", i, name);
            printf("     Flags: %s%s\n",
                   (f->flags & FIELD_STATIC) ? "static " : "",
                   (f->flags & FIELD_PUBLIC) ? "public" : "private");
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    CodeGenerator* codegen;
    const char* ast_file;
    const char* symbol_file;
    const char* output_file;
    int result;
    
    /* Check arguments */
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }
    
    ast_file = argv[1];
    symbol_file = argv[2];
    output_file = argv[3];
    
    printf("DOS Java Code Generator Test\n");
    printf("============================\n");
    printf("AST file:    %s\n", ast_file);
    printf("Symbol file: %s\n", symbol_file);
    printf("Output file: %s\n", output_file);
    printf("\n");
    
    /* Allocate code generator on heap (avoid stack overflow) */
    codegen = (CodeGenerator*)malloc(sizeof(CodeGenerator));
    if (!codegen) {
        printf("Error: Failed to allocate code generator\n");
        return 1;
    }
    
    /* Initialize code generator */
    printf("Initializing code generator...\n");
    if (codegen_init(codegen, ast_file, symbol_file, output_file) != 0) {
        printf("Error: Failed to initialize code generator\n");
        printf("  - Check that AST and symbol table files exist and are valid\n");
        free(codegen);
        return 1;
    }
    
    printf("Loaded AST: %u nodes\n", codegen->total_nodes);
    printf("Loaded symbols: %u\n", codegen->symtable->symbol_count);
    printf("\n");
    
    /* Generate code */
    printf("Generating bytecode...\n");
    printf("----------------------\n");
    
    result = codegen_generate(codegen);
    
    printf("----------------------\n");
    
    /* Print results */
    if (result == 0) {
        printf("\nCode generation completed successfully!\n");
        
        /* Print .djc file information */
        print_djc_info(codegen);
        
        printf("Bytecode written to %s\n", output_file);
    } else {
        printf("\nCode generation failed!\n");
        printf("  Errors: %u\n", codegen->error_count);
    }
    
    /* Cleanup */
    codegen_cleanup(codegen);
    free(codegen);
    
    return result == 0 ? 0 : 1;
}

// Made with Bob
