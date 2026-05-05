#ifndef CODEGEN_H
#define CODEGEN_H

#include "semantic.h"
#include "symtable.h"
#include "ast.h"
#include "../../src/format/djc.h"
#include "../../src/format/opcodes.h"
#include <stdio.h>

/**
 * DOS Java Compiler - Code Generator
 * 
 * Generates .djc bytecode from AST and symbol table.
 * Uses stack-based code generation with label backpatching.
 * Optimized for 16-bit DOS environment.
 */

/* Label for jump instructions */
typedef struct {
    uint16_t offset;        /* Offset in bytecode where label is referenced */
    int16_t target;         /* Target offset (-1 if not resolved) */
} Label;

/* Label list */
typedef struct {
    Label labels[128];      /* Label array */
    uint16_t count;         /* Number of labels */
} LabelList;

/* Bytecode buffer */
typedef struct {
    uint8_t* data;          /* Bytecode data (allocated separately) */
    uint16_t size;          /* Current size */
    uint16_t capacity;      /* Buffer capacity */
} ByteBuffer;

/* Constant pool builder */
typedef struct {
    DJCConstant constants[256];  /* Constant entries */
    uint16_t count;              /* Number of constants */
    char string_data[4096];      /* String storage */
    uint16_t string_size;        /* String data size */
} ConstantPool;

/* Code generation context (per method) */
typedef struct {
    uint16_t max_stack;     /* Maximum stack depth */
    uint16_t max_locals;    /* Maximum local variables */
    uint16_t current_stack; /* Current stack depth */
    ByteBuffer* code;       /* Bytecode buffer */
    LabelList* labels;      /* Label list */
} CodeGenContext;

/* Code generator state */
typedef struct {
    /* Input */
    FILE* ast_file;         /* AST input file */
    ASTNode nodes[128];     /* Node buffer */
    uint16_t node_count;
    uint16_t total_nodes;
    
    /* Symbol table */
    SymbolTable* symtable;
    
    /* Output */
    FILE* output_file;      /* .djc output file */
    
    /* Code generation */
    ConstantPool* constants;    /* Constant pool */
    DJCMethod methods[64];      /* Method descriptors */
    uint16_t method_count;
    DJCField fields[64];        /* Field descriptors */
    uint16_t field_count;
    ByteBuffer* bytecode;       /* All bytecode */
    
    /* Current context */
    CodeGenContext* context;    /* Current method context */
    Symbol* current_class;
    Symbol* current_method;
    
    /* String pool (shared) */
    char string_pool[2048];
    uint16_t pool_size;
    
    /* Error tracking */
    int has_error;
    uint16_t error_count;
} CodeGenerator;

/**
 * Initialize code generator
 * @param codegen Code generator state
 * @param ast_file AST input file path
 * @param symbol_file Symbol table input file path
 * @param output_file .djc output file path
 * @return 0 on success, -1 on error
 */
int codegen_init(CodeGenerator* codegen, const char* ast_file, const char* symbol_file, const char* output_file);

/**
 * Cleanup code generator and close files
 * @param codegen Code generator state
 */
void codegen_cleanup(CodeGenerator* codegen);

/**
 * Generate .djc bytecode
 * @param codegen Code generator state
 * @return 0 on success, -1 on error
 */
int codegen_generate(CodeGenerator* codegen);

/**
 * Generate code for class
 * @param codegen Code generator state
 * @param class_node Class AST node
 * @return 0 on success, -1 on error
 */
int generate_class(CodeGenerator* codegen, ASTNode* class_node);

/**
 * Generate code for method
 * @param codegen Code generator state
 * @param method_node Method AST node
 * @return 0 on success, -1 on error
 */
int generate_method(CodeGenerator* codegen, ASTNode* method_node);

/**
 * Generate code for statement
 * @param codegen Code generator state
 * @param stmt_node Statement AST node
 * @return 0 on success, -1 on error
 */
int generate_statement(CodeGenerator* codegen, ASTNode* stmt_node);

/**
 * Generate code for block
 * @param codegen Code generator state
 * @param block_node Block AST node
 * @return 0 on success, -1 on error
 */
int generate_block(CodeGenerator* codegen, ASTNode* block_node);

/**
 * Generate code for variable declaration
 * @param codegen Code generator state
 * @param var_node Variable declaration AST node
 * @return 0 on success, -1 on error
 */
int generate_var_decl(CodeGenerator* codegen, ASTNode* var_node);

/**
 * Generate code for if statement
 * @param codegen Code generator state
 * @param if_node If statement AST node
 * @return 0 on success, -1 on error
 */
int generate_if_stmt(CodeGenerator* codegen, ASTNode* if_node);

/**
 * Generate code for while statement
 * @param codegen Code generator state
 * @param while_node While statement AST node
 * @return 0 on success, -1 on error
 */
int generate_while_stmt(CodeGenerator* codegen, ASTNode* while_node);

/**
 * Generate code for for statement
 * @param codegen Code generator
 * @param for_node For statement node
 * @return 0 on success, -1 on error
 */
int generate_for_stmt(CodeGenerator* codegen, ASTNode* for_node);

/**
 * Generate code for return statement
 * @param codegen Code generator state
 * @param return_node Return statement AST node
 * @return 0 on success, -1 on error
 */
int generate_return_stmt(CodeGenerator* codegen, ASTNode* return_node);

/**
 * Generate code for expression
 * @param codegen Code generator state
 * @param expr_node Expression AST node
 * @return 0 on success, -1 on error
 */
