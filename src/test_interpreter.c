/**
 * Interpreter Test Program
 * 
 * Tests the bytecode interpreter with .djc files
 */

#include "vm/memory.h"
#include "vm/interpreter.h"
#include "format/djc.h"
#include "runtime/system.h"
#include <stdio.h>
#include <stdlib.h>

void test_djc_file(const char* filename) {
    DJCFile* djc_file;
    DJCMethod* main_method;
    ExecutionContext ctx;
    int result;
    
    printf("\n=== Testing: %s ===\n", filename);
    
    /* Load .djc file */
    printf("Loading file...\n");
    djc_file = djc_open(filename);
    if (djc_file == NULL) {
        printf("ERROR: Failed to load file\n");
        return;
    }
    
    printf("File loaded successfully\n");
    printf("  Constants: %u\n", djc_file->header.constant_pool_count);
    printf("  Methods: %u\n", djc_file->header.method_count);
    printf("  Code size: %u bytes\n", djc_file->header.code_size);
    
    /* Find main method */
    printf("\nFinding main method...\n");
    main_method = djc_find_method(djc_file, "main");
    if (main_method == NULL) {
        printf("ERROR: main method not found\n");
        djc_close(djc_file);
        return;
    }
    
    printf("Found main method\n");
    printf("  Code offset: %u\n", main_method->code_offset);
    printf("  Code length: %u\n", main_method->code_length);
    printf("  Max stack: %u\n", main_method->max_stack);
    printf("  Max locals: %u\n", main_method->max_locals);
    
    /* Initialize execution context */
    printf("\nInitializing interpreter...\n");
    if (interpreter_init_context(&ctx, djc_file, main_method) != 0) {
        printf("ERROR: Failed to initialize context\n");
        djc_close(djc_file);
        return;
    }
    
    printf("Interpreter initialized\n");
    
    /* Execute */
    printf("\n--- Execution Output ---\n");
    result = interpreter_execute(&ctx);
    printf("\n--- End of Output ---\n");
    
    if (result < 0) {
        printf("\nERROR: Execution failed\n");
    } else {
        printf("\nExecution completed successfully\n");
    }
    
    /* Print final state */
    printf("\nFinal state:\n");
    interpreter_print_state(&ctx);
    
    /* Cleanup */
    interpreter_free_context(&ctx);
    djc_close(djc_file);
    
    printf("\n=== Test Complete ===\n");
}

int main(int argc, char* argv[]) {
    printf("=== DOS Java Interpreter Test ===\n");
    
    /* Initialize memory manager */
    if (memory_init(0) != 0) {
        printf("ERROR: Failed to initialize memory manager\n");
        return 1;
    }
    
    printf("Memory manager initialized\n");
    memory_stats();
    
    /* Initialize system */
    if (system_init() != 0) {
        printf("ERROR: Failed to initialize system\n");
        return 1;
    }
    
    /* Test files */
    if (argc > 1) {
        /* Test specified file */
        test_djc_file(argv[1]);
    } else {
        /* Test default files */
        printf("\nUsage: %s <file.djc>\n", argv[0]);
        printf("\nTry:\n");
        printf("  %s test_arithmetic.djc\n", argv[0]);
        printf("  %s test_loop.djc\n", argv[0]);
    }
    
    /* Shutdown */
    printf("\n");
    memory_stats();
    system_shutdown();
    memory_shutdown();
    
    printf("\n=== All Tests Complete ===\n");
    
    return 0;
}

// Made with Bob
