#ifndef AST_H
#define AST_H

#include "../../src/types.h"

/**
 * DOS Java Compiler - Abstract Syntax Tree Definitions
 * 
 * Defines AST node types and structures for the parser.
 * Optimized for 16-bit DOS environment with minimal memory footprint.
 */

/* AST Node Types */
typedef enum {
    /* Program structure */
    NODE_PROGRAM,           /* Root node */
    NODE_CLASS,             /* Class declaration */
    NODE_METHOD,            /* Method declaration */
    NODE_FIELD,             /* Field declaration */
    NODE_PARAM,             /* Method parameter */
    
    /* Statements */
    NODE_BLOCK,             /* Statement block { ... } */
    NODE_VAR_DECL,          /* Variable declaration */
    NODE_IF,                /* If statement */
    NODE_WHILE,             /* While loop */
    NODE_FOR,               /* For loop */
    NODE_RETURN,            /* Return statement */
    NODE_EXPR_STMT,         /* Expression statement */
    
    /* Expressions */
    NODE_ASSIGN,            /* Assignment = */
    NODE_BINARY_OP,         /* Binary operation */
    NODE_UNARY_OP,          /* Unary operation */
    NODE_CALL,              /* Method call */
    NODE_NEW,               /* Object creation */
    NODE_FIELD_ACCESS,      /* Field access obj.field */
    NODE_ARRAY_ACCESS,      /* Array access arr[i] */
    
    /* Literals and identifiers */
    NODE_LITERAL_INT,       /* Integer literal */
    NODE_LITERAL_BOOL,      /* Boolean literal */
    NODE_LITERAL_STRING,    /* String literal */
    NODE_IDENTIFIER,        /* Variable/field reference */
    NODE_THIS               /* this keyword */
} NodeType;

/* Binary operators */
typedef enum {
    OP_ADD,         /* + */
    OP_SUB,         /* - */
    OP_MUL,         /* * */
    OP_DIV,         /* / */
    OP_MOD,         /* % */
    OP_EQ,          /* == */
    OP_NE,          /* != */
    OP_LT,          /* < */
    OP_LE,          /* <= */
    OP_GT,          /* > */
    OP_GE,          /* >= */
    OP_AND,         /* && */
    OP_OR           /* || */
} BinaryOp;

/* Unary operators */
typedef enum {
    UOP_NEG,        /* - (negation) */
    UOP_NOT         /* ! (logical not) */
} UnaryOp;

/* Type kinds */
typedef enum {
    TYPE_VOID,
    TYPE_INT,
    TYPE_BOOLEAN,
    TYPE_CLASS
} TypeKind;

/* Type information */
typedef struct {
    TypeKind kind;
    uint16_t class_name;    /* Offset in string pool for TYPE_CLASS */
} TypeInfo;

/* AST Node structure */
typedef struct ASTNode {
    NodeType type;
    uint16_t line;
    uint16_t column;
    
    /* Node-specific data */
    union {
        /* Program */
        struct {
            uint16_t class_node;    /* Index of class node */
        } program;
        
        /* Class */
        struct {
            uint16_t name;          /* Offset in string pool */
            uint16_t member_count;
            uint16_t first_member;  /* Index of first member */
        } class_decl;
        
        /* Method */
        struct {
            uint16_t name;          /* Offset in string pool */
            TypeInfo return_type;
            uint16_t is_static;
            uint16_t is_public;
            uint16_t param_count;
            uint16_t first_param;   /* Index of first parameter */
            uint16_t body;          /* Index of body block */
        } method;
        
        /* Field */
        struct {
            uint16_t name;          /* Offset in string pool */
            TypeInfo type;
            uint16_t is_static;
            uint16_t is_public;
        } field;
        
        /* Parameter */
        struct {
            uint16_t name;          /* Offset in string pool */
            TypeInfo type;
        } param;
        
        /* Block */
        struct {
            uint16_t stmt_count;
            uint16_t first_stmt;    /* Index of first statement */
        } block;
        
        /* Variable declaration */
        struct {
            uint16_t name;          /* Offset in string pool */
            TypeInfo type;
            uint16_t init_expr;     /* Index of initializer (0 if none) */
        } var_decl;
        
        /* If statement */
        struct {
            uint16_t condition;     /* Index of condition expression */
            uint16_t then_stmt;     /* Index of then statement */
            uint16_t else_stmt;     /* Index of else statement (0 if none) */
        } if_stmt;
        
        /* While statement */
        struct {
            uint16_t condition;     /* Index of condition expression */
            uint16_t body;          /* Index of body statement */
        } while_stmt;
        
        /* For statement */
        struct {
            uint16_t init;          /* Index of init statement */
            uint16_t condition;     /* Index of condition expression */
            uint16_t update;        /* Index of update expression */
            uint16_t body;          /* Index of body statement */
        } for_stmt;
        
        /* Return statement */
        struct {
            uint16_t expr;          /* Index of return expression (0 if void) */
        } return_stmt;
        
        /* Expression statement */
        struct {
            uint16_t expr;          /* Index of expression */
        } expr_stmt;
        
        /* Assignment */
        struct {
            uint16_t target;        /* Index of target (identifier/field access) */
            uint16_t value;         /* Index of value expression */
        } assign;
        
        /* Binary operation */
        struct {
            BinaryOp op;
            uint16_t left;          /* Index of left operand */
            uint16_t right;         /* Index of right operand */
        } binary_op;
        
        /* Unary operation */
        struct {
            UnaryOp op;
            uint16_t operand;       /* Index of operand */
        } unary_op;
        
        /* Method call */
        struct {
            uint16_t object;        /* Index of object (0 for static) */
            uint16_t method_name;   /* Offset in string pool */
            uint16_t arg_count;
            uint16_t first_arg;     /* Index of first argument */
        } call;
        
        /* New expression */
        struct {
            uint16_t class_name;    /* Offset in string pool */
        } new_expr;
        
        /* Field access */
        struct {
            uint16_t object;        /* Index of object expression */
            uint16_t field_name;    /* Offset in string pool */
        } field_access;
        
        /* Array access */
        struct {
            uint16_t array;         /* Index of array expression */
            uint16_t index;         /* Index of index expression */
        } array_access;
        
        /* Literals */
        struct {
            int16_t int_value;
        } literal_int;
        
        struct {
            uint16_t bool_value;    /* 0 = false, 1 = true */
        } literal_bool;
        
        struct {
            uint16_t str_offset;    /* Offset in string pool */
        } literal_string;
        
        /* Identifier */
        struct {
            uint16_t name;          /* Offset in string pool */
        } identifier;
    } data;
    
    /* Next sibling (for lists) */
    uint16_t next_sibling;
} ASTNode;

/**
 * Get node type name (for debugging)
 * @param type Node type
 * @return String name of node type
 */
const char* node_type_name(NodeType type);

/**
 * Get binary operator name (for debugging)
 * @param op Binary operator
 * @return String name of operator
 */
const char* binary_op_name(BinaryOp op);

/**
 * Get unary operator name (for debugging)
 * @param op Unary operator
 * @return String name of operator
 */
const char* unary_op_name(UnaryOp op);

/**
 * Get type kind name (for debugging)
 * @param kind Type kind
 * @return String name of type
 */
const char* type_kind_name(TypeKind kind);

#endif /* AST_H */

// Made with Bob
