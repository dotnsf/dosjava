/**
 * DOS Java Compiler - Lexical Analyzer Implementation
 * 
 * Tokenizes Java source code with memory-efficient buffering.
 * Designed for 16-bit DOS environment.
 */

#include "lexer.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* Keyword table */
static const struct {
    const char* name;
    TokenType type;
} keywords[] = {
    {"class", TOK_CLASS},
    {"public", TOK_PUBLIC},
    {"static", TOK_STATIC},
    {"void", TOK_VOID},
    {"int", TOK_INT},
    {"boolean", TOK_BOOLEAN},
    {"if", TOK_IF},
    {"else", TOK_ELSE},
    {"while", TOK_WHILE},
    {"for", TOK_FOR},
    {"return", TOK_RETURN},
    {"new", TOK_NEW},
    {"this", TOK_THIS},
    {"true", TOK_TRUE},
    {"false", TOK_FALSE},
    {NULL, TOK_EOF}
};

/* Forward declarations */
static int lexer_read_char(Lexer* lexer);
static void lexer_skip_whitespace(Lexer* lexer);
static void lexer_skip_comment(Lexer* lexer);
static int lexer_read_identifier(Lexer* lexer, Token* token);
static int lexer_read_number(Lexer* lexer, Token* token);
static int lexer_read_string_literal(Lexer* lexer, Token* token);
static TokenType lexer_lookup_keyword(const char* str);

/**
 * Initialize lexer
 */
int lexer_init(Lexer* lexer, const char* source_file, const char* token_file) {
    uint16_t placeholder;
    
    /* Initialize all fields */
    lexer->source = NULL;
    lexer->tokens = NULL;
    lexer->buf_pos = 0;
    lexer->buf_len = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->pool_size = 0;
    lexer->current_char = '\0';
    lexer->has_error = 0;
    
    /* Open source file */
    lexer->source = fopen(source_file, "r");
    if (!lexer->source) {
        fprintf(stderr, "Error: Cannot open source file '%s'\n", source_file);
        return -1;
    }
    
    /* Open token output file */
    lexer->tokens = fopen(token_file, "wb");
    if (!lexer->tokens) {
        fprintf(stderr, "Error: Cannot create token file '%s'\n", token_file);
        fclose(lexer->source);
        return -1;
    }
    
    /* Write placeholder for string pool size (will be updated in cleanup) */
    placeholder = 0;
    fwrite(&placeholder, sizeof(uint16_t), 1, lexer->tokens);
    
    /* Read first character */
    if (lexer_read_char(lexer) < 0) {
        lexer_cleanup(lexer);
        return -1;
    }
    
    return 0;
}

/**
 * Read next character from source
 */
static int lexer_read_char(Lexer* lexer) {
    /* Refill buffer if needed */
    if (lexer->buf_pos >= lexer->buf_len) {
        lexer->buf_len = (uint16_t)fread(lexer->buffer, 1, sizeof(lexer->buffer), lexer->source);
        lexer->buf_pos = 0;
        
        if (lexer->buf_len == 0) {
            lexer->current_char = '\0';
            return 0; /* EOF */
        }
    }
    
    lexer->current_char = lexer->buffer[lexer->buf_pos++];
    return 1;
}

/**
 * Peek at next character without consuming
 */
static char lexer_peek_char(Lexer* lexer) {
    if (lexer->buf_pos >= lexer->buf_len) {
        return '\0';
    }
    return lexer->buffer[lexer->buf_pos];
}

/**
 * Skip whitespace
 */
static void lexer_skip_whitespace(Lexer* lexer) {
    while (lexer->current_char != '\0') {
        if (lexer->current_char == ' ' || lexer->current_char == '\t' || 
            lexer->current_char == '\r') {
            lexer->column++;
            lexer_read_char(lexer);
        } else if (lexer->current_char == '\n') {
            lexer->line++;
            lexer->column = 1;
            lexer_read_char(lexer);
        } else {
            break;
        }
    }
}

/**
 * Skip single-line comment (double-slash)
 */
