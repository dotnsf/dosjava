# Phase 5: Detailed Design Document

## 1. Compiler Context Structure

### CompilerContext Definition

```c
/* tools/compiler/djc.h */

#ifndef DJC_H
#define DJC_H

#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"

/* Compiler phase identifiers */
#define PHASE_NONE      0
#define PHASE_LEXER     1
#define PHASE_PARSER    2
#define PHASE_SEMANTIC  3
#define PHASE_CODEGEN   4

/* Exit codes */
#define EXIT_SUCCESS    0
#define EXIT_ERROR      1
#define EXIT_USAGE      2

/* Maximum path length for DOS */
#define MAX_PATH_LEN    256

typedef struct {
    /* Input/Output files */
    char source_file[MAX_PATH_LEN];     /* Input .java file */
    char output_file[MAX_PATH_LEN];     /* Output .djc file */
    
    /* Intermediate files */
    char tok_file[MAX_PATH_LEN];        /* Token dump file */
    char ast_file[MAX_PATH_LEN];        /* AST dump file */
    char sym_file[MAX_PATH_LEN];        /* Symbol table dump file */
    
    /* Options */
    int keep_intermediates;             /* Keep intermediate files */
    int verbose;                        /* Verbose output */
    int dump_tokens;                    /* Dump tokens to file */
    int dump_ast;                       /* Dump AST to file */
    int dump_symbols;                   /* Dump symbol table to file */
    
    /* Phase components (heap allocated) */
    Lexer* lexer;
    Parser* parser;
    SemanticAnalyzer* semantic;
    CodeGenerator* codegen;
    
    /* Statistics */
    uint16_t line_count;
    uint16_t token_count;
    uint16_t node_count;
    uint16_t symbol_count;
    uint16_t bytecode_size;
    uint32_t compile_time_ms;           /* Compilation time in milliseconds */
    
    /* Error tracking */
    int has_error;
    int error_phase;                    /* Which phase failed */
    char error_message[256];            /* Last error message */
} CompilerContext;

/* Initialization and cleanup */
void compiler_init(CompilerContext* ctx);
void compiler_cleanup(CompilerContext* ctx);

/* Main compilation function */
int compiler_compile(CompilerContext* ctx);

/* Phase execution functions */
int run_lexer_phase(CompilerContext* ctx);
int run_parser_phase(CompilerContext* ctx);
int run_semantic_phase(CompilerContext* ctx);
int run_codegen_phase(CompilerContext* ctx);

/* Utility functions */
void generate_intermediate_filenames(CompilerContext* ctx);
void cleanup_intermediate_files(CompilerContext* ctx);
int check_file_exists(const char* filename);
void get_base_filename(const char* path, char* base, size_t base_size);

/* Reporting functions */
void print_statistics(const CompilerContext* ctx);
void report_phase_error(const CompilerContext* ctx);
void print_usage(const char* program_name);
void print_version(void);

/* Command-line parsing */
int parse_arguments(int argc, char* argv[], CompilerContext* ctx);

#endif /* DJC_H */
```

## 2. Main Compiler Driver Implementation

### 2.1 Initialization and Cleanup

```c
void compiler_init(CompilerContext* ctx) {
    /* Clear all fields */
    memset(ctx, 0, sizeof(CompilerContext));
    
    /* Set defaults */
    ctx->keep_intermediates = 0;
    ctx->verbose = 0;
    ctx->dump_tokens = 0;
    ctx->dump_ast = 0;
    ctx->dump_symbols = 0;
    ctx->has_error = 0;
    ctx->error_phase = PHASE_NONE;
    
    /* Initialize phase components to NULL */
    ctx->lexer = NULL;
    ctx->parser = NULL;
    ctx->semantic = NULL;
    ctx->codegen = NULL;
}

void compiler_cleanup(CompilerContext* ctx) {
    /* Free phase components */
    if (ctx->lexer) {
        lexer_destroy(ctx->lexer);
        ctx->lexer = NULL;
    }
    
    if (ctx->parser) {
        parser_destroy(ctx->parser);
        ctx->parser = NULL;
    }
    
    if (ctx->semantic) {
        semantic_destroy(ctx->semantic);
        ctx->semantic = NULL;
    }
    
    if (ctx->codegen) {
        codegen_destroy(ctx->codegen);
        ctx->codegen = NULL;
    }
    
    /* Remove intermediate files if not keeping them */
    if (!ctx->keep_intermediates) {
        cleanup_intermediate_files(ctx);
    }
}
```