int generate_expression(CodeGenerator* codegen, ASTNode* expr_node);

/**
 * Generate code for binary operation
 * @param codegen Code generator state
 * @param binop_node Binary operation AST node
 * @return 0 on success, -1 on error
 */
int generate_binary_op(CodeGenerator* codegen, ASTNode* binop_node);

/**
 * Generate code for unary operation
 * @param codegen Code generator state
 * @param unop_node Unary operation AST node
 * @return 0 on success, -1 on error
 */
int generate_unary_op(CodeGenerator* codegen, ASTNode* unop_node);

/**
 * Generate code for postfix operation
 * @param codegen Code generator
 * @param postop_node Postfix operation node
 * @return 0 on success, -1 on error
 */
int generate_postfix_op(CodeGenerator* codegen, ASTNode* postop_node);

/**
 * Generate code for assignment
 * @param codegen Code generator state
 * @param assign_node Assignment AST node
 * @return 0 on success, -1 on error
 */
int generate_assignment(CodeGenerator* codegen, ASTNode* assign_node);

/**
 * Generate code for identifier (load variable)
 * @param codegen Code generator state
 * @param id_node Identifier AST node
 * @return 0 on success, -1 on error
 */
int generate_identifier(CodeGenerator* codegen, ASTNode* id_node);

/**
 * Generate code for method call
 * @param codegen Code generator state
 * @param call_node Method call AST node
 * @return 0 on success, -1 on error
 */
int generate_method_call(CodeGenerator* codegen, ASTNode* call_node);

/**
 * Find or create method index
 * @param codegen Code generator state
 * @param method_name Method name
 * @param is_native 1 if native method, 0 otherwise
 * @return Method index, or 0xFFFF on error
 */
uint16_t find_method_index(CodeGenerator* codegen, const char* method_name, int is_native);
uint16_t build_method_descriptor(CodeGenerator* codegen, Symbol* method_sym);

/* Bytecode emission */

/**
 * Emit single byte opcode
 * @param codegen Code generator state
 * @param opcode Opcode to emit
 * @return 0 on success, -1 on error
 */
int emit_opcode(CodeGenerator* codegen, uint8_t opcode);

/**
 * Emit single byte
 * @param codegen Code generator state
 * @param value Byte value
 * @return 0 on success, -1 on error
 */
int emit_u1(CodeGenerator* codegen, uint8_t value);

/**
 * Emit 16-bit word
 * @param codegen Code generator state
 * @param value Word value
 * @return 0 on success, -1 on error
 */
int emit_u2(CodeGenerator* codegen, uint16_t value);

/* Constant pool operations */

/**
 * Add UTF8 string to constant pool
 * @param codegen Code generator state
 * @param str String to add
 * @return Constant index, or 0xFFFF on error
 */
uint16_t add_utf8_constant(CodeGenerator* codegen, const char* str);

/**
 * Add integer constant to constant pool
 * @param codegen Code generator state
 * @param value Integer value
 * @return Constant index, or 0xFFFF on error
 */
uint16_t add_int_constant(CodeGenerator* codegen, int16_t value);

/**
 * Find or add UTF8 constant
 * @param codegen Code generator state
 * @param str String to find/add
 * @return Constant index, or 0xFFFF on error
 */
uint16_t find_or_add_utf8(CodeGenerator* codegen, const char* str);

/* Label operations */

/**
 * Create new label
 * @param codegen Code generator state
 * @return Label index, or 0xFFFF on error
 */
uint16_t create_label(CodeGenerator* codegen);

/**
 * Mark label position (resolve label)
 * @param codegen Code generator state
 * @param label_index Label index
 * @return 0 on success, -1 on error
 */
int emit_label(CodeGenerator* codegen, uint16_t label_index);

/**
 * Emit jump instruction with label
 * @param codegen Code generator state
 * @param opcode Jump opcode
 * @param label_index Label index
 * @return 0 on success, -1 on error
 */
int emit_jump(CodeGenerator* codegen, uint8_t opcode, uint16_t label_index);

/**
 * Backpatch all unresolved labels
 * @param codegen Code generator state
 * @return 0 on success, -1 on error
 */
int backpatch_labels(CodeGenerator* codegen);

/* Helper functions */

/**
 * Get local variable index from symbol
 * @param codegen Code generator state
 * @param name Variable name
 * @return Local index, or 0xFFFF if not found
 */
uint16_t get_local_index(CodeGenerator* codegen, const char* name);

/**
 * Update stack depth
 * @param codegen Code generator state
 * @param delta Stack change (+1 for push, -1 for pop)
 */
void update_stack(CodeGenerator* codegen, int16_t delta);

/**
 * Get AST node
 * @param codegen Code generator state
 * @param node_index Node index (1-based)
 * @return Pointer to node, or NULL on error
 */
ASTNode* codegen_get_node(CodeGenerator* codegen, uint16_t node_index);

/**
 * Get string from string pool
 * @param codegen Code generator state
 * @param offset Offset in string pool
 * @return Pointer to string, or NULL on error
 */
const char* codegen_get_string(CodeGenerator* codegen, uint16_t offset);

/**
 * Write .djc file
 * @param codegen Code generator state
 * @return 0 on success, -1 on error
 */
int write_djc_file(CodeGenerator* codegen);

/* Error reporting */

/**
 * Report code generation error
 * @param codegen Code generator state
 * @param message Error message
 */
void codegen_error(CodeGenerator* codegen, const char* message);

#endif /* CODEGEN_H */


