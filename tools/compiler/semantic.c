#include "semantic.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/**
 * DOS Java Compiler - Semantic Analyzer Implementation
 * 
 * Performs two-pass semantic analysis:
 * Pass 1: Collect declarations
 * Pass 2: Type check and resolve identifiers
 */

/* Forward declarations */
static int read_ast_header(SemanticAnalyzer* analyzer);
static int load_string_pool(SemanticAnalyzer* analyzer);

/* Initialize semantic analyzer */
int semantic_init(SemanticAnalyzer* analyzer, const char* ast_file, const char* symbol_file) {
    if (!analyzer || !ast_file || !symbol_file) {
        return -1;
    }
    
    /* Clear structure */
    memset(analyzer, 0, sizeof(SemanticAnalyzer));
    
    /* Save symbol file path */
    analyzer->symbol_file = symbol_file;
    
    /* Open AST file */
    analyzer->ast_file = fopen(ast_file, "rb");
    if (!analyzer->ast_file) {
        return -1;
    }
    
    /* Read AST header and string pool */
    if (read_ast_header(analyzer) != 0) {
        fclose(analyzer->ast_file);
        return -1;
    }
    
    if (load_string_pool(analyzer) != 0) {
        fclose(analyzer->ast_file);
        return -1;
    }
    
    /* Allocate and initialize symbol table */
    analyzer->symtable = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!analyzer->symtable) {
        fclose(analyzer->ast_file);
        return -1;
    }
    
    if (symtable_init(analyzer->symtable) != 0) {
        free(analyzer->symtable);
        fclose(analyzer->ast_file);
        return -1;
    }
    
    /* Copy string pool to symbol table */
    memcpy(analyzer->symtable->string_pool, analyzer->string_pool, analyzer->pool_size);
    analyzer->symtable->pool_size = analyzer->pool_size;
    
    return 0;
}

/* Cleanup semantic analyzer */
void semantic_cleanup(SemanticAnalyzer* analyzer) {
    if (!analyzer) {
        return;
    }
    
    if (analyzer->ast_file) {
        fclose(analyzer->ast_file);
    }
    
    if (analyzer->symtable) {
        symtable_cleanup(analyzer->symtable);
        free(analyzer->symtable);
    }
    
    memset(analyzer, 0, sizeof(SemanticAnalyzer));
}

/* Read AST header */
static int read_ast_header(SemanticAnalyzer* analyzer) {
    uint16_t pool_size;
    
    /* Read total node count */
    if (fread(&analyzer->total_nodes, sizeof(uint16_t), 1, analyzer->ast_file) != 1) {
        return -1;
    }
    
    /* Read string pool size */
    if (fread(&pool_size, sizeof(uint16_t), 1, analyzer->ast_file) != 1) {
        return -1;
    }
    
    analyzer->pool_size = pool_size;
    return 0;
}

/* Load string pool */
static int load_string_pool(SemanticAnalyzer* analyzer) {
    if (analyzer->pool_size > 2048) {
        return -1;
    }
    
    if (analyzer->pool_size > 0) {
        if (fread(analyzer->string_pool, 1, analyzer->pool_size, analyzer->ast_file) != analyzer->pool_size) {
            return -1;
        }
    }
    
    return 0;
}

/* Get AST node */
ASTNode* semantic_get_node(SemanticAnalyzer* analyzer, uint16_t node_index) {
    long file_pos;
    
    if (!analyzer || node_index == 0 || node_index > analyzer->total_nodes) {
        return NULL;
    }
    
    /* Always read node from file to avoid pointer invalidation issues */
    file_pos = sizeof(uint16_t) * 2 + analyzer->pool_size + (node_index - 1) * sizeof(ASTNode);
    fseek(analyzer->ast_file, file_pos, SEEK_SET);
    
    if (fread(&analyzer->nodes[0], sizeof(ASTNode), 1, analyzer->ast_file) != 1) {
        return NULL;
    }
    
    analyzer->node_count = 1;
    return &analyzer->nodes[0];
}

/* Get string from pool */
const char* semantic_get_string(SemanticAnalyzer* analyzer, uint16_t offset) {
    if (!analyzer || offset >= analyzer->pool_size) {
        return NULL;
    }
    
    return &analyzer->string_pool[offset];
}

/* Add string to pool and return offset */
uint16_t semantic_add_string(SemanticAnalyzer* analyzer, const char* str) {
    uint16_t offset;
    uint16_t len;
    
    if (!analyzer || !str) {
        return 0xFFFF;
    }
    
    len = strlen(str) + 1;  /* Include null terminator */
    
    /* Check if string already exists in pool */
    for (offset = 0; offset < analyzer->pool_size; ) {
        if (strcmp(&analyzer->string_pool[offset], str) == 0) {
            return offset;  /* String already exists */
        }
        offset += strlen(&analyzer->string_pool[offset]) + 1;
    }
    
    /* Check if there's enough space */
    if (analyzer->pool_size + len > 2048) {
        return 0xFFFF;  /* Pool full */
    }
    
    /* Add string to pool */
    offset = analyzer->pool_size;
    strcpy(&analyzer->string_pool[offset], str);
    analyzer->pool_size += len;

    return offset;
}