### 2.2 Command-Line Parsing

```c
int parse_arguments(int argc, char* argv[], CompilerContext* ctx) {
    int i;
    int has_input = 0;
    
    /* Need at least program name and input file */
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_USAGE;
    }
    
    /* Parse options and input file */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            /* Option */
            if (strcmp(argv[i], "-o") == 0) {
                /* Output file */
                if (i + 1 >= argc) {
                    fprintf(stderr, "Error: -o requires an argument\n");
                    return EXIT_ERROR;
                }
                strncpy(ctx->output_file, argv[++i], MAX_PATH_LEN - 1);
                ctx->output_file[MAX_PATH_LEN - 1] = '\0';
            }
            else if (strcmp(argv[i], "-k") == 0) {
                /* Keep intermediate files */
                ctx->keep_intermediates = 1;
            }
            else if (strcmp(argv[i], "-v") == 0) {
                /* Verbose */
                ctx->verbose = 1;
            }
            else if (strcmp(argv[i], "--dump-tokens") == 0) {
                ctx->dump_tokens = 1;
                ctx->keep_intermediates = 1;
            }
            else if (strcmp(argv[i], "--dump-ast") == 0) {
                ctx->dump_ast = 1;
                ctx->keep_intermediates = 1;
            }
            else if (strcmp(argv[i], "--dump-symbols") == 0) {
                ctx->dump_symbols = 1;
                ctx->keep_intermediates = 1;
            }
            else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                print_usage(argv[0]);
                return EXIT_USAGE;
            }
            else if (strcmp(argv[i], "--version") == 0) {
                print_version();
                return EXIT_USAGE;
            }
            else {
                fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
                return EXIT_ERROR;
            }
        }
        else {
            /* Input file */
            if (has_input) {
                fprintf(stderr, "Error: Multiple input files not supported\n");
                return EXIT_ERROR;
            }
            strncpy(ctx->source_file, argv[i], MAX_PATH_LEN - 1);
            ctx->source_file[MAX_PATH_LEN - 1] = '\0';
            has_input = 1;
        }
    }
    
    /* Check if input file was provided */
    if (!has_input) {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        return EXIT_USAGE;
    }
    
    /* Check if input file exists */
    if (!check_file_exists(ctx->source_file)) {
        fprintf(stderr, "Error: Input file not found: %s\n", ctx->source_file);
        return EXIT_ERROR;
    }
    
    /* Generate output filename if not specified */
    if (ctx->output_file[0] == '\0') {
        char base[MAX_PATH_LEN];
        get_base_filename(ctx->source_file, base, sizeof(base));
        snprintf(ctx->output_file, MAX_PATH_LEN, "%s.djc", base);
    }
    
    /* Generate intermediate filenames */
    generate_intermediate_filenames(ctx);
    
    return EXIT_SUCCESS;
}
```

### 2.3 Pipeline Execution

```c
int compiler_compile(CompilerContext* ctx) {
    uint32_t start_time, end_time;
    
    if (ctx->verbose) {
        printf("DOS Java Compiler v1.0\n");
        printf("Compiling: %s\n", ctx->source_file);
        printf("Output: %s\n\n", ctx->output_file);
    }
    
    /* Record start time */
    start_time = get_time_ms();
    
    /* Phase 1: Lexical Analysis */
    if (ctx->verbose) {
        printf("[1/4] Lexical analysis...\n");
    }
    if (run_lexer_phase(ctx) != 0) {
        report_phase_error(ctx);
        return EXIT_ERROR;
    }
    if (ctx->verbose) {
        printf("      %u tokens generated\n", ctx->token_count);
    }
    
    /* Phase 2: Parsing */
    if (ctx->verbose) {
        printf("[2/4] Parsing...\n");
    }
    if (run_parser_phase(ctx) != 0) {
        report_phase_error(ctx);
        return EXIT_ERROR;
    }
    if (ctx->verbose) {
        printf("      %u AST nodes created\n", ctx->node_count);
    }
    
    /* Phase 3: Semantic Analysis */
    if (ctx->verbose) {
        printf("[3/4] Semantic analysis...\n");
    }
    if (run_semantic_phase(ctx) != 0) {
        report_phase_error(ctx);
        return EXIT_ERROR;
    }
    if (ctx->verbose) {
        printf("      %u symbols defined\n", ctx->symbol_count);
    }
    
    /* Phase 4: Code Generation */
    if (ctx->verbose) {
        printf("[4/4] Code generation...\n");
    }
    if (run_codegen_phase(ctx) != 0) {
        report_phase_error(ctx);
        return EXIT_ERROR;
    }
    if (ctx->verbose) {
        printf("      %u bytes generated\n", ctx->bytecode_size);
    }
    
    /* Record end time */
    end_time = get_time_ms();
    ctx->compile_time_ms = end_time - start_time;
    
    /* Success */
    if (ctx->verbose) {
        printf("\nCompilation successful!\n");
        print_statistics(ctx);
    }
    else {
        printf("Compiled: %s -> %s\n", ctx->source_file, ctx->output_file);
    }
    
    return EXIT_SUCCESS;
}
```

