#ifndef LEXER_H
#define LEXER_H

#include "../../src/types.h"
#include <stdio.h>

/**
 * DOS Java Compiler - Lexical Analyzer
 * 
 * Tokenizes Java source code for the DOS Java compiler.
 * Optimized for 16-bit DOS environment with limited memory.
 */

/* Token types */
typedef enum {
    TOK_EOF = 0,
    
    /* Keywords */
    TOK_CLASS,          /* class */
    TOK_PUBLIC,         /* public */
    TOK_STATIC,         /* static */
    TOK_VOID,           /* void */
    TOK_INT,            /* int */
    TOK_BOOLEAN,        /* boolean */
    TOK_IF,             /* if */
    TOK_ELSE,           /* else */
    TOK_WHILE,          /* while */
    TOK_FOR,            /* for */
    TOK_RETURN,         /* return */
    TOK_NEW,            /* new */
    TOK_THIS,           /* this */
    
    /* Literals */
    TOK_IDENTIFIER,     /* variable/method/class names */
    TOK_INTEGER,        /* integer literal */
    TOK_STRING,         /* string literal */
    TOK_TRUE,           /* true */
    TOK_FALSE,          /* false */
    
    /* Operators */
    TOK_PLUS,           /* + */
    TOK_MINUS,          /* - */
    TOK_STAR,           /* * */
    TOK_SLASH,          /* / */
    TOK_PERCENT,        /* % */
    TOK_ASSIGN,         /* = */
    TOK_EQ,             /* == */
    TOK_NE,             /* != */
    TOK_LT,             /* < */
    TOK_LE,             /* <= */
    TOK_GT,             /* > */
    TOK_GE,             /* >= */
    TOK_AND,            /* && */
    TOK_OR,             /* || */
    TOK_NOT,            /* ! */
    
    /* Delimiters */
    TOK_LPAREN,         /* ( */
    TOK_RPAREN,         /* ) */
    TOK_LBRACE,         /* { */
    TOK_RBRACE,         /* } */
    TOK_LBRACKET,       /* [ */
    TOK_RBRACKET,       /* ] */
    TOK_SEMICOLON,      /* ; */
    TOK_COMMA,          /* , */
    TOK_DOT,            /* . */
    
    /* Error */
    TOK_ERROR           /* Lexical error */
} TokenType;

/* Token structure */
typedef struct {
    TokenType type;
    uint16_t line;
    uint16_t column;
    union {
        int16_t int_value;      /* For TOK_INTEGER */
        uint16_t str_offset;    /* For TOK_IDENTIFIER, TOK_STRING */
    } value;
} Token;

/* Lexer state */
typedef struct {
    FILE* source;               /* Source file */
    FILE* tokens;               /* Token output file */
    char buffer[512];           /* Input buffer */
    uint16_t buf_pos;           /* Current position in buffer */
    uint16_t buf_len;           /* Valid data in buffer */
    uint16_t line;              /* Current line number */
    uint16_t column;            /* Current column number */
    char string_pool[2048];     /* String pool for identifiers/strings */
    uint16_t pool_size;         /* Current pool size */
    char current_char;          /* Current character */
    int has_error;              /* Error flag */
} Lexer;

/**
 * Initialize lexer with source file
 * @param lexer Lexer state
 * @param source_file Source file path
 * @param token_file Token output file path
 * @return 0 on success, -1 on error
 */
int lexer_init(Lexer* lexer, const char* source_file, const char* token_file);

/**
 * Get next token from source
 * @param lexer Lexer state
 * @param token Output token
 * @return 0 on success, -1 on error
 */
int lexer_next_token(Lexer* lexer, Token* token);

/**
 * Write token to output file
 * @param lexer Lexer state
 * @param token Token to write
 * @return 0 on success, -1 on error
 */
int lexer_write_token(Lexer* lexer, Token* token);

/**
 * Cleanup lexer and close files
 * @param lexer Lexer state
 */
void lexer_cleanup(Lexer* lexer);

/**
 * Get token type name (for debugging)
 * @param type Token type
 * @return String name of token type
 */
const char* token_type_name(TokenType type);

/**
 * Add string to string pool
 * @param lexer Lexer state
 * @param str String to add
 * @return Offset in string pool, or 0xFFFF on error
 */
uint16_t lexer_add_string(Lexer* lexer, const char* str);

/**
 * Get string from string pool
 * @param lexer Lexer state
 * @param offset Offset in string pool
 * @return Pointer to string, or NULL on error
 */
const char* lexer_get_string(Lexer* lexer, uint16_t offset);

/**
 * Get token type name for debugging
 * @param type Token type
 * @return String representation of token type
 */
const char* token_type_name(TokenType type);

#endif /* LEXER_H */

// Made with Bob