/* Report semantic error */
void semantic_error(SemanticAnalyzer* analyzer, uint16_t line, uint16_t col, const char* message) {
    char buffer[256];
    int len;
    
    if (!analyzer || !message) {
        return;
    }
    
    /* Format error message */
    sprintf(buffer, "Error at line %u, col %u: %s\n", line, col, message);
    len = strlen(buffer);
    
    /* Add to error buffer if space available */
    if (analyzer->error_buf_size + len < 2048) {
        strcpy(&analyzer->error_buffer[analyzer->error_buf_size], buffer);
        analyzer->error_buf_size += len;
    }
    
    analyzer->has_error = 1;
    analyzer->error_count++;
}

/* Report error with node context */
void semantic_error_node(SemanticAnalyzer* analyzer, ASTNode* node, const char* message) {
    if (!analyzer || !node || !message) {
        return;
    }
    
    semantic_error(analyzer, node->line, node->column, message);
}

/* Print errors */
void semantic_print_errors(SemanticAnalyzer* analyzer) {
    if (!analyzer) {
        return;
    }
    
    if (analyzer->error_count > 0) {
        printf("%s", analyzer->error_buffer);
        printf("\nTotal errors: %u\n", analyzer->error_count);
    }
}

/* Perform semantic analysis */
int semantic_analyze(SemanticAnalyzer* analyzer) {
    if (!analyzer) {
        return -1;
    }
    
    /* Pass 1: Collect declarations */
    if (collect_declarations(analyzer) != 0) {
        return -1;
    }
    
    /* Pass 2: Type check and resolve */
    if (check_semantics(analyzer) != 0) {
        return -1;
    }
    
    /* Update symbol table string pool before writing */
    if (!analyzer->has_error) {
        /* Copy updated string pool to symbol table */
        memcpy(analyzer->symtable->string_pool, analyzer->string_pool, analyzer->pool_size);
        analyzer->symtable->pool_size = analyzer->pool_size;

    }
    
    /* Write symbol table to file if no errors */
    if (!analyzer->has_error && analyzer->symbol_file) {
        symtable_write(analyzer->symtable, analyzer->symbol_file);
    }
    
    return analyzer->has_error ? -1 : 0;
}

/* Pass 1: Collect declarations */
int collect_declarations(SemanticAnalyzer* analyzer) {
    ASTNode* root;
    ASTNode* class_node;
    
    if (!analyzer) {
        return -1;
    }
    
    /* Get root node (should be NODE_PROGRAM) */
    root = semantic_get_node(analyzer, 1);
    if (!root || root->type != NODE_PROGRAM) {
        semantic_error(analyzer, 0, 0, "Invalid AST: root node is not NODE_PROGRAM");
        return -1;
    }
    
    /* Get class node */
    class_node = semantic_get_node(analyzer, root->data.program.class_node);
    if (!class_node || class_node->type != NODE_CLASS) {
        semantic_error(analyzer, 0, 0, "Invalid AST: no class node");
        return -1;
    }
    
    /* Collect class symbols */
    if (collect_class_symbols(analyzer, class_node) != 0) {
        return -1;
    }
    
    return 0;
}

/* Collect class symbols */
int collect_class_symbols(SemanticAnalyzer* analyzer, ASTNode* class_node) {
    Symbol class_sym;
    const char* class_name;
    uint16_t member_idx;
    ASTNode* member_node;
    uint16_t member_count;
    
    if (!analyzer || !class_node) {
        return -1;
    }
    
    /* Get class name */
    class_name = semantic_get_string(analyzer, class_node->data.class_decl.name);
    if (!class_name) {
        semantic_error_node(analyzer, class_node, "Invalid class name");
        return -1;
    }
    
    /* Check for duplicate class */
    if (symtable_exists_in_current_scope(analyzer->symtable, class_name)) {
        semantic_error_node(analyzer, class_node, "Duplicate class declaration");
        return -1;
    }
    
    /* Create class symbol */
    memset(&class_sym, 0, sizeof(Symbol));
    class_sym.kind = SYM_CLASS;
    class_sym.name_offset = class_node->data.class_decl.name;
    class_sym.type.kind = TYPE_CLASS;
    class_sym.type.class_name = class_node->data.class_decl.name;
    class_sym.data.class_data.member_count = class_node->data.class_decl.member_count;
    
    /* Add class to symbol table */
    if (symtable_add_symbol(analyzer->symtable, &class_sym) == 0xFFFF) {
        semantic_error_node(analyzer, class_node, "Failed to add class symbol");
        return -1;
    }
    
    analyzer->current_class = symtable_lookup(analyzer->symtable, class_name);
    
    /* Enter class scope */
    symtable_enter_scope(analyzer->symtable);
    
    /* Collect member symbols */
    member_idx = class_node->data.class_decl.first_member;
    member_count = 0;
    
    while (member_idx != 0 && member_count < class_node->data.class_decl.member_count) {
        uint16_t next_member_idx;
        member_node = semantic_get_node(analyzer, member_idx);
        if (!member_node) {
            break;
        }
        
        /* Save next_sibling before any function calls that might invalidate the pointer */
        next_member_idx = member_node->next_sibling;
        
        if (member_node->type == NODE_METHOD) {
            if (collect_method_symbols(analyzer, member_node) != 0) {
                return -1;
            }
        } else if (member_node->type == NODE_FIELD) {
            if (collect_field_symbols(analyzer, member_node) != 0) {
                return -1;
            }
        }
        
        member_idx = next_member_idx;
        member_count++;
    }
    
    /* Exit class scope */
    symtable_exit_scope(analyzer->symtable);
    
    return 0;
}