static void lexer_skip_line_comment(Lexer* lexer) {
    /* Skip until end of line */
    while (lexer->current_char != '\0' && lexer->current_char != '\n') {
        lexer_read_char(lexer);
    }
}

/**
 * Skip multi-line comment (slash-star ... star-slash)
 */
static void lexer_skip_block_comment(Lexer* lexer) {
    int prev_char;
    
    prev_char = 0;
    lexer_read_char(lexer); /* Skip * */
    
    while (lexer->current_char != '\0') {
        if (prev_char == '*' && lexer->current_char == '/') {
            lexer_read_char(lexer); /* Skip / */
            return;
        }
        
        if (lexer->current_char == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        
        prev_char = lexer->current_char;
        lexer_read_char(lexer);
    }
    
    /* Unterminated comment */
    fprintf(stderr, "Error: Unterminated comment\n");
    lexer->has_error = 1;
}

/**
 * Read identifier or keyword
 */
static int lexer_read_identifier(Lexer* lexer, Token* token) {
    char buffer[256];
    int len;
    TokenType keyword_type;
    
    len = 0;
    
    /* Read identifier characters */
    while (lexer->current_char != '\0' && 
           (isalnum(lexer->current_char) || lexer->current_char == '_')) {
        if (len < 255) {
            buffer[len++] = lexer->current_char;
        }
        lexer->column++;
        lexer_read_char(lexer);
    }
    
    buffer[len] = '\0';
    
    /* Check if it's a keyword */
    keyword_type = lexer_lookup_keyword(buffer);
    if (keyword_type != TOK_EOF) {
        token->type = keyword_type;
        return 0;
    }
    
    /* It's an identifier - add to string pool */
    token->type = TOK_IDENTIFIER;
    token->value.str_offset = lexer_add_string(lexer, buffer);
    
    if (token->value.str_offset == 0xFFFF) {
        fprintf(stderr, "Error: String pool overflow\n");
        return -1;
    }
    
    return 0;
}

/**
 * Read integer literal
 */
static int lexer_read_number(Lexer* lexer, Token* token) {
    int value;
    
    value = 0;
    
    while (lexer->current_char != '\0' && isdigit(lexer->current_char)) {
        value = value * 10 + (lexer->current_char - '0');
        lexer->column++;
        lexer_read_char(lexer);
    }
    
    token->type = TOK_INTEGER;
    token->value.int_value = (int16_t)value;
    
    return 0;
}

/**
 * Read string literal
 */
static int lexer_read_string_literal(Lexer* lexer, Token* token) {
    char buffer[256];
    int len;
    
    len = 0;
    
    /* Skip opening quote */
    lexer->column++;
    lexer_read_char(lexer);
    
    /* Read string content */
    while (lexer->current_char != '\0' && lexer->current_char != '"') {
        if (lexer->current_char == '\\') {
            /* Handle escape sequences */
            lexer->column++;
            lexer_read_char(lexer);
            
            if (lexer->current_char == 'n') {
                buffer[len++] = '\n';
            } else if (lexer->current_char == 't') {
                buffer[len++] = '\t';
            } else if (lexer->current_char == '\\') {
                buffer[len++] = '\\';
            } else if (lexer->current_char == '"') {
                buffer[len++] = '"';
            } else {
                buffer[len++] = lexer->current_char;
            }
        } else {
            if (len < 255) {
                buffer[len++] = lexer->current_char;
            }
        }
        
        lexer->column++;
        lexer_read_char(lexer);
    }
    
    if (lexer->current_char != '"') {
        fprintf(stderr, "Error: Unterminated string literal\n");
        return -1;
    }
    
    /* Skip closing quote */
    lexer->column++;
    lexer_read_char(lexer);
    
    buffer[len] = '\0';
    
    /* Add to string pool */
    token->type = TOK_STRING;
    token->value.str_offset = lexer_add_string(lexer, buffer);
    
    if (token->value.str_offset == 0xFFFF) {
        fprintf(stderr, "Error: String pool overflow\n");
        return -1;
    }
    
    return 0;
}

/**
 * Lookup keyword
 */
static TokenType lexer_lookup_keyword(const char* str) {
    int i;
    
    for (i = 0; keywords[i].name != NULL; i++) {
        if (strcmp(str, keywords[i].name) == 0) {
            return keywords[i].type;
        }
    }
    
    return TOK_EOF; /* Not a keyword */
}

/**
 * Get next token
 */
int lexer_next_token(Lexer* lexer, Token* token) {
    /* Skip whitespace and comments */
    while (1) {
        lexer_skip_whitespace(lexer);
        
        if (lexer->current_char == '/') {
            char next = lexer_peek_char(lexer);
            if (next == '/') {
                lexer_read_char(lexer); /* Skip first / */
                lexer_skip_line_comment(lexer);
                continue;
            } else if (next == '*') {
                lexer_read_char(lexer); /* Skip first / */
                lexer_skip_block_comment(lexer);
                continue;
            }
        }
        break;
    }
    
    /* Initialize token */
    token->line = lexer->line;
    token->column = lexer->column;
    
    /* EOF */
    if (lexer->current_char == '\0') {
        token->type = TOK_EOF;
        return 0;
    }
    
    /* Identifier or keyword */
    if (isalpha(lexer->current_char) || lexer->current_char == '_') {
        return lexer_read_identifier(lexer, token);
    }
    
    /* Number */
    if (isdigit(lexer->current_char)) {
        return lexer_read_number(lexer, token);
    }
    
    /* String literal */
    if (lexer->current_char == '"') {
        return lexer_read_string_literal(lexer, token);
    }
    
    /* Operators and delimiters */
    switch (lexer->current_char) {
        case '+':
            token->type = TOK_PLUS;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case '-':
            token->type = TOK_MINUS;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case '*':
            token->type = TOK_STAR;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case '/':
            token->type = TOK_SLASH;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case '%':
            token->type = TOK_PERCENT;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case '=':
            lexer->column++;
            lexer_read_char(lexer);
            if (lexer->current_char == '=') {
                token->type = TOK_EQ;
                lexer->column++;
                lexer_read_char(lexer);
            } else {
                token->type = TOK_ASSIGN;
            }
            break;
            
        case '!':
            lexer->column++;
            lexer_read_char(lexer);
            if (lexer->current_char == '=') {
                token->type = TOK_NE;
                lexer->column++;
                lexer_read_char(lexer);
            } else {
                token->type = TOK_NOT;
            }
            break;
            
        case '<':
            lexer->column++;
            lexer_read_char(lexer);
            if (lexer->current_char == '=') {
                token->type = TOK_LE;
                lexer->column++;
                lexer_read_char(lexer);
            } else {
                token->type = TOK_LT;
            }
            break;
            
        case '>':
            lexer->column++;
            lexer_read_char(lexer);
            if (lexer->current_char == '=') {
                token->type = TOK_GE;
                lexer->column++;
                lexer_read_char(lexer);
            } else {
                token->type = TOK_GT;
            }
            break;
            
        case '&':
            lexer->column++;
            lexer_read_char(lexer);
            if (lexer->current_char == '&') {
                token->type = TOK_AND;
                lexer->column++;
                lexer_read_char(lexer);
            } else {
                fprintf(stderr, "Error: Expected '&' after '&'\n");
                token->type = TOK_ERROR;
                return -1;
            }
            break;
            
        case '|':
            lexer->column++;
            lexer_read_char(lexer);
            if (lexer->current_char == '|') {
                token->type = TOK_OR;
                lexer->column++;
                lexer_read_char(lexer);
            } else {
                fprintf(stderr, "Error: Expected '|' after '|'\n");
                token->type = TOK_ERROR;
                return -1;
            }
            break;
            
        case '(':
            token->type = TOK_LPAREN;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case ')':
            token->type = TOK_RPAREN;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case '{':
            token->type = TOK_LBRACE;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case '}':
            token->type = TOK_RBRACE;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case '[':
            token->type = TOK_LBRACKET;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case ']':
            token->type = TOK_RBRACKET;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case ';':
            token->type = TOK_SEMICOLON;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case ',':
            token->type = TOK_COMMA;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        case '.':
            token->type = TOK_DOT;
            lexer->column++;
            lexer_read_char(lexer);
            break;
            
        default:
            fprintf(stderr, "Error: Unexpected character '%c' (0x%02X)\n", 
                    lexer->current_char, (unsigned char)lexer->current_char);
            token->type = TOK_ERROR;
            return -1;
    }
    
    return 0;
}

/**
 * Write token to output file
 */
int lexer_write_token(Lexer* lexer, Token* token) {
    size_t written;
    
    written = fwrite(token, sizeof(Token), 1, lexer->tokens);
    if (written != 1) {
        fprintf(stderr, "Error: Failed to write token\n");
        return -1;
    }
    
    return 0;
}

/**
 * Add string to string pool
 */
uint16_t lexer_add_string(Lexer* lexer, const char* str) {
    uint16_t offset;
    size_t len;
    
    len = strlen(str) + 1; /* Include null terminator */
    
    if (lexer->pool_size + len > sizeof(lexer->string_pool)) {
        return 0xFFFF; /* Pool overflow */
    }
    
    offset = lexer->pool_size;
    strcpy(lexer->string_pool + offset, str);
    lexer->pool_size += (uint16_t)len;
    
    return offset;
}

/**
 * Get string from string pool
 */
const char* lexer_get_string(Lexer* lexer, uint16_t offset) {
    if (offset >= lexer->pool_size) {
        return NULL;
    }
    return lexer->string_pool + offset;
}

/**
 * Get token type name
 */
const char* token_type_name(TokenType type) {
    switch (type) {
        case TOK_EOF: return "EOF";
        case TOK_CLASS: return "class";
        case TOK_PUBLIC: return "public";
        case TOK_STATIC: return "static";
        case TOK_VOID: return "void";
        case TOK_INT: return "int";
        case TOK_BOOLEAN: return "boolean";
        case TOK_IF: return "if";
        case TOK_ELSE: return "else";
        case TOK_WHILE: return "while";
        case TOK_FOR: return "for";
        case TOK_RETURN: return "return";
        case TOK_NEW: return "new";
        case TOK_THIS: return "this";
        case TOK_IDENTIFIER: return "IDENTIFIER";
        case TOK_INTEGER: return "INTEGER";
        case TOK_STRING: return "STRING";
        case TOK_TRUE: return "true";
        case TOK_FALSE: return "false";
        case TOK_PLUS: return "+";
        case TOK_MINUS: return "-";
        case TOK_STAR: return "*";
        case TOK_SLASH: return "/";
        case TOK_PERCENT: return "%";
        case TOK_ASSIGN: return "=";
        case TOK_EQ: return "==";
        case TOK_NE: return "!=";
        case TOK_LT: return "<";
        case TOK_LE: return "<=";
        case TOK_GT: return ">";
        case TOK_GE: return ">=";
        case TOK_AND: return "&&";
        case TOK_OR: return "||";
        case TOK_NOT: return "!";
        case TOK_LPAREN: return "(";
        case TOK_RPAREN: return ")";
        case TOK_LBRACE: return "{";
        case TOK_RBRACE: return "}";
        case TOK_LBRACKET: return "[";
        case TOK_RBRACKET: return "]";
        case TOK_SEMICOLON: return ";";
        case TOK_COMMA: return ",";
        case TOK_DOT: return ".";
        case TOK_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

/**
 * Cleanup lexer
 */
void lexer_cleanup(Lexer* lexer) {
    if (lexer->source) {
        fclose(lexer->source);
        lexer->source = NULL;
    }
    if (lexer->tokens) {
        /* Write string pool size at beginning of file */
        fseek(lexer->tokens, 0, SEEK_SET);
        fwrite(&lexer->pool_size, sizeof(uint16_t), 1, lexer->tokens);
        
        /* Write string pool data at end of file */
        fseek(lexer->tokens, 0, SEEK_END);
        if (lexer->pool_size > 0) {
            fwrite(lexer->string_pool, 1, lexer->pool_size, lexer->tokens);
        }
        
        fclose(lexer->tokens);
        lexer->tokens = NULL;
    }
}

// Made with Bob
