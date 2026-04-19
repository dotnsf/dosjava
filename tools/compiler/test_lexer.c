/**
 * DOS Java Compiler - Lexer Test Program
 * 
 * Tests the lexical analyzer by tokenizing a Java source file
 * and displaying the tokens.
 */

#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

static void print_token(Lexer* lexer, Token* token) {
    const char* str;
    
    printf("%4d:%3d  %-15s", token->line, token->column, token_type_name(token->type));
    
    switch (token->type) {
        case TOK_IDENTIFIER:
            str = lexer_get_string(lexer, token->value.str_offset);
            printf("  '%s'", str ? str : "(null)");
            break;
            
        case TOK_INTEGER:
            printf("  %d", token->value.int_value);
            break;
            
        case TOK_STRING:
            str = lexer_get_string(lexer, token->value.str_offset);
            printf("  \"%s\"", str ? str : "(null)");
            break;
            
        default:
            break;
    }
    
    printf("\n");
}

int main(int argc, char* argv[]) {
    Lexer lexer;
    Token token;
    int token_count;
    const char* source_file;
    const char* token_file;
    
    /* Check arguments */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source.java> [tokens.tmp]\n", argv[0]);
        return 1;
    }
    
    source_file = argv[1];
    token_file = (argc >= 3) ? argv[2] : "tokens.tmp";
    
    printf("DOS Java Compiler - Lexer Test\n");
    printf("================================\n");
    printf("Source: %s\n", source_file);
    printf("Output: %s\n\n", token_file);
    
    /* Initialize lexer */
    if (lexer_init(&lexer, source_file, token_file) < 0) {
        fprintf(stderr, "Error: Failed to initialize lexer\n");
        return 1;
    }
    
    printf("Line:Col  Token Type       Value\n");
    printf("--------  ---------------  -----\n");
    
    /* Tokenize */
    token_count = 0;
    while (1) {
        if (lexer_next_token(&lexer, &token) < 0) {
            fprintf(stderr, "Error: Lexical analysis failed\n");
            lexer_cleanup(&lexer);
            return 1;
        }
        
        if (token.type == TOK_EOF) {
            break;
        }
        
        print_token(&lexer, &token);
        
        /* Write token to file */
        if (lexer_write_token(&lexer, &token) < 0) {
            fprintf(stderr, "Error: Failed to write token\n");
            lexer_cleanup(&lexer);
            return 1;
        }
        
        token_count++;
    }
    
    printf("\nTotal tokens: %d\n", token_count);
    printf("String pool size: %d bytes\n", lexer.pool_size);
    
    /* Cleanup */
    lexer_cleanup(&lexer);
    
    printf("\nTokenization complete!\n");
    return 0;
}

// Made with Bob