/* Collect method symbols */
int collect_method_symbols(SemanticAnalyzer* analyzer, ASTNode* method_node) {
    Symbol method_sym;
    const char* method_name;
    uint16_t param_idx;
    ASTNode* param_node;
    uint16_t param_count;
    
    if (!analyzer || !method_node) {
        return -1;
    }
    
    /* Get method name */
    method_name = semantic_get_string(analyzer, method_node->data.method.name);
    if (!method_name) {
        semantic_error_node(analyzer, method_node, "Invalid method name");
        return -1;
    }
    
    /* Check for duplicate method */
    if (symtable_exists_in_current_scope(analyzer->symtable, method_name)) {
        semantic_error_node(analyzer, method_node, "Duplicate method declaration");
        return -1;
    }
    
    /* Create method symbol */
    memset(&method_sym, 0, sizeof(Symbol));
    method_sym.kind = SYM_METHOD;
    method_sym.name_offset = method_node->data.method.name;
    method_sym.type = method_node->data.method.return_type;
    method_sym.data.method_data.param_count = method_node->data.method.param_count;
    method_sym.data.method_data.is_static = method_node->data.method.is_static;
    method_sym.data.method_data.is_public = method_node->data.method.is_public;
    
    /* Add method to symbol table */
    if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
        semantic_error_node(analyzer, method_node, "Failed to add method symbol");
        return -1;
    }
    
    /* Enter method scope for parameters */
    symtable_enter_scope(analyzer->symtable);
    
    /* Collect parameter symbols */
    param_idx = method_node->data.method.first_param;
    param_count = 0;
    
    while (param_idx != 0 && param_count < method_node->data.method.param_count) {
        Symbol param_sym;
        const char* param_name;
        
        param_node = semantic_get_node(analyzer, param_idx);
        if (!param_node || param_node->type != NODE_PARAM) {
            break;
        }
        
        param_name = semantic_get_string(analyzer, param_node->data.param.name);
        if (!param_name) {
            semantic_error_node(analyzer, param_node, "Invalid parameter name");
            return -1;
        }
        
        /* Check for duplicate parameter */
        if (symtable_exists_in_current_scope(analyzer->symtable, param_name)) {
            semantic_error_node(analyzer, param_node, "Duplicate parameter name");
            return -1;
        }
        
        /* Create parameter symbol */
        memset(&param_sym, 0, sizeof(Symbol));
        param_sym.kind = SYM_PARAM;
        param_sym.name_offset = param_node->data.param.name;
        param_sym.type = param_node->data.param.type;
        param_sym.data.param_data.index = param_count;
        
        if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
            semantic_error_node(analyzer, param_node, "Failed to add parameter symbol");
            return -1;
        }
        
        param_idx = param_node->next_sibling;
        param_count++;
    }
    
    /* Exit method scope */
    symtable_exit_scope(analyzer->symtable);
    
    return 0;
}

/* Collect field symbols */
int collect_field_symbols(SemanticAnalyzer* analyzer, ASTNode* field_node) {
    Symbol field_sym;
    const char* field_name;
    
    if (!analyzer || !field_node) {
        return -1;
    }
    
    /* Get field name */
    field_name = semantic_get_string(analyzer, field_node->data.field.name);
    if (!field_name) {
        semantic_error_node(analyzer, field_node, "Invalid field name");
        return -1;
    }
    
    /* Check for duplicate field */
    if (symtable_exists_in_current_scope(analyzer->symtable, field_name)) {
        semantic_error_node(analyzer, field_node, "Duplicate field declaration");
        return -1;
    }
    
    /* Create field symbol */
    memset(&field_sym, 0, sizeof(Symbol));
    field_sym.kind = SYM_FIELD;
    field_sym.name_offset = field_node->data.field.name;
    field_sym.type = field_node->data.field.type;
    field_sym.data.field_data.is_static = field_node->data.field.is_static;
    field_sym.data.field_data.is_public = field_node->data.field.is_public;
    
    /* Add field to symbol table */
    if (symtable_add_symbol(analyzer->symtable, &field_sym) == 0xFFFF) {
        semantic_error_node(analyzer, field_node, "Failed to add field symbol");
        return -1;
    }
    
    return 0;
}

