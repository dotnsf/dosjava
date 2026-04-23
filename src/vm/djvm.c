/*
 * djvm.c - DOS Java Virtual Machine
 * 
 * Main program for executing .djc bytecode files
 */

#include "interpreter.h"
#include "memory.h"
#include "../format/djc.h"
#include "../runtime/system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Program version */
#define DJVM_VERSION "1.0"

/* Command-line options */
typedef struct {
    const char* filename;       /* .djc file to execute */
    const char* method_name;    /* Method to execute (default: "main") */
    int verbose;                /* Verbose output */
    int debug;                  /* Debug mode (trace execution) */
} VMOptions;

/**
 * Print usage information
 */
void print_usage(const char* program_name) {
    printf("DOS Java Virtual Machine v%s\n", DJVM_VERSION);
    printf("\n");
    printf("Usage: %s [options] <file.djc> [method]\n", program_name);
    printf("\n");
    printf("Options:\n");
    printf("  -v, --verbose    Verbose output\n");
    printf("  -d, --debug      Debug mode (trace execution)\n");
    printf("  -h, --help       Show this help message\n");
    printf("  --version        Show version information\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  file.djc         Bytecode file to execute\n");
    printf("  method           Method name to execute (default: main)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s hello.djc           Execute main method\n", program_name);
    printf("  %s hello.djc main      Execute specific method\n", program_name);
    printf("  %s -v hello.djc        Verbose execution\n", program_name);
    printf("  %s -d hello.djc        Debug mode\n", program_name);
}

/**
 * Print version information
 */
void print_version(void) {
    printf("DOS Java Virtual Machine v%s\n", DJVM_VERSION);
    printf("16-bit DOS Bytecode Interpreter\n");
    printf("Built with Open Watcom v2\n");
}

/**
 * Parse command-line arguments
 */
int parse_arguments(int argc, char* argv[], VMOptions* options) {
    int i;
    int has_file = 0;
    
    /* Initialize options */
    memset(options, 0, sizeof(VMOptions));
    options->method_name = "main";
    
    /* Need at least program name and file */
    if (argc < 2) {
        print_usage(argv[0]);
        return -1;
    }
    
    /* Parse arguments */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            /* Option */
            if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
                options->verbose = 1;
            }
            else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
                options->debug = 1;
            }
            else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                print_usage(argv[0]);
                return -1;
            }
            else if (strcmp(argv[i], "--version") == 0) {
                print_version();
                return -1;
            }
            else {
                fprintf(stderr, "ERROR: Unknown option: %s\n", argv[i]);
                return -1;
            }
        }
        else {
            /* Positional argument */
            if (!has_file) {
                options->filename = argv[i];
                has_file = 1;
            }
            else {
                options->method_name = argv[i];
            }
        }
    }
    
    /* Check if file was provided */
    if (!has_file) {
        fprintf(stderr, "ERROR: No input file specified\n");
        print_usage(argv[0]);
        return -1;
    }
    
    return 0;
}

/**
 * Execute a method
 */
int execute_method(DJCFile* djc_file, DJCMethod* method, VMOptions* options) {
    ExecutionContext ctx;
    int result;
    
    /* Always print method info in debug mode */
    if (options->verbose || options->debug) {
        uint8_t* code;
        uint16_t i;
        
        printf("Executing method: %s\n", djc_get_utf8(djc_file, method->name_index));
        printf("  Code offset: %u\n", method->code_offset);
        printf("  Code length: %u\n", method->code_length);
        printf("  Max stack:   %u\n", method->max_stack);
        printf("  Max locals:  %u\n", method->max_locals);
        
        /* Dump bytecode in hex */
        code = djc_get_method_code(djc_file, method);
        if (code && method->code_length > 0) {
            printf("  Bytecode: ");
            for (i = 0; i < method->code_length; i++) {
                printf("%02X ", code[i]);
            }
            printf("\n");
        }
        printf("\n");
    }
    
    /* Initialize execution context */
    result = interpreter_init_context(&ctx, djc_file, method);
    if (result != 0) {
        fprintf(stderr, "ERROR: Failed to initialize execution context\n");
        return -1;
    }
    
    /* Execute method */
    if (options->debug) {
        /* Debug mode: step through instructions */
        printf("=== Debug Mode ===\n");
        while (ctx.running) {
            uint16_t pc_offset = (uint16_t)(ctx.pc - ctx.code_start);
            uint8_t opcode = *ctx.pc;
            
            printf("[PC=%04X] Opcode=0x%02X\n", pc_offset, opcode);
            
            result = interpreter_step(&ctx);
            if (result < 0) {
                fprintf(stderr, "ERROR: Execution failed at PC=%04X\n", pc_offset);
                interpreter_free_context(&ctx);
                return -1;
            }
            else if (result > 0) {
                /* Method returned */
                break;
            }
            
            if (options->verbose) {
                interpreter_print_state(&ctx);
                printf("\n");
            }
        }
    }
    else {
        /* Normal execution */
        result = interpreter_execute(&ctx);
        if (result < 0) {
            fprintf(stderr, "ERROR: Execution failed\n");
            interpreter_free_context(&ctx);
            return -1;
        }
    }
    
    if (options->verbose || options->debug) {
        printf("\nExecution completed successfully\n");
    }
    
    /* Cleanup */
    interpreter_free_context(&ctx);
    
    return 0;
}

/**
 * Main function
 */
int main(int argc, char* argv[]) {
    VMOptions options;
    DJCFile* djc_file;
    DJCMethod* method;
    int result;
    
    /* Parse command-line arguments */
    result = parse_arguments(argc, argv, &options);
    if (result != 0) {
        return (result < 0) ? 1 : 0;
    }
    
    /* Initialize memory manager */
    if (memory_init(0) != 0) {
        fprintf(stderr, "ERROR: Failed to initialize memory manager\n");
        return 1;
    }
    
    /* Initialize system */
    if (system_init() != 0) {
        fprintf(stderr, "ERROR: Failed to initialize system\n");
        memory_shutdown();
        return 1;
    }
    
    if (options.verbose) {
        printf("DOS Java Virtual Machine v%s\n", DJVM_VERSION);
        printf("Loading: %s\n", options.filename);
        printf("\n");
    }
    
    /* Open DJC file */
    djc_file = djc_open(options.filename);
    if (djc_file == NULL) {
        fprintf(stderr, "ERROR: Failed to open file: %s\n", options.filename);
        system_shutdown();
        memory_shutdown();
        return 1;
    }
    
    if (options.verbose) {
        printf("DJC File Information:\n");
        printf("  Magic:     0x%04X\n", djc_file->header.magic);
        printf("  Version:   0x%04X\n", djc_file->header.version);
        printf("  Constants: %u\n", djc_file->header.constant_pool_count);
        printf("  Methods:   %u\n", djc_file->header.method_count);
        printf("  Fields:    %u\n", djc_file->header.field_count);
        printf("  Code size: %u bytes\n", djc_file->header.code_size);
        printf("\n");
    }
    
    /* Find method by name */
    method = djc_find_method_by_name(djc_file, options.method_name);
    if (method == NULL) {
        fprintf(stderr, "ERROR: Method not found: %s\n", options.method_name);
        djc_close(djc_file);
        system_shutdown();
        memory_shutdown();
        return 1;
    }
    
    /* Execute method */
    result = execute_method(djc_file, method, &options);
    
    /* Cleanup */
    djc_close(djc_file);
    system_shutdown();
    memory_shutdown();
    
    return (result == 0) ? 0 : 1;
}

// Made with Bob
