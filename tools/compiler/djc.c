/*
 * djc.c - DOS Java Compiler Driver Implementation
 * 
 * Main compiler driver that orchestrates all compilation phases
 * using file-based pipeline.
 */

#include "djc.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initialization and cleanup
 */

void compiler_init(CompilerContext* ctx) {
    /* Clear all fields */
    memset(ctx, 0, sizeof(CompilerContext));
    
    /* Set defaults */
    ctx->keep_intermediates = 0;
    ctx->verbose = 0;
    ctx->has_error = 0;
}

void compiler_cleanup(CompilerContext* ctx) {
    /* Remove intermediate files if not keeping them */
    if (!ctx->keep_intermediates) {
        cleanup_intermediate_files(ctx);
    }
}

/*
 * Utility functions
 */

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
    const char* start;
    
    /* Find last path separator (DOS uses backslash) */
    last_slash = strrchr(path, '\\');
    if (!last_slash) {
        last_slash = strrchr(path, '/');
    }
    
    /* Start from filename (after last slash) */
    start = last_slash ? (last_slash + 1) : path;
    
    /* Find extension */
    last_dot = strrchr(start, '.');
    
    /* Copy base name */
    if (last_dot) {
        len = last_dot - start;
    } else {
        len = strlen(start);
    }
    
    if (len >= base_size) {
        len = base_size - 1;
    }
    
    strncpy(base, start, len);
    base[len] = '\0';
}

void generate_intermediate_filenames(CompilerContext* ctx) {
    char base[MAX_PATH_LEN];
    get_base_filename(ctx->source_file, base, sizeof(base));
    
    sprintf(ctx->tok_file, "%s.tok", base);
    sprintf(ctx->ast_file, "%s.ast", base);
    sprintf(ctx->sym_file, "%s.sym", base);
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

/*
 * Reporting functions
 */

void print_usage(const char* program_name) {
    printf("DOS Java Compiler v1.0\n");
    printf("\n");
    printf("Usage: %s [options] <source.java>\n", program_name);
    printf("\n");
    printf("Options:\n");
    printf("  -o <file>       Output file (default: <source>.djc)\n");
    printf("  -k              Keep intermediate files\n");
    printf("  -v              Verbose output\n");
    printf("  -h, --help      Show this help message\n");
    printf("  --version       Show version information\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s Hello.java\n", program_name);
    printf("  %s -o test.djc Test.java\n", program_name);
    printf("  %s -k -v Hello.java\n", program_name);
}

void print_version(void) {
    printf("DOS Java Compiler v1.0\n");
    printf("16-bit PC-DOS Java Compiler\n");
    printf("Built with Open Watcom v2\n");
}

void print_statistics(const CompilerContext* ctx) {
    printf("\n");
    printf("Statistics:\n");
    printf("  Lines:     %u\n", ctx->line_count);
    printf("  Tokens:    %u\n", ctx->token_count);
    printf("  AST nodes: %u\n", ctx->node_count);
    printf("  Symbols:   %u\n", ctx->symbol_count);
    printf("  Bytecode:  %u bytes\n", ctx->bytecode_size);
}

void report_error(const CompilerContext* ctx) {
    fprintf(stderr, "\n");
    fprintf(stderr, "Compilation failed\n");
    if (ctx->error_message[0] != '\0') {
        fprintf(stderr, "Error: %s\n", ctx->error_message);
    }
}

/*
 * Command-line parsing
 */

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
        sprintf(ctx->output_file, "%s.djc", base);
    }
    
    /* Generate intermediate filenames */
    generate_intermediate_filenames(ctx);
    
    return EXIT_SUCCESS;
}

/*
 * Phase execution functions
 */

int run_lexer_phase(CompilerContext* ctx) {
    Lexer* lexer;
    Token token;
    int result;
    
    /* Allocate lexer on heap to avoid stack overflow */
    lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) {
        strcpy(ctx->error_message, "Out of memory (lexer)");
        ctx->has_error = 1;
        return EXIT_ERROR;
    }
    
    /* Initialize lexer */
    result = lexer_init(lexer, ctx->source_file, ctx->tok_file);
    if (result != 0) {
        strcpy(ctx->error_message, "Failed to initialize lexer");
        ctx->has_error = 1;
        free(lexer);
        return EXIT_ERROR;
    }
    
    /* Tokenize entire file */
    ctx->token_count = 0;
    while (1) {
        result = lexer_next_token(lexer, &token);
        if (result != 0) {
            sprintf(ctx->error_message, "Lexical error at line %u, column %u",
                    lexer->line, lexer->column);
            ctx->has_error = 1;
            lexer_cleanup(lexer);
            free(lexer);
            return EXIT_ERROR;
        }
        
        /* Write token to output file */
        result = lexer_write_token(lexer, &token);
        if (result != 0) {
            strcpy(ctx->error_message, "Failed to write token");
            ctx->has_error = 1;
            lexer_cleanup(lexer);
            free(lexer);
            return EXIT_ERROR;
        }
        
        ctx->token_count++;
        
        if (token.type == TOK_EOF) {
            break;
        }
    }
    
    ctx->line_count = lexer->line;
    
    /* Cleanup */
    lexer_cleanup(lexer);
    free(lexer);
    
    return EXIT_SUCCESS;
}