/* Pass 2: Type check and resolve */
int check_semantics(SemanticAnalyzer* analyzer) {
    ASTNode* root;
    ASTNode* class_node;
    uint16_t member_idx;
    ASTNode* member_node;
    uint16_t member_count;
    
    if (!analyzer) {
        return -1;
    }
    
    /* Get root and class nodes */
    root = semantic_get_node(analyzer, 1);
    if (!root) {
        return -1;
    }
    
    class_node = semantic_get_node(analyzer, root->data.program.class_node);
    if (!class_node) {
        return -1;
    }
    
    /* Enter class scope */
    symtable_enter_scope(analyzer->symtable);
    
    /* Check each method body */
    member_idx = class_node->data.class_decl.first_member;
    member_count = 0;
    
    while (member_idx != 0 && member_count < class_node->data.class_decl.member_count) {
        uint16_t next_member_idx;
        member_node = semantic_get_node(analyzer, member_idx);
        if (!member_node) {
            break;
        }
        
        /* Save next_sibling before any function calls */
        next_member_idx = member_node->next_sibling;
        
        if (member_node->type == NODE_METHOD) {
            if (check_method_body(analyzer, member_node) != 0) {
                /* Continue checking other methods */
            }
        }
        
        member_idx = next_member_idx;
        member_count++;
    }
    
    /* Exit class scope */
    symtable_exit_scope(analyzer->symtable);
    
    return 0;
}

/* Check method body */
int check_method_body(SemanticAnalyzer* analyzer, ASTNode* method_node) {
    const char* method_name;
    ASTNode* body_node;
    uint16_t param_idx;
    ASTNode* param_node;
    uint16_t param_count;
    
    if (!analyzer || !method_node) {
        return -1;
    }
    
    /* Get method symbol */
    method_name = semantic_get_string(analyzer, method_node->data.method.name);
    analyzer->current_method = symtable_lookup(analyzer->symtable, method_name);
    analyzer->expected_return = method_node->data.method.return_type;
    analyzer->has_return = 0;
    analyzer->next_local_index = 0;
    
    /* Enter method scope */
    symtable_enter_scope(analyzer->symtable);
    
    /* Re-add parameters to scope */
    param_idx = method_node->data.method.first_param;
    param_count = 0;
    
    while (param_idx != 0 && param_count < method_node->data.method.param_count) {
        Symbol param_sym;
        const char* param_name;
        
        param_node = semantic_get_node(analyzer, param_idx);
        if (!param_node) {
            break;
        }
        
        param_name = semantic_get_string(analyzer, param_node->data.param.name);
        
        memset(&param_sym, 0, sizeof(Symbol));
        param_sym.kind = SYM_PARAM;
        param_sym.name_offset = param_node->data.param.name;
        param_sym.type = param_node->data.param.type;
        param_sym.data.param_data.index = param_count;
        
        symtable_add_symbol(analyzer->symtable, &param_sym);
        
        param_idx = param_node->next_sibling;
        param_count++;
    }
    
    /* Check method body */
    body_node = semantic_get_node(analyzer, method_node->data.method.body);
    if (body_node) {
        check_block(analyzer, body_node);
    }
    
    /* Check return statement requirement */
    if (!is_void_type(analyzer->expected_return) && !analyzer->has_return) {
        semantic_error_node(analyzer, method_node, "Missing return statement in non-void method");
    }
    
    /* Exit method scope */
    symtable_exit_scope(analyzer->symtable);
    
    analyzer->current_method = NULL;
    
    return 0;
}

/* Check statement */
int check_statement(SemanticAnalyzer* analyzer, ASTNode* stmt_node, uint16_t stmt_idx) {
    if (!analyzer || !stmt_node) {
        return -1;
    }
    
    switch (stmt_node->type) {
        case NODE_BLOCK:
            return check_block(analyzer, stmt_node);
        
        case NODE_VAR_DECL:
            return check_var_decl(analyzer, stmt_node);
        
        case NODE_IF:
            return check_if_stmt(analyzer, stmt_node);
        
        case NODE_WHILE:
            return check_while_stmt(analyzer, stmt_node);
        
        case NODE_FOR:
            return check_for_stmt(analyzer, stmt_node);
        
        case NODE_RETURN:
            return check_return_stmt_idx(analyzer, stmt_idx);
        
        case NODE_EXPR_STMT: {
            TypeInfo expr_type = {0};
            ASTNode* expr_node = semantic_get_node(analyzer, stmt_node->data.expr_stmt.expr);
            if (expr_node) {
                return check_expression(analyzer, expr_node, &expr_type);
            }
            return 0;
        }
        
        default:
            semantic_error_node(analyzer, stmt_node, "Unknown statement type");
            return -1;
    }
}

