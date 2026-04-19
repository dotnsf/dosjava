#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symtable.h"
#include "ast.h"
#include "parser.h"
#include <stdio.h>

/**
 * DOS Java Compiler - Semantic Analyzer
 * 
 * Performs semantic analysis on AST:
 * - Symbol table construction
 * - Type checking
 * - Identifier resolution
 * - Error detection
 * 
 * Uses two-pass approach:
 * Pass 1: Collect declarations (classes, methods, fields)
 * Pass 2: Type check and resolve identifiers
 */

/* Semantic analyzer state */
typedef struct {
    /* Input */
    FILE* ast_file;             /* AST input file */
    ASTNode nodes[128];         /* Node buffer (4KB) */
    uint16_t node_count;        /* Nodes in buffer */
    uint16_t total_nodes;       /* Total nodes read */
    
    /* String pool (shared with parser) */
    char string_pool[2048];
    uint16_t pool_size;
    
    /* Symbol table */
    SymbolTable* symtable;
    
    /* Current context */
    Symbol* current_class;      /* Current class being analyzed */
    Symbol* current_method;     /* Current method being analyzed */
    TypeInfo expected_return;   /* Expected return type for current method */
    int has_return;             /* 1 if current method has return statement */
    
    /* Error tracking */
    int has_error;
    uint16_t error_count;
    char error_buffer[2048];    /* Error message buffer (2KB) */
    uint16_t error_buf_size;
} SemanticAnalyzer;

/**
 * Initialize semantic analyzer
 * @param analyzer Semantic analyzer state
 * @param ast_file AST input file path
 * @param symbol_file Symbol table output file path
 * @return 0 on success, -1 on error
 */
int semantic_init(SemanticAnalyzer* analyzer, const char* ast_file, const char* symbol_file);

/**
 * Cleanup semantic analyzer and close files
 * @param analyzer Semantic analyzer state
 */
void semantic_cleanup(SemanticAnalyzer* analyzer);

/**
 * Perform semantic analysis (both passes)
 * @param analyzer Semantic analyzer state
 * @return 0 on success, -1 on error
 */
int semantic_analyze(SemanticAnalyzer* analyzer);

/**
 * Pass 1: Collect declarations
 * @param analyzer Semantic analyzer state
 * @return 0 on success, -1 on error
 */
int collect_declarations(SemanticAnalyzer* analyzer);

/**
 * Collect class symbols
 * @param analyzer Semantic analyzer state
 * @param class_node Class AST node
 * @return 0 on success, -1 on error
 */
int collect_class_symbols(SemanticAnalyzer* analyzer, ASTNode* class_node);

/**
 * Collect method symbols
 * @param analyzer Semantic analyzer state
 * @param method_node Method AST node
 * @return 0 on success, -1 on error
 */
int collect_method_symbols(SemanticAnalyzer* analyzer, ASTNode* method_node);

/**
 * Collect field symbols
 * @param analyzer Semantic analyzer state
 * @param field_node Field AST node
 * @return 0 on success, -1 on error
 */
int collect_field_symbols(SemanticAnalyzer* analyzer, ASTNode* field_node);

/**
 * Pass 2: Type check and resolve
 * @param analyzer Semantic analyzer state
 * @return 0 on success, -1 on error
 */
int check_semantics(SemanticAnalyzer* analyzer);

/**
 * Check method body
 * @param analyzer Semantic analyzer state
 * @param method_node Method AST node
 * @return 0 on success, -1 on error
 */
int check_method_body(SemanticAnalyzer* analyzer, ASTNode* method_node);

/**
 * Check statement
 * @param analyzer Semantic analyzer state
 * @param stmt_node Statement AST node
 * @return 0 on success, -1 on error
 */
int check_statement(SemanticAnalyzer* analyzer, ASTNode* stmt_node);

/**
 * Check block statement
 * @param analyzer Semantic analyzer state
 * @param block_node Block AST node
 * @return 0 on success, -1 on error
 */
int check_block(SemanticAnalyzer* analyzer, ASTNode* block_node);

/**
 * Check variable declaration
 * @param analyzer Semantic analyzer state
 * @param var_node Variable declaration AST node
 * @return 0 on success, -1 on error
 */
int check_var_decl(SemanticAnalyzer* analyzer, ASTNode* var_node);

/**
 * Check if statement
 * @param analyzer Semantic analyzer state
 * @param if_node If statement AST node
 * @return 0 on success, -1 on error
 */
int check_if_stmt(SemanticAnalyzer* analyzer, ASTNode* if_node);

/**
 * Check while statement
 * @param analyzer Semantic analyzer state
 * @param while_node While statement AST node
 * @return 0 on success, -1 on error
 */
