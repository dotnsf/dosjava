#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "ast.h"
#include "../../src/types.h"
#include <stdio.h>

/**
 * DOS Java Compiler - Symbol Table
 * 
 * Manages symbols (classes, methods, fields, variables) during semantic analysis.
 * Supports nested scopes and type information.
 * Optimized for 16-bit DOS environment with limited memory.
 */

/* Symbol kinds */
typedef enum {
    SYM_CLASS,      /* Class symbol */
    SYM_METHOD,     /* Method symbol */
    SYM_FIELD,      /* Field symbol */
    SYM_LOCAL,      /* Local variable */
    SYM_PARAM       /* Method parameter */
} SymbolKind;

/* Symbol entry */
typedef struct {
    SymbolKind kind;
    uint16_t name_offset;       /* Offset in string pool */
    TypeInfo type;              /* Symbol type */
    uint16_t scope_level;       /* Scope nesting level (0 = global) */
    
    /* Kind-specific data */
    union {
        /* Class data */
        struct {
            uint16_t member_count;  /* Number of members (methods + fields) */
        } class_data;
        
        /* Method data */
        struct {
            uint16_t param_count;   /* Number of parameters */
            uint16_t local_count;   /* Number of local variables */
            uint16_t is_static;     /* 1 if static, 0 otherwise */
            uint16_t is_public;     /* 1 if public, 0 otherwise */
        } method_data;
        
        /* Field data */
        struct {
            uint16_t is_static;     /* 1 if static, 0 otherwise */
            uint16_t is_public;     /* 1 if public, 0 otherwise */
        } field_data;
        
        /* Local variable data */
        struct {
            uint16_t index;         /* Local variable index */
        } local_data;
        
        /* Parameter data */
        struct {
            uint16_t index;         /* Parameter index */
        } param_data;
    } data;
} Symbol;

/* Symbol table */
typedef struct {
    Symbol symbols[256];        /* Symbol entries (8KB) */
    uint16_t symbol_count;      /* Number of symbols */
    
    uint16_t scope_stack[16];   /* Scope start indices (512 bytes) */
    uint16_t scope_level;       /* Current scope level (0 = global) */
    
    char string_pool[2048];     /* Shared string pool (2KB) */
    uint16_t pool_size;         /* Current pool size */
} SymbolTable;

/**
 * Initialize symbol table
 * @param table Symbol table to initialize
 * @return 0 on success, -1 on error
 */
int symtable_init(SymbolTable* table);

/**
 * Cleanup symbol table
 * @param table Symbol table to cleanup
 */
void symtable_cleanup(SymbolTable* table);

/**
 * Enter new scope
 * @param table Symbol table
 * @return 0 on success, -1 on error (max scope depth exceeded)
 */
int symtable_enter_scope(SymbolTable* table);

/**
 * Exit current scope (removes all symbols in current scope)
 * @param table Symbol table
 * @return 0 on success, -1 on error (already at global scope)
 */
int symtable_exit_scope(SymbolTable* table);

/**
 * Add symbol to table
 * @param table Symbol table
 * @param sym Symbol to add (will be copied)
 * @return Symbol index on success, 0xFFFF on error
 */
uint16_t symtable_add_symbol(SymbolTable* table, const Symbol* sym);

/**
 * Lookup symbol by name (searches from current scope to global)
 * @param table Symbol table
 * @param name Symbol name
 * @return Pointer to symbol, or NULL if not found
 */
Symbol* symtable_lookup(SymbolTable* table, const char* name);

/**
 * Lookup symbol in specific scope only
 * @param table Symbol table
 * @param name Symbol name
 * @param scope Scope level to search
 * @return Pointer to symbol, or NULL if not found
 */
Symbol* symtable_lookup_in_scope(SymbolTable* table, const char* name, uint16_t scope);

/**
 * Check if symbol exists in current scope (for duplicate detection)
 * @param table Symbol table
 * @param name Symbol name
 * @return 1 if exists, 0 otherwise
 */
int symtable_exists_in_current_scope(SymbolTable* table, const char* name);

/**
 * Add string to string pool
 * @param table Symbol table
 * @param str String to add
 * @return Offset in string pool, or 0xFFFF on error
 */
uint16_t symtable_add_string(SymbolTable* table, const char* str);

/**
 * Get string from string pool
 * @param table Symbol table
 * @param offset Offset in string pool
 * @return Pointer to string, or NULL on error
 */
const char* symtable_get_string(SymbolTable* table, uint16_t offset);

/**
 * Write symbol table to file
 * @param table Symbol table
 * @param filename Output file path
 * @return 0 on success, -1 on error
 */
int symtable_write(SymbolTable* table, const char* filename);

/**
 * Read symbol table from file
 * @param table Symbol table
 * @param filename Input file path
 * @return 0 on success, -1 on error
 */
int symtable_read(SymbolTable* table, const char* filename);

/**
 * Get symbol kind name (for debugging)
 * @param kind Symbol kind
 * @return String representation of symbol kind
 */
const char* symbol_kind_name(SymbolKind kind);

/**
 * Print symbol table (for debugging)
 * @param table Symbol table
 */
void symtable_print(SymbolTable* table);

#endif /* SYMTABLE_H */

// Made with Bob