/* Check block */
int check_block(SemanticAnalyzer* analyzer, ASTNode* block_node) {
    uint16_t stmt_idx;
    ASTNode* stmt_node;
    uint16_t stmt_count;
    uint16_t current_stmt_idx;
    uint16_t next_idx;
    uint16_t expected_stmt_count;
    
    if (!analyzer || !block_node) {
        return -1;
    }
    
    /* Enter block scope */
    symtable_enter_scope(analyzer->symtable);
    
    /* CRITICAL: Save stmt_count BEFORE any semantic_get_node calls */
    expected_stmt_count = block_node->data.block.stmt_count;
    stmt_idx = block_node->data.block.first_stmt;
    stmt_count = 0;
    
    while (stmt_idx != 0 && stmt_count < expected_stmt_count) {
        current_stmt_idx = stmt_idx;
        stmt_node = semantic_get_node(analyzer, stmt_idx);
        if (!stmt_node) {
            break;
        }
        
        next_idx = stmt_node->next_sibling;
        check_statement(analyzer, stmt_node, current_stmt_idx);
        
        stmt_idx = next_idx;
        stmt_count++;
    }
    
    /* Exit block scope */
    symtable_exit_scope(analyzer->symtable);
    
    return 0;
}

/* Check variable declaration */
int check_var_decl(SemanticAnalyzer* analyzer, ASTNode* var_node) {
    Symbol var_sym;
    const char* var_name;
    TypeInfo init_type = {0};
    TypeInfo var_type;  /* Copy of variable type to avoid buffer overwrite */
    ASTNode* init_expr;
    uint16_t init_expr_index;
    
    if (!analyzer || !var_node) {
        return -1;
    }
    
    /* Save variable type before any other node reads */
    var_type = var_node->data.var_decl.type;
    init_expr_index = var_node->data.var_decl.init_expr;
    
    /* Get variable name */
    var_name = semantic_get_string(analyzer, var_node->data.var_decl.name);
    if (!var_name) {
        semantic_error_node(analyzer, var_node, "Invalid variable name");
        return -1;
    }
    
    /* Check for duplicate variable in current scope */
    if (symtable_exists_in_current_scope(analyzer->symtable, var_name)) {
        semantic_error_node(analyzer, var_node, "Duplicate variable declaration");
        return -1;
    }
    
    /* Check initializer if present */
    if (init_expr_index != 0) {
        init_expr = semantic_get_node(analyzer, init_expr_index);
        if (init_expr) {
            if (check_expression(analyzer, init_expr, &init_type) == 0) {
                /* Check type compatibility */
                if (!types_compatible(var_type, init_type)) {
                    semantic_error_node(analyzer, var_node, "Type mismatch in variable initialization");
                }
            }
        }
    }
    
    /* Create variable symbol */
    memset(&var_sym, 0, sizeof(Symbol));
    var_sym.kind = SYM_LOCAL;
    /* Add variable name to string pool and use the new offset */
    var_sym.name_offset = semantic_add_string(analyzer, var_name);
    if (var_sym.name_offset == 0xFFFF) {
        semantic_error_node(analyzer, var_node, "String pool full");
        return -1;
    }
    var_sym.type = var_type;  /* Use saved copy */
    var_sym.data.local_data.index = analyzer->next_local_index++;
    
    /* Add variable to symbol table */
    if (symtable_add_symbol(analyzer->symtable, &var_sym) == 0xFFFF) {
        semantic_error_node(analyzer, var_node, "Failed to add variable symbol");
        return -1;
    }

    return 0;
}

/* Check if statement */
int check_if_stmt(SemanticAnalyzer* analyzer, ASTNode* if_node) {
    TypeInfo cond_type = {0};
    ASTNode* cond_node;
    ASTNode* then_node;
    ASTNode* else_node;
    uint16_t cond_idx;
    uint16_t then_idx;
    uint16_t else_idx;
    
    if (!analyzer || !if_node) {
        return -1;
    }
    
    /* CRITICAL: Save all indices BEFORE any semantic_get_node calls */
    cond_idx = if_node->data.if_stmt.condition;
    then_idx = if_node->data.if_stmt.then_stmt;
    else_idx = if_node->data.if_stmt.else_stmt;
    
    /* Check condition */
    cond_node = semantic_get_node(analyzer, cond_idx);
    if (cond_node) {
        if (check_expression(analyzer, cond_node, &cond_type) == 0) {
            if (!is_boolean_type(cond_type)) {
                semantic_error_node(analyzer, if_node, "If condition must be boolean");
            }
        }
    }
    
    /* Check then branch */
    then_node = semantic_get_node(analyzer, then_idx);
    if (then_node) {
        check_statement(analyzer, then_node, then_idx);
    }
    
    /* Check else branch if present */
    if (else_idx != 0) {
        else_node = semantic_get_node(analyzer, else_idx);
        if (else_node) {
            check_statement(analyzer, else_node, else_idx);
        }
    }
    
    return 0;
}

