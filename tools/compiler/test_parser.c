/**
 * DOS Java Compiler - Parser Test Program
 * 
 * Tests the parser by reading tokens and building AST.
 */

#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

static void print_ast_node(Parser* parser, uint16_t node_index, int indent);

int main(int argc, char* argv[]) {
    Lexer* lexer;
    Parser* parser;
    Token token;
    uint16_t root_node;
    const char* source_file;
    const char* token_file;
    const char* ast_file;
    
    /* Check arguments */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source.java> [tokens.tmp] [ast.tmp]\n", argv[0]);
        return 1;
    }
    
    source_file = argv[1];
    token_file = (argc >= 3) ? argv[2] : "tokens.tmp";
    ast_file = (argc >= 4) ? argv[3] : "ast.tmp";
    
    printf("DOS Java Compiler - Parser Test\n");
    printf("================================\n");
    printf("Source: %s\n", source_file);
    printf("Tokens: %s\n", token_file);
    printf("AST:    %s\n\n", ast_file);
    
    /* Allocate lexer on heap to avoid stack overflow */
    lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) {
        fprintf(stderr, "Error: Failed to allocate memory for lexer\n");
        return 1;
    }
    
    /* Step 1: Tokenize */
    printf("Step 1: Tokenizing...\n");
    if (lexer_init(lexer, source_file, token_file) < 0) {
        fprintf(stderr, "Error: Failed to initialize lexer\n");
        free(lexer);
        return 1;
    }
    
    while (1) {
        if (lexer_next_token(lexer, &token) < 0) {
            fprintf(stderr, "Error: Lexical analysis failed\n");
            lexer_cleanup(lexer);
            free(lexer);
            return 1;
        }
        
        if (token.type == TOK_EOF) {
            break;
        }
        
        if (lexer_write_token(lexer, &token) < 0) {
            fprintf(stderr, "Error: Failed to write token\n");
            lexer_cleanup(lexer);
            free(lexer);
            return 1;
        }
    }
    
    lexer_cleanup(lexer);
    free(lexer);
    printf("Tokenization complete.\n\n");
    
    /* Allocate parser on heap to avoid stack overflow */
    parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) {
        fprintf(stderr, "Error: Failed to allocate memory for parser\n");
        return 1;
    }
    
    /* Step 2: Parse */
    printf("Step 2: Parsing...\n");
    if (parser_init(parser, token_file, ast_file) < 0) {
        fprintf(stderr, "Error: Failed to initialize parser\n");
        free(parser);
        return 1;
    }
    
    root_node = parser_parse(parser);
    if (root_node == 0) {
        fprintf(stderr, "Error: Parsing failed\n");
        parser_cleanup(parser);
        free(parser);
        return 1;
    }
    
    printf("Parsing complete.\n");
    printf("Total nodes: %d\n", parser->total_nodes);
    printf("Errors: %d\n\n", parser->error_count);
    
    if (parser->error_count > 0) {
        printf("Parsing completed with errors.\n");
        parser_cleanup(parser);
        free(parser);
        return 1;
    }
    
    parser_cleanup(parser);
    free(parser);
    
    printf("Parsing successful!\n");
    return 0;
}


