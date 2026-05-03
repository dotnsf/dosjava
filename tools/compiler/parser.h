#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <stdio.h>

/**
 * DOS Java Compiler - Parser (Syntax Analyzer)
 * 
 * Parses token stream and builds Abstract Syntax Tree (AST).
 * Uses recursive descent parsing with operator precedence.
 * Optimized for 16-bit DOS environment with limited memory.
 */

/* Parser state */
typedef struct {
    /* Input */
    FILE* tokens;               /* Token input file */
    Token current;              /* Current token */
    Token lookahead;            /* Lookahead token */
    
    /* Output */
    FILE* ast_file;             /* AST output file */
    
    /* AST node buffer
     * Keep large enough to avoid mid-parse flushes, because many parser paths
     * still link sibling nodes by in-memory index.
     */
    ASTNode nodes[512];
    uint16_t node_count;        /* Nodes in buffer */
    uint16_t total_nodes;       /* Total nodes written */
    
    /* String pool (shared with lexer) */
    char string_pool[2048];
    uint16_t pool_size;
    
    /* Error tracking */
    int has_error;
    uint16_t error_count;
    
    /* Context flags */
    uint16_t parsing_array_index;
} Parser;

/**
 * Initialize parser
 * @param parser Parser state
 * @param token_file Token input file path
 * @param ast_file AST output file path
 * @return 0 on success, -1 on error
 */
int parser_init(Parser* parser, const char* token_file, const char* ast_file);

/**
 * Parse entire program
 * @param parser Parser state
 * @return Index of root node, or 0 on error
 */
uint16_t parser_parse(Parser* parser);

/**
 * Cleanup parser and close files
 * @param parser Parser state
 */
void parser_cleanup(Parser* parser);

/**
 * Add string to string pool
 * @param parser Parser state
 * @param str String to add
 * @return Offset in string pool, or 0xFFFF on error
 */
uint16_t parser_add_string(Parser* parser, const char* str);

/**
 * Get string from string pool
 * @param parser Parser state
 * @param offset Offset in string pool
 * @return Pointer to string, or NULL on error
 */
const char* parser_get_string(Parser* parser, uint16_t offset);

/**
 * Allocate new AST node
 * @param parser Parser state
 * @param type Node type
 * @return Index of new node, or 0 on error
 */
uint16_t parser_alloc_node(Parser* parser, NodeType type);

/**
 * Write AST node to file
 * @param parser Parser state
 * @param node_index Index of node to write
 * @return 0 on success, -1 on error
 */
int parser_write_node(Parser* parser, uint16_t node_index);

/**
 * Flush node buffer to file
 * @param parser Parser state
 * @return 0 on success, -1 on error
 */
int parser_flush_nodes(Parser* parser);

/* Parsing functions (internal) */

/**
 * Parse class declaration
 * @param parser Parser state
 * @return Index of class node, or 0 on error
 */
uint16_t parse_class(Parser* parser);

/**
 * Parse member declaration (method or field)
 * @param parser Parser state
 * @return Index of member node, or 0 on error
 */
uint16_t parse_member(Parser* parser);

/**
 * Parse method declaration
 * @param parser Parser state
 * @param is_public Is method public
 * @param is_static Is method static
 * @param return_type Return type
 * @return Index of method node, or 0 on error
 */
uint16_t parse_method(Parser* parser, int is_public, int is_static, TypeInfo return_type);

/**
 * Parse field declaration
 * @param parser Parser state
 * @param is_public Is field public
 * @param is_static Is field static
 * @param type Field type
 * @return Index of field node, or 0 on error
 */
uint16_t parse_field(Parser* parser, int is_public, int is_static, TypeInfo type);

/**
 * Parse statement
 * @param parser Parser state
 * @return Index of statement node, or 0 on error
 */
uint16_t parse_statement(Parser* parser);

/**
 * Parse block statement
 * @param parser Parser state
 * @return Index of block node, or 0 on error
 */
uint16_t parse_block(Parser* parser);

/**
 * Parse variable declaration
 * @param parser Parser state
 * @return Index of var_decl node, or 0 on error
 */
uint16_t parse_var_decl(Parser* parser);

/**
 * Parse if statement
 * @param parser Parser state
 * @return Index of if node, or 0 on error
 */