/* Check while statement */
int check_while_stmt(SemanticAnalyzer* analyzer, ASTNode* while_node) {
    TypeInfo cond_type = {0};
    ASTNode* cond_node;
    ASTNode* body_node;
    uint16_t cond_idx;
    uint16_t body_idx;
    
    if (!analyzer || !while_node) {
        return -1;
    }
    
    /* CRITICAL: Save all indices BEFORE any semantic_get_node calls */
    cond_idx = while_node->data.while_stmt.condition;
    body_idx = while_node->data.while_stmt.body;
    
    /* Check condition */
    cond_node = semantic_get_node(analyzer, cond_idx);
    if (cond_node) {
        if (check_expression(analyzer, cond_node, &cond_type) == 0) {
            if (!is_boolean_type(cond_type)) {
                semantic_error_node(analyzer, while_node, "While condition must be boolean");
            }
        }
    }
    
    /* Check body */
    body_node = semantic_get_node(analyzer, body_idx);
    if (body_node) {
        check_statement(analyzer, body_node, body_idx);
    }
    
    return 0;
}

/* Check for statement */
int check_for_stmt(SemanticAnalyzer* analyzer, ASTNode* for_node) {
    TypeInfo cond_type = {0};
    TypeInfo init_type = {0};
    TypeInfo update_type = {0};
    ASTNode* init_node;
    ASTNode* cond_node;
    ASTNode* update_node;
    ASTNode* body_node;
    uint16_t init_idx;
    uint16_t cond_idx;
    uint16_t update_idx;
    uint16_t body_idx;
    
    if (!analyzer || !for_node) {
        return -1;
    }
    
    /* CRITICAL: Save all indices BEFORE any semantic_get_node calls */
    init_idx = for_node->data.for_stmt.init;
    cond_idx = for_node->data.for_stmt.condition;
    update_idx = for_node->data.for_stmt.update;
    body_idx = for_node->data.for_stmt.body;
    
    /* Check init (optional) */
    if (init_idx != 0) {
        init_node = semantic_get_node(analyzer, init_idx);
        if (init_node) {
            check_expression(analyzer, init_node, &init_type);
        }
    }
    
    /* Check condition (optional) */
    if (cond_idx != 0) {
        cond_node = semantic_get_node(analyzer, cond_idx);
        if (cond_node) {
            if (check_expression(analyzer, cond_node, &cond_type) == 0) {
                if (!is_boolean_type(cond_type)) {
                    semantic_error_node(analyzer, for_node, "For condition must be boolean");
                }
            }
        }
    }
    
    /* Check update (optional) */
    if (update_idx != 0) {
        update_node = semantic_get_node(analyzer, update_idx);
        if (update_node) {
            check_expression(analyzer, update_node, &update_type);
        }
    }
    
    /* Check body */
    body_node = semantic_get_node(analyzer, body_idx);
    if (body_node) {
        check_statement(analyzer, body_node, body_idx);
    }
    
    return 0;
}

/* Check return statement using node index */
int check_return_stmt_idx(SemanticAnalyzer* analyzer, uint16_t return_idx) {
    TypeInfo expr_type = {0};
    ASTNode* return_node;
    ASTNode* expr_node;
    uint16_t expr_idx;
    
    if (!analyzer || return_idx == 0) {
        return -1;
    }
    
    /* Get return node */
    return_node = semantic_get_node(analyzer, return_idx);
    if (!return_node) {
        return -1;
    }
    
    analyzer->has_return = 1;
    
    /* Save expression index before any semantic_get_node calls */
    expr_idx = return_node->data.return_stmt.expr;
    
    /* Check return expression */
    if (expr_idx != 0) {
        expr_node = semantic_get_node(analyzer, expr_idx);
        if (expr_node) {
            if (check_expression(analyzer, expr_node, &expr_type) == 0) {
                /* Re-fetch return node to check type compatibility */
                return_node = semantic_get_node(analyzer, return_idx);
                if (return_node) {
                    /* Check type compatibility with expected return type */
                    if (!types_compatible(analyzer->expected_return, expr_type)) {
                        semantic_error_node(analyzer, return_node, "Return type mismatch");
                    }
                }
            }
        }
    } else {
        /* Void return */
        if (!is_void_type(analyzer->expected_return)) {
            return_node = semantic_get_node(analyzer, return_idx);
            if (return_node) {
                semantic_error_node(analyzer, return_node, "Missing return value in non-void method");
            }
        }
    }
    
    return 0;
}