### 2.4 Phase Execution Functions

```c
int run_lexer_phase(CompilerContext* ctx) {
    FILE* fp;
    
    ctx->error_phase = PHASE_LEXER;
    
    /* Create lexer */
    ctx->lexer = lexer_create();
    if (!ctx->lexer) {
        strcpy(ctx->error_message, "Failed to create lexer");
        return EXIT_ERROR;
    }
    
    /* Open source file */
    fp = fopen(ctx->source_file, "r");
    if (!fp) {
        strcpy(ctx->error_message, "Failed to open source file");
        return EXIT_ERROR;
    }
    
    /* Initialize lexer with file */
    if (lexer_init_file(ctx->lexer, fp) != 0) {
        fclose(fp);
        strcpy(ctx->error_message, "Failed to initialize lexer");
        return EXIT_ERROR;
    }
    
    /* Tokenize entire file */
    while (1) {
        Token tok;
        if (lexer_next_token(ctx->lexer, &tok) != 0) {
            fclose(fp);
            snprintf(ctx->error_message, sizeof(ctx->error_message),
                     "Lexical error at line %u, column %u",
                     ctx->lexer->line, ctx->lexer->column);
            return EXIT_ERROR;
        }
        
        ctx->token_count++;
        
        if (tok.type == TOKEN_EOF) {
            break;
        }
    }
    
    ctx->line_count = ctx->lexer->line;
    
    fclose(fp);
    
    /* Dump tokens if requested */
    if (ctx->dump_tokens) {
        dump_tokens_to_file(ctx);
    }
    
    return EXIT_SUCCESS;
}

int run_parser_phase(CompilerContext* ctx) {
    ctx->error_phase = PHASE_PARSER;
    
    /* Create parser */
    ctx->parser = parser_create(ctx->lexer);
    if (!ctx->parser) {
        strcpy(ctx->error_message, "Failed to create parser");
        return EXIT_ERROR;
    }
    
    /* Parse the program */
    if (parser_parse(ctx->parser) != 0) {
        snprintf(ctx->error_message, sizeof(ctx->error_message),
                 "Parse error at line %u, column %u",
                 ctx->lexer->line, ctx->lexer->column);
        return EXIT_ERROR;
    }
    
    ctx->node_count = ctx->parser->node_count;
    
    /* Dump AST if requested */
    if (ctx->dump_ast) {
        dump_ast_to_file(ctx);
    }
    
    return EXIT_SUCCESS;
}

int run_semantic_phase(CompilerContext* ctx) {
    ctx->error_phase = PHASE_SEMANTIC;
    
    /* Create semantic analyzer */
    ctx->semantic = semantic_create(ctx->parser);
    if (!ctx->semantic) {
        strcpy(ctx->error_message, "Failed to create semantic analyzer");
        return EXIT_ERROR;
    }
    
    /* Analyze the AST */
    if (semantic_analyze(ctx->semantic) != 0) {
        if (ctx->semantic->error_count > 0) {
            snprintf(ctx->error_message, sizeof(ctx->error_message),
                     "Semantic error: %s", ctx->semantic->error_message);
        } else {
            strcpy(ctx->error_message, "Semantic analysis failed");
        }
        return EXIT_ERROR;
    }
    
    ctx->symbol_count = ctx->semantic->symbol_table->symbol_count;
    
    /* Dump symbol table if requested */
    if (ctx->dump_symbols) {
        dump_symbols_to_file(ctx);
    }
    
    return EXIT_SUCCESS;
}

int run_codegen_phase(CompilerContext* ctx) {
    ctx->error_phase = PHASE_CODEGEN;
    
    /* Create code generator */
    ctx->codegen = codegen_create(ctx->parser, ctx->semantic);
    if (!ctx->codegen) {
        strcpy(ctx->error_message, "Failed to create code generator");
        return EXIT_ERROR;
    }
    
    /* Generate bytecode */
    if (codegen_generate(ctx->codegen) != 0) {
        strcpy(ctx->error_message, "Code generation failed");
        return EXIT_ERROR;
    }
    
    /* Write output file */
    if (codegen_write_file(ctx->codegen, ctx->output_file) != 0) {
        snprintf(ctx->error_message, sizeof(ctx->error_message),
                 "Failed to write output file: %s", ctx->output_file);
        return EXIT_ERROR;
    }
    
    ctx->bytecode_size = ctx->codegen->code_size;
    
    return EXIT_SUCCESS;
}
```