int run_parser_phase(CompilerContext* ctx) {
    Parser* parser;
    uint16_t root_node;
    int result;
    
    /* Allocate parser on heap to avoid stack overflow */
    parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) {
        strcpy(ctx->error_message, "Out of memory (parser)");
        ctx->has_error = 1;
        return EXIT_ERROR;
    }
    
    /* Initialize parser */
    result = parser_init(parser, ctx->tok_file, ctx->ast_file);
    if (result != 0) {
        strcpy(ctx->error_message, "Failed to initialize parser");
        ctx->has_error = 1;
        free(parser);
        return EXIT_ERROR;
    }
    
    /* Parse the program */
    root_node = parser_parse(parser);
    if (root_node == 0) {
        strcpy(ctx->error_message, "Parse error");
        ctx->has_error = 1;
        parser_cleanup(parser);
        free(parser);
        return EXIT_ERROR;
    }
    
    ctx->node_count = parser->total_nodes;
    
    /* Cleanup */
    parser_cleanup(parser);
    free(parser);
    
    return EXIT_SUCCESS;
}

int run_semantic_phase(CompilerContext* ctx) {
    SemanticAnalyzer* analyzer;
    int result;
    
    /* Allocate semantic analyzer on heap to avoid stack overflow */
    analyzer = (SemanticAnalyzer*)malloc(sizeof(SemanticAnalyzer));
    if (!analyzer) {
        strcpy(ctx->error_message, "Out of memory (semantic analyzer)");
        ctx->has_error = 1;
        return EXIT_ERROR;
    }
    
    /* Initialize semantic analyzer */
    result = semantic_init(analyzer, ctx->ast_file, ctx->sym_file);
    if (result != 0) {
        strcpy(ctx->error_message, "Failed to initialize semantic analyzer");
        ctx->has_error = 1;
        free(analyzer);
        return EXIT_ERROR;
    }
    
    /* Analyze the AST */
    result = semantic_analyze(analyzer);
    if (result != 0) {
        if (analyzer->error_count > 0) {
            strncpy(ctx->error_message, "Semantic errors detected", MAX_ERROR_LEN - 1);
            ctx->error_message[MAX_ERROR_LEN - 1] = '\0';
            /* Print accumulated errors */
            semantic_print_errors(analyzer);
        } else {
            strcpy(ctx->error_message, "Semantic analysis failed");
        }
        ctx->has_error = 1;
        semantic_cleanup(analyzer);
        free(analyzer);
        return EXIT_ERROR;
    }
    
    ctx->symbol_count = analyzer->symtable->symbol_count;
    
    /* Cleanup */
    semantic_cleanup(analyzer);
    free(analyzer);
    
    return EXIT_SUCCESS;
}

int run_codegen_phase(CompilerContext* ctx) {
    CodeGenerator* codegen;
    int result;
    
    /* Allocate code generator on heap to avoid stack overflow */
    codegen = (CodeGenerator*)malloc(sizeof(CodeGenerator));
    if (!codegen) {
        strcpy(ctx->error_message, "Out of memory (code generator)");
        ctx->has_error = 1;
        return EXIT_ERROR;
    }
    
    /* Initialize code generator */
    result = codegen_init(codegen, ctx->ast_file, ctx->sym_file, ctx->output_file);
    if (result != 0) {
        strcpy(ctx->error_message, "Failed to initialize code generator");
        ctx->has_error = 1;
        free(codegen);
        return EXIT_ERROR;
    }
    
    /* Generate bytecode */
    result = codegen_generate(codegen);
    if (result != 0) {
        strcpy(ctx->error_message, "Code generation failed");
        ctx->has_error = 1;
        codegen_cleanup(codegen);
        free(codegen);
        return EXIT_ERROR;
    }
    
    ctx->bytecode_size = codegen->bytecode->size;
    
    /* Cleanup */
    codegen_cleanup(codegen);
    free(codegen);
    
    return EXIT_SUCCESS;
}

/*
 * Main compilation function
 */

int compiler_compile(CompilerContext* ctx) {
    if (ctx->verbose) {
        printf("DOS Java Compiler v1.0\n");
        printf("Compiling: %s\n", ctx->source_file);
        printf("Output: %s\n\n", ctx->output_file);
    }
    
    /* Phase 1: Lexical Analysis */
    if (ctx->verbose) {
        printf("[1/4] Lexical analysis...\n");
    }
    if (run_lexer_phase(ctx) != 0) {
        report_error(ctx);
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
        report_error(ctx);
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
        report_error(ctx);
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
        report_error(ctx);
        return EXIT_ERROR;
    }
    if (ctx->verbose) {
        printf("      %u bytes generated\n", ctx->bytecode_size);
    }
    
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

/*
 * Main function
 */

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