/* Check expression */
int check_expression(SemanticAnalyzer* analyzer, ASTNode* expr_node, TypeInfo* result_type) {
    if (!analyzer || !expr_node || !result_type) {
        return -1;
    }
    
    switch (expr_node->type) {
        case NODE_LITERAL_INT:
            result_type->kind = TYPE_INT;
            result_type->class_name = 0;
            return 0;
        
        case NODE_LITERAL_BOOL:
            result_type->kind = TYPE_BOOLEAN;
            result_type->class_name = 0;
            return 0;
        
        case NODE_LITERAL_STRING:
            /* String literals are treated as String type */
            result_type->kind = TYPE_CLASS;
            result_type->class_name = 0;  /* TODO: Add "String" to constant pool */
            return 0;
        
        case NODE_IDENTIFIER:
            return check_identifier(analyzer, expr_node, result_type);
        
        case NODE_BINARY_OP:
            return check_binary_op(analyzer, expr_node, result_type);
        
        case NODE_UNARY_OP:
            return check_unary_op(analyzer, expr_node, result_type);
        
        case NODE_POSTFIX_OP:
            return check_postfix_op(analyzer, expr_node, result_type);
        
        case NODE_ASSIGN:
            return check_assignment(analyzer, expr_node, result_type);
        
        case NODE_CALL: {
            /* Method calls return void for now */
            ASTNode* call_node = expr_node;
            const char* method_name = semantic_get_string(analyzer, call_node->data.call.method_name);
            uint16_t object_idx = call_node->data.call.object;
            result_type->kind = TYPE_VOID;
            result_type->class_name = 0;
            return 0;
        }
        
        default:
            semantic_error_node(analyzer, expr_node, "Unknown expression type");
            return -1;
    }
}

/* Check binary operation */
int check_binary_op(SemanticAnalyzer* analyzer, ASTNode* binop_node, TypeInfo* result_type) {
    TypeInfo left_type, right_type;
    ASTNode* left_node;
    ASTNode* right_node;
    uint16_t left_idx;
    uint16_t right_idx;
    uint16_t op;
    
    if (!analyzer || !binop_node || !result_type) {
        return -1;
    }
    
    /* CRITICAL: Save indices and operator BEFORE any semantic_get_node calls */
    left_idx = binop_node->data.binary_op.left;
    right_idx = binop_node->data.binary_op.right;
    op = binop_node->data.binary_op.op;
    
    /* Check left operand */
    left_node = semantic_get_node(analyzer, left_idx);
    if (!left_node || check_expression(analyzer, left_node, &left_type) != 0) {
        return -1;
    }
    
    /* Check right operand */
    right_node = semantic_get_node(analyzer, right_idx);
    if (!right_node || check_expression(analyzer, right_node, &right_type) != 0) {
        return -1;
    }
    
    /* Get result type */
    if (get_binary_op_result_type(op, left_type, right_type, result_type) != 0) {
        semantic_error_node(analyzer, binop_node, "Invalid operand types for binary operation");
        return -1;
    }
    
    return 0;
}

/* Check unary operation */
int check_unary_op(SemanticAnalyzer* analyzer, ASTNode* unop_node, TypeInfo* result_type) {
    TypeInfo operand_type = {0};
    ASTNode* operand_node;
    uint16_t operand_idx;
    uint16_t op;
    
    if (!analyzer || !unop_node || !result_type) {
        return -1;
    }
    
    /* CRITICAL: Save index and operator BEFORE any semantic_get_node calls */
    operand_idx = unop_node->data.unary_op.operand;
    op = unop_node->data.unary_op.op;
    
    /* Check operand */
    operand_node = semantic_get_node(analyzer, operand_idx);
    if (!operand_node || check_expression(analyzer, operand_node, &operand_type) != 0) {
        return -1;
    }
    
    /* Get result type */
    if (get_unary_op_result_type(op, operand_type, result_type) != 0) {
        semantic_error_node(analyzer, unop_node, "Invalid operand type for unary operation");
        return -1;
    }
    
    return 0;
}

/* Check postfix operation */
int check_postfix_op(SemanticAnalyzer* analyzer, ASTNode* postop_node, TypeInfo* result_type) {
    TypeInfo operand_type = {0};
    ASTNode* operand_node;
    uint16_t operand_idx;
    
    if (!analyzer || !postop_node || !result_type) {
        return -1;
    }
    
    /* CRITICAL: Save index BEFORE any semantic_get_node calls */
    operand_idx = postop_node->data.postfix_op.operand;
    
    /* Check operand */
    operand_node = semantic_get_node(analyzer, operand_idx);
    if (!operand_node || check_expression(analyzer, operand_node, &operand_type) != 0) {
        return -1;
    }
    
    /* Postfix operators require integer operand and return integer */
    if (operand_type.kind != TYPE_INT) {
        semantic_error_node(analyzer, postop_node, "Postfix operator requires integer operand");
        return -1;
    }
    
    result_type->kind = TYPE_INT;
    result_type->class_name = 0;
    
    return 0;
}