## 3. Utility Functions

### 3.1 File Management

```c
void generate_intermediate_filenames(CompilerContext* ctx) {
    char base[MAX_PATH_LEN];
    get_base_filename(ctx->source_file, base, sizeof(base));
    
    snprintf(ctx->tok_file, MAX_PATH_LEN, "%s.tok", base);
    snprintf(ctx->ast_file, MAX_PATH_LEN, "%s.ast", base);
    snprintf(ctx->sym_file, MAX_PATH_LEN, "%s.sym", base);
}

void cleanup_intermediate_files(CompilerContext* ctx) {
    if (ctx->tok_file[0] != '\0') {
        remove(ctx->tok_file);
    }
    if (ctx->ast_file[0] != '\0') {
        remove(ctx->ast_file);
    }
    if (ctx->sym_file[0] != '\0') {
        remove(ctx->sym_file);
    }
}

int check_file_exists(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp) {
        fclose(fp);
        return 1;
    }
    return 0;
}

void get_base_filename(const char* path, char* base, size_t base_size) {
    const char* last_slash;
    const char* last_dot;
    size_t len;
    
    /* Find last path separator */
    last_slash = strrchr(path, '\\');
    if (!last_slash) {
        last_slash = strrchr(path, '/');
    }
    
    /* Start from filename (after last slash) */
    if (last_slash) {
        path = last_slash + 1;
    }
    
    /* Find extension */
    last_dot = strrchr(path, '.');
    
    /* Copy base name */
    if (last_dot) {
        len = last_dot - path;
    } else {
        len = strlen(path);
    }
    
    if (len >= base_size) {
        len = base_size - 1;
    }
    
    strncpy(base, path, len);
    base[len] = '\0';
}
```

### 3.2 Reporting Functions

```c
void print_statistics(const CompilerContext* ctx) {
    printf("\nStatistics:\n");
    printf("  Lines:     %u\n", ctx->line_count);
    printf("  Tokens:    %u\n", ctx->token_count);
    printf("  AST nodes: %u\n", ctx->node_count);
    printf("  Symbols:   %u\n", ctx->symbol_count);
    printf("  Bytecode:  %u bytes\n", ctx->bytecode_size);
    printf("  Time:      %lu ms\n", (unsigned long)ctx->compile_time_ms);
}

void report_phase_error(const CompilerContext* ctx) {
    const char* phase_name;
    
    switch (ctx->error_phase) {
        case PHASE_LEXER:
            phase_name = "Lexical analysis";
            break;
        case PHASE_PARSER:
            phase_name = "Parsing";
            break;
        case PHASE_SEMANTIC:
            phase_name = "Semantic analysis";
            break;
        case PHASE_CODEGEN:
            phase_name = "Code generation";
            break;
        default:
            phase_name = "Unknown";
            break;
    }
    
    fprintf(stderr, "\nCompilation failed in phase: %s\n", phase_name);
    if (ctx->error_message[0] != '\0') {
        fprintf(stderr, "Error: %s\n", ctx->error_message);
    }
}

void print_usage(const char* program_name) {
    printf("DOS Java Compiler v1.0\n");
    printf("\nUsage: %s [options] <source.java>\n", program_name);
    printf("\nOptions:\n");
    printf("  -o <file>       Output file (default: <source>.djc)\n");
    printf("  -k              Keep intermediate files\n");
    printf("  -v              Verbose output\n");
    printf("  --dump-tokens   Dump tokens to .tok file\n");
    printf("  --dump-ast      Dump AST to .ast file\n");
    printf("  --dump-symbols  Dump symbol table to .sym file\n");
    printf("  -h, --help      Show this help message\n");
    printf("  --version       Show version information\n");
    printf("\nExamples:\n");
    printf("  %s Hello.java\n", program_name);
    printf("  %s -o test.djc Test.java\n", program_name);
    printf("  %s -k -v Hello.java\n", program_name);
}

void print_version(void) {
    printf("DOS Java Compiler v1.0\n");
    printf("16-bit PC-DOS Java Compiler\n");
    printf("Built with Open Watcom v2\n");
}
```