uint16_t parse_if(Parser* parser);

/**
 * Parse while statement
 * @param parser Parser state
 * @return Index of while node, or 0 on error
 */
uint16_t parse_while(Parser* parser);

/**
 * Parse for statement
 * @param parser Parser state
 * @return Index of for node, or 0 on error
 */
uint16_t parse_for(Parser* parser);

/**
 * Parse return statement
 * @param parser Parser state
 * @return Index of return node, or 0 on error
 */
uint16_t parse_return(Parser* parser);

/**
 * Parse expression statement
 * @param parser Parser state
 * @return Index of expr_stmt node, or 0 on error
 */
uint16_t parse_expr_stmt(Parser* parser);

/**
 * Parse expression
 * @param parser Parser state
 * @return Index of expression node, or 0 on error
 */
uint16_t parse_expression(Parser* parser);

/**
 * Parse assignment expression
 * @param parser Parser state
 * @return Index of expression node, or 0 on error
 */
uint16_t parse_assignment(Parser* parser);

/**
 * Parse logical OR expression
 * @param parser Parser state
 * @return Index of expression node, or 0 on error
 */
uint16_t parse_logical_or(Parser* parser);

/**
 * Parse logical AND expression
 * @param parser Parser state
 * @return Index of expression node, or 0 on error
 */
uint16_t parse_logical_and(Parser* parser);

/**
 * Parse equality expression
 * @param parser Parser state
 * @return Index of expression node, or 0 on error
 */
uint16_t parse_equality(Parser* parser);

/**
 * Parse relational expression
 * @param parser Parser state
 * @return Index of expression node, or 0 on error
 */
uint16_t parse_relational(Parser* parser);

/**
 * Parse additive expression
 * @param parser Parser state
 * @return Index of expression node, or 0 on error
 */
uint16_t parse_additive(Parser* parser);

/**
 * Parse multiplicative expression
 * @param parser Parser state
 * @return Index of expression node, or 0 on error
 */
uint16_t parse_multiplicative(Parser* parser);

/**
 * Parse unary expression
 * @param parser Parser state
 * @return Index of expression node, or 0 on error
 */
uint16_t parse_unary(Parser* parser);

/**
 * Parse postfix expression
 * @param parser Parser state
 * @return Index of expression node, or 0 on error
 */
uint16_t parse_postfix(Parser* parser);

/**
 * Parse primary expression
 * @param parser Parser state
 * @return Index of expression node, or 0 on error
 */
uint16_t parse_primary(Parser* parser);

/**
 * Parse type
 * @param parser Parser state
 * @param type Output type info
 * @return 0 on success, -1 on error
 */
int parse_type(Parser* parser, TypeInfo* type);

/* Token manipulation */

/**
 * Read next token
 * @param parser Parser state
 * @return 0 on success, -1 on error
 */
int parser_next_token(Parser* parser);

/**
 * Check if current token matches expected type
 * @param parser Parser state
 * @param type Expected token type
 * @return 1 if matches, 0 otherwise
 */
int parser_match(Parser* parser, TokenType type);

/**
 * Consume current token if it matches expected type
 * @param parser Parser state
 * @param type Expected token type
 * @return 1 if consumed, 0 otherwise
 */
int parser_consume(Parser* parser, TokenType type);

/**
 * Expect current token to match type and consume it
 * @param parser Parser state
 * @param type Expected token type
 * @return 0 on success, -1 on error
 */
int parser_expect(Parser* parser, TokenType type);

/* Error handling */

/**
 * Report parse error
 * @param parser Parser state
 * @param message Error message
 */
void parser_error(Parser* parser, const char* message);

/* Debug helpers */

/**
 * Get node type name for debugging
 * @param type Node type
 * @return String representation of node type
 */
const char* node_type_name(NodeType type);

/**
 * Get binary operator name for debugging
 * @param op Binary operator
 * @return String representation of operator
 */
const char* binary_op_name(BinaryOp op);

/**
 * Get unary operator name for debugging
 * @param op Unary operator
 * @return String representation of operator
 */
void parser_error(Parser* parser, const char* message);

/**
 * Synchronize parser after error
 * @param parser Parser state
 */
void parser_synchronize(Parser* parser);

#endif /* PARSER_H */