int check_while_stmt(SemanticAnalyzer* analyzer, ASTNode* while_node);

/**
 * Check return statement
 * @param analyzer Semantic analyzer state
 * @param return_node Return statement AST node
 * @return 0 on success, -1 on error
 */
int check_return_stmt(SemanticAnalyzer* analyzer, ASTNode* return_node);

/**
 * Check expression and return its type
 * @param analyzer Semantic analyzer state
 * @param expr_node Expression AST node
 * @param result_type Output: expression type
 * @return 0 on success, -1 on error
 */
int check_expression(SemanticAnalyzer* analyzer, ASTNode* expr_node, TypeInfo* result_type);

/**
 * Check binary operation
 * @param analyzer Semantic analyzer state
 * @param binop_node Binary operation AST node
 * @param result_type Output: result type
 * @return 0 on success, -1 on error
 */
int check_binary_op(SemanticAnalyzer* analyzer, ASTNode* binop_node, TypeInfo* result_type);

/**
 * Check unary operation
 * @param analyzer Semantic analyzer state
 * @param unop_node Unary operation AST node
 * @param result_type Output: result type
 * @return 0 on success, -1 on error
 */
int check_unary_op(SemanticAnalyzer* analyzer, ASTNode* unop_node, TypeInfo* result_type);

/**
 * Check assignment
 * @param analyzer Semantic analyzer state
 * @param assign_node Assignment AST node
 * @param result_type Output: result type
 * @return 0 on success, -1 on error
 */
int check_assignment(SemanticAnalyzer* analyzer, ASTNode* assign_node, TypeInfo* result_type);

/**
 * Check identifier reference
 * @param analyzer Semantic analyzer state
 * @param id_node Identifier AST node
 * @param result_type Output: identifier type
 * @return 0 on success, -1 on error
 */
int check_identifier(SemanticAnalyzer* analyzer, ASTNode* id_node, TypeInfo* result_type);

/* Type checking helpers */

/**
 * Check if two types are compatible
 * @param t1 First type
 * @param t2 Second type
 * @return 1 if compatible, 0 otherwise
 */
int types_compatible(TypeInfo t1, TypeInfo t2);

/**
 * Check if type is boolean
 * @param type Type to check
 * @return 1 if boolean, 0 otherwise
 */
int is_boolean_type(TypeInfo type);

/**
 * Check if type is numeric (int)
 * @param type Type to check
 * @return 1 if numeric, 0 otherwise
 */
int is_numeric_type(TypeInfo type);

/**
 * Check if type is void
 * @param type Type to check
 * @return 1 if void, 0 otherwise
 */
int is_void_type(TypeInfo type);

/**
 * Get result type of binary operation
 * @param op Binary operator
 * @param left_type Left operand type
 * @param right_type Right operand type
 * @param result_type Output: result type
 * @return 0 on success, -1 if invalid operation
 */
int get_binary_op_result_type(BinaryOp op, TypeInfo left_type, TypeInfo right_type, TypeInfo* result_type);

/**
 * Get result type of unary operation
 * @param op Unary operator
 * @param operand_type Operand type
 * @param result_type Output: result type
 * @return 0 on success, -1 if invalid operation
 */
int get_unary_op_result_type(UnaryOp op, TypeInfo operand_type, TypeInfo* result_type);

/* AST node access */

/**
 * Read AST node from file
 * @param analyzer Semantic analyzer state
 * @param node_index Node index (1-based)
 * @return Pointer to node, or NULL on error
 */
ASTNode* semantic_get_node(SemanticAnalyzer* analyzer, uint16_t node_index);

/**
 * Get string from string pool
 * @param analyzer Semantic analyzer state
 * @param offset Offset in string pool
 * @return Pointer to string, or NULL on error
 */
const char* semantic_get_string(SemanticAnalyzer* analyzer, uint16_t offset);

/* Error reporting */

/**
 * Report semantic error
 * @param analyzer Semantic analyzer state
 * @param line Line number
 * @param col Column number
 * @param message Error message
 */
void semantic_error(SemanticAnalyzer* analyzer, uint16_t line, uint16_t col, const char* message);

/**
 * Report semantic error with node context
 * @param analyzer Semantic analyzer state
 * @param node AST node
 * @param message Error message
 */
void semantic_error_node(SemanticAnalyzer* analyzer, ASTNode* node, const char* message);

/**
 * Print all accumulated errors
 * @param analyzer Semantic analyzer state
 */
void semantic_print_errors(SemanticAnalyzer* analyzer);

#endif /* SEMANTIC_H */

// Made with Bob