## 4. Main Function

```c
int main(int argc, char* argv[]) {
    CompilerContext ctx;
    int result;
    
    /* Initialize compiler context */
    compiler_init(&ctx);
    
    /* Parse command-line arguments */
    result = parse_arguments(argc, argv, &ctx);
    if (result != EXIT_SUCCESS) {
        return result;
    }
    
    /* Run compilation pipeline */
    result = compiler_compile(&ctx);
    
    /* Cleanup */
    compiler_cleanup(&ctx);
    
    return result;
}
```

## 5. Time Measurement (DOS-specific)

```c
/* For DOS, use BIOS timer tick count */
#include <dos.h>

uint32_t get_time_ms(void) {
    union REGS regs;
    uint32_t ticks;
    
    /* Get timer tick count (INT 1Ah, AH=00h) */
    regs.h.ah = 0x00;
    int86(0x1A, &regs, &regs);
    
    /* Combine CX:DX into 32-bit tick count */
    ticks = ((uint32_t)regs.w.cx << 16) | regs.w.dx;
    
    /* Convert ticks to milliseconds (18.2 ticks per second) */
    return (ticks * 1000) / 18;
}
```

## 6. Memory Considerations

### Heap Usage
- Lexer: ~8KB
- Parser: ~16KB (AST nodes)
- Semantic: ~8KB (symbol table)
- CodeGen: ~8KB (bytecode buffer)
- **Total**: ~40KB heap usage

### Stack Usage
- CompilerContext: ~1KB
- Function call depth: ~2KB
- **Total**: ~3KB stack usage

### DOS Memory Model
- Small model: 64KB code + 64KB data
- Heap allocated from data segment
- Should fit comfortably in 64KB data segment

## 7. Error Handling Strategy

### Error Categories
1. **File errors**: Cannot open, read, or write files
2. **Lexical errors**: Invalid tokens, unterminated strings
3. **Parse errors**: Syntax errors, unexpected tokens
4. **Semantic errors**: Type mismatches, undefined symbols
5. **Codegen errors**: Internal errors, output file errors

### Error Reporting Format
```
Error: <phase> failed at line <line>, column <column>
<error message>
<source line with error>
       ^
```

## 8. Testing Strategy

### Unit Tests
- Test each phase independently
- Test error handling
- Test edge cases

### Integration Tests
- Test full pipeline
- Test with various Java programs
- Test error propagation

### Performance Tests
- Measure compilation time
- Check memory usage
- Verify DOS compatibility

## 9. Build Integration

### Makefile Target
```makefile
djc: $(BIN_DIR)/djc.exe

$(BIN_DIR)/djc.exe: $(OBJ_DIR)/djc.obj $(COMPILER_OBJS) $(FORMAT_OBJS) $(VM_OBJS)
	$(LD) $(LDFLAGS) name $@ file { $< $(COMPILER_OBJS) $(FORMAT_OBJS) $(VM_OBJS) }
```

## 10. Success Criteria

- ✓ Compiles with Open Watcom v2 without warnings
- ✓ Runs on 16-bit DOS
- ✓ Compiles simple Java programs successfully
- ✓ Generates valid .djc bytecode
- ✓ Provides clear error messages
- ✓ Completes in reasonable time (<5 seconds for small programs)
- ✓ Uses <64KB memory