/* Check assignment */
int check_assignment(SemanticAnalyzer* analyzer, ASTNode* assign_node, TypeInfo* result_type) {
    TypeInfo target_type, value_type;
    ASTNode* target_node;
    ASTNode* value_node;
    uint16_t target_idx;
    uint16_t value_idx;
    
    if (!analyzer || !assign_node || !result_type) {
        return -1;
    }
    
    /* CRITICAL: Save indices BEFORE any semantic_get_node calls */
    target_idx = assign_node->data.assign.target;
    value_idx = assign_node->data.assign.value;
    
    /* Check target */
    target_node = semantic_get_node(analyzer, target_idx);
    if (!target_node || check_expression(analyzer, target_node, &target_type) != 0) {
        return -1;
    }
    
    /* Check value */
    value_node = semantic_get_node(analyzer, value_idx);
    if (!value_node || check_expression(analyzer, value_node, &value_type) != 0) {
        return -1;
    }
    
    /* Check type compatibility */
    if (!types_compatible(target_type, value_type)) {
        semantic_error_node(analyzer, assign_node, "Type mismatch in assignment");
        return -1;
    }
    
    *result_type = target_type;
    return 0;
}

/* Check identifier */
int check_identifier(SemanticAnalyzer* analyzer, ASTNode* id_node, TypeInfo* result_type) {
    const char* id_name;
    Symbol* sym;
    
    if (!analyzer || !id_node || !result_type) {
        return -1;
    }
    
    /* Get identifier name */
    id_name = semantic_get_string(analyzer, id_node->data.identifier.name);
    if (!id_name) {
        semantic_error_node(analyzer, id_node, "Invalid identifier");
        return -1;
    }
    
    /* Lookup symbol */
    sym = symtable_lookup(analyzer->symtable, id_name);
    if (!sym) {
        semantic_error_node(analyzer, id_node, "Undefined identifier");
        return -1;
    }
    
    *result_type = sym->type;
    return 0;
}

/* Type checking helpers */

int types_compatible(TypeInfo t1, TypeInfo t2) {
    if (t1.kind != t2.kind) {
        return 0;
    }
    
    if (t1.kind == TYPE_CLASS) {
        return t1.class_name == t2.class_name;
    }
    
    return 1;
}

int is_boolean_type(TypeInfo type) {
    return type.kind == TYPE_BOOLEAN;
}

int is_numeric_type(TypeInfo type) {
    return type.kind == TYPE_INT;
}

int is_void_type(TypeInfo type) {
    return type.kind == TYPE_VOID;
}

int get_binary_op_result_type(BinaryOp op, TypeInfo left_type, TypeInfo right_type, TypeInfo* result_type) {
    if (!result_type) {
        return -1;
    }
    
    /* Arithmetic operations: int op int -> int */
    if (op == BINOP_ADD || op == BINOP_SUB || op == BINOP_MUL || op == BINOP_DIV || op == BINOP_MOD) {
        if (is_numeric_type(left_type) && is_numeric_type(right_type)) {
            result_type->kind = TYPE_INT;
            result_type->class_name = 0;
            return 0;
        }
        return -1;
    }
    
    /* Relational operations: int op int -> boolean */
    if (op == BINOP_LT || op == BINOP_LE || op == BINOP_GT || op == BINOP_GE) {
        if (is_numeric_type(left_type) && is_numeric_type(right_type)) {
            result_type->kind = TYPE_BOOLEAN;
            result_type->class_name = 0;
            return 0;
        }
        return -1;
    }
    
    /* Equality operations: T op T -> boolean */
    if (op == BINOP_EQ || op == BINOP_NE) {
        if (types_compatible(left_type, right_type)) {
            result_type->kind = TYPE_BOOLEAN;
            result_type->class_name = 0;
            return 0;
        }
        return -1;
    }
    
    /* Logical operations: boolean op boolean -> boolean */
    if (op == BINOP_AND || op == BINOP_OR) {
        if (is_boolean_type(left_type) && is_boolean_type(right_type)) {
            result_type->kind = TYPE_BOOLEAN;
            result_type->class_name = 0;
            return 0;
        }
        return -1;
    }
    
    return -1;
}

int get_unary_op_result_type(UnaryOp op, TypeInfo operand_type, TypeInfo* result_type) {
    if (!result_type) {
        return -1;
    }
    
    /* Negation: -int -> int */
    if (op == UNOP_NEG) {
        if (is_numeric_type(operand_type)) {
            result_type->kind = TYPE_INT;
            result_type->class_name = 0;
            return 0;
        }
        return -1;
    }
    
    /* Logical not: !boolean -> boolean */
    if (op == UNOP_NOT) {
        if (is_boolean_type(operand_type)) {
            result_type->kind = TYPE_BOOLEAN;
            result_type->class_name = 0;
            return 0;
        }
        return -1;
    }
    
    return -1;
}


