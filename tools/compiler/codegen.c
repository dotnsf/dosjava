#include "codegen.h"
#include <string.h>
#include <stdlib.h>

/**
 * DOS Java Compiler - Code Generator Implementation
 * 
 * Generates .djc bytecode from AST and symbol table.
 */

/* Forward declarations */
static int read_ast_header(CodeGenerator* codegen);
static int load_string_pool(CodeGenerator* codegen);
static int load_symbol_table(CodeGenerator* codegen, const char* symbol_file);

/* Initialize code generator */
int codegen_init(CodeGenerator* codegen, const char* ast_file, const char* symbol_file, const char* output_file) {
    if (!codegen || !ast_file || !symbol_file || !output_file) {
        return -1;
    }
    
    /* Clear structure */
    memset(codegen, 0, sizeof(CodeGenerator));
    /* Open AST file */
    codegen->ast_file = fopen(ast_file, "rb");
    if (!codegen->ast_file) {
        return -1;
    }
    
    /* Read AST header and string pool */
    if (read_ast_header(codegen) != 0) {
        fclose(codegen->ast_file);
        return -1;
    }
    
    if (load_string_pool(codegen) != 0) {
        fclose(codegen->ast_file);
        return -1;
    }
    
    /* Load symbol table */
    if (load_symbol_table(codegen, symbol_file) != 0) {
        fclose(codegen->ast_file);
        return -1;
    }
    
    /* Open output file */
    codegen->output_file = fopen(output_file, "wb");
    if (!codegen->output_file) {
        fclose(codegen->ast_file);
        symtable_cleanup(codegen->symtable);
        free(codegen->symtable);
        return -1;
    }
    
    /* Allocate constant pool */
    codegen->constants = (ConstantPool*)malloc(sizeof(ConstantPool));
    if (!codegen->constants) {
        fclose(codegen->ast_file);
        fclose(codegen->output_file);
        symtable_cleanup(codegen->symtable);
        free(codegen->symtable);
        return -1;
    }
    memset(codegen->constants, 0, sizeof(ConstantPool));
    
    /* Allocate bytecode buffer structure */
    codegen->bytecode = (ByteBuffer*)malloc(sizeof(ByteBuffer));
    if (!codegen->bytecode) {
        free(codegen->constants);
        fclose(codegen->ast_file);
        fclose(codegen->output_file);
        symtable_cleanup(codegen->symtable);
        free(codegen->symtable);
        return -1;
    }
    memset(codegen->bytecode, 0, sizeof(ByteBuffer));
    
    /* Allocate bytecode data separately (8KB - reduced for DOS memory constraints) */
    codegen->bytecode->capacity = 8192;
    codegen->bytecode->data = (uint8_t*)malloc(codegen->bytecode->capacity);
    if (!codegen->bytecode->data) {
        free(codegen->bytecode);
        free(codegen->constants);
        fclose(codegen->ast_file);
        fclose(codegen->output_file);
        symtable_cleanup(codegen->symtable);
        free(codegen->symtable);
        return -1;
    }
    codegen->bytecode->size = 0;
    return 0;
}

/* Cleanup code generator */
void codegen_cleanup(CodeGenerator* codegen) {
    if (!codegen) {
        return;
    }
    
    if (codegen->ast_file) {
        fclose(codegen->ast_file);
    }
    
    if (codegen->output_file) {
        fclose(codegen->output_file);
    }
    
    if (codegen->symtable) {
        symtable_cleanup(codegen->symtable);
        free(codegen->symtable);
    }
    
    if (codegen->constants) {
        free(codegen->constants);
    }
    
    if (codegen->bytecode) {
        if (codegen->bytecode->data) {
            free(codegen->bytecode->data);
        }
        free(codegen->bytecode);
    }
    
    if (codegen->context) {
        if (codegen->context->code) {
            free(codegen->context->code);
        }
        if (codegen->context->labels) {
            free(codegen->context->labels);
        }
        free(codegen->context);
    }
    
    memset(codegen, 0, sizeof(CodeGenerator));
}

/* Read AST header */
static int read_ast_header(CodeGenerator* codegen) {
    uint16_t pool_size;
    
    if (fread(&codegen->total_nodes, sizeof(uint16_t), 1, codegen->ast_file) != 1) {
        return -1;
    }
    
    if (fread(&pool_size, sizeof(uint16_t), 1, codegen->ast_file) != 1) {
        return -1;
    }
    
    codegen->pool_size = pool_size;
    return 0;
}

/* Load string pool */
static int load_string_pool(CodeGenerator* codegen) {
    if (codegen->pool_size > 2048) {
        return -1;
    }
    
    if (codegen->pool_size > 0) {
        if (fread(codegen->string_pool, 1, codegen->pool_size, codegen->ast_file) != codegen->pool_size) {
            return -1;
        }
    }
    
    return 0;
}

/* Load symbol table */
static int load_symbol_table(CodeGenerator* codegen, const char* symbol_file) {
    codegen->symtable = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!codegen->symtable) {
        return -1;
    }
    
    if (symtable_read(codegen->symtable, symbol_file) != 0) {
        free(codegen->symtable);
        codegen->symtable = NULL;
        return -1;
    }
    
    return 0;
}

/* Get AST node */
ASTNode* codegen_get_node(CodeGenerator* codegen, uint16_t node_index) {
    long file_pos;
    
    if (!codegen || node_index == 0 || node_index > codegen->total_nodes) {
        return NULL;
    }
    
    /* Read node from file */
    file_pos = sizeof(uint16_t) * 2 + codegen->pool_size + (node_index - 1) * sizeof(ASTNode);
    fseek(codegen->ast_file, file_pos, SEEK_SET);
    
    if (fread(&codegen->nodes[0], sizeof(ASTNode), 1, codegen->ast_file) != 1) {
        return NULL;
    }
    
    codegen->node_count = 1;
    return &codegen->nodes[0];
}

/* Get string from pool */
const char* codegen_get_string(CodeGenerator* codegen, uint16_t offset) {
    if (!codegen || offset >= codegen->pool_size) {
        return NULL;
    }
    
    return &codegen->string_pool[offset];
}

/* Report error */
void codegen_error(CodeGenerator* codegen, const char* message) {
    if (!codegen || !message) {
        return;
    }
    
    printf("Code generation error: %s\n", message);
    codegen->has_error = 1;
    codegen->error_count++;
}

/* Generate code */
int codegen_generate(CodeGenerator* codegen) {
    ASTNode* root;
    ASTNode* class_node;
    
    if (!codegen) {
        return -1;
    }
    
    /* Get root node (parser writes NODE_PROGRAM last) */
    root = codegen_get_node(codegen, codegen->total_nodes);
    if (!root || root->type != NODE_PROGRAM) {
        codegen_error(codegen, "Invalid AST: root node is not NODE_PROGRAM");
        return -1;
    }
    
    /* Get class node */
    class_node = codegen_get_node(codegen, root->data.program.class_node);
    if (!class_node || class_node->type != NODE_CLASS) {
        codegen_error(codegen, "Invalid AST: no class node");
        return -1;
    }
    
    /* Generate code for class */
    if (generate_class(codegen, class_node) != 0) {
        return -1;
    }
    
    /* Write .djc file */
    if (write_djc_file(codegen) != 0) {
        codegen_error(codegen, "Failed to write .djc file");
        return -1;
    }
    
    return codegen->has_error ? -1 : 0;
}

/* Generate code for class */
int generate_class(CodeGenerator* codegen, ASTNode* class_node) {
    const char* class_name;
    uint16_t member_idx;
    ASTNode* member_node;
    uint16_t member_count;
    uint16_t total_member_count;  /* Save before it gets overwritten */
    uint16_t class_name_off;
    uint16_t class_first_member;
    
    if (!codegen || !class_node) {
        return -1;
    }
    
    /* Save class data before later node reads overwrite the shared buffer */
    class_name_off = class_node->data.class_decl.name;
    class_first_member = class_node->data.class_decl.first_member;
    total_member_count = class_node->data.class_decl.member_count;
    
    /* Get class name */
    class_name = codegen_get_string(codegen, class_name_off);
    if (!class_name) {
        codegen_error(codegen, "Invalid class name");
        return -1;
    }
    
    
    
    /* Add class name to constant pool */
    add_utf8_constant(codegen, class_name);
    
    /* Get class symbol */
    codegen->current_class = symtable_lookup(codegen->symtable, class_name);
    
    /* Process members */
    member_idx = class_first_member;
    member_count = 0;
    
    
    
    while (member_idx != 0 && member_count < total_member_count) {
        uint16_t next_sibling;
        
        
        
        member_node = codegen_get_node(codegen, member_idx);
        if (!member_node) {
            
            break;
        }
        
        
        
        /* Save next_sibling before processing */
        next_sibling = member_node->next_sibling;
        
        if (member_node->type == NODE_METHOD) {
            if (generate_method(codegen, member_node) != 0) {
                return -1;
            }
        } else if (member_node->type == NODE_FIELD) {
            /* Add field to field table */
            const char* field_name = codegen_get_string(codegen, member_node->data.field.name);
            if (field_name && codegen->field_count < 64) {
                codegen->fields[codegen->field_count].name_index = find_or_add_utf8(codegen, field_name);
                codegen->fields[codegen->field_count].descriptor_index = 0; /* TODO: type descriptor */
                codegen->fields[codegen->field_count].flags =
                    (member_node->data.field.is_static ? FIELD_STATIC : 0) |
                    (member_node->data.field.is_public ? FIELD_PUBLIC : 0);
                codegen->field_count++;
            }
        }
        
        member_idx = next_sibling;
        member_count++;
        
    }
    
    
    
    return 0;
}

/* Generate code for method */
int generate_method(CodeGenerator* codegen, ASTNode* method_node) {
    const char* method_name;
    ASTNode* body_node;
    uint16_t code_start;
    uint16_t code_length;
    TypeInfo return_type;  /* Save return type before reading other nodes */
    uint16_t body_index;   /* Save body node index */
    int is_static;         /* Save method flags */
    int is_public;
    
    if (!codegen || !method_node) {
        return -1;
    }
    
    /* Save method info before reading other nodes (which will overwrite method_node) */
    method_name = codegen_get_string(codegen, method_node->data.method.name);
    return_type = method_node->data.method.return_type;
    body_index = method_node->data.method.body;
    is_static = method_node->data.method.is_static;
    is_public = method_node->data.method.is_public;
    
    if (!method_name) {
        codegen_error(codegen, "Invalid method name");
        return -1;
    }
    
    /* Get method symbol */
    codegen->current_method = symtable_lookup(codegen->symtable, method_name);
    
    /* Create code generation context */
    codegen->context = (CodeGenContext*)malloc(sizeof(CodeGenContext));
    if (!codegen->context) {
        codegen_error(codegen, "Failed to allocate context");
        return -1;
    }
    memset(codegen->context, 0, sizeof(CodeGenContext));
    
    codegen->context->code = (ByteBuffer*)malloc(sizeof(ByteBuffer));
    codegen->context->labels = (LabelList*)malloc(sizeof(LabelList));
    if (!codegen->context->code || !codegen->context->labels) {
        codegen_error(codegen, "Failed to allocate context buffers");
        return -1;
    }
    memset(codegen->context->code, 0, sizeof(ByteBuffer));
    memset(codegen->context->labels, 0, sizeof(LabelList));
    
    /* Allocate code buffer data separately (8KB - reduced for DOS memory constraints) */
    codegen->context->code->capacity = 8192;
    codegen->context->code->data = (uint8_t*)malloc(codegen->context->code->capacity);
    if (!codegen->context->code->data) {
        codegen_error(codegen, "Failed to allocate method code buffer");
        free(codegen->context->labels);
        free(codegen->context->code);
        free(codegen->context);
        codegen->context = NULL;
        return -1;
    }
    codegen->context->code->size = 0;
    
    /* Generate method body */
    body_node = codegen_get_node(codegen, body_index);
    if (body_node) {
        generate_block(codegen, body_node);
    }
    
    /* Add implicit return for void methods if not already present */
    if (return_type.kind == TYPE_VOID) {
        /* Check if last instruction is already a return */
        if (codegen->context->code->size == 0 ||
            codegen->context->code->data[codegen->context->code->size - 1] != OP_RETURN) {
            emit_opcode(codegen, OP_RETURN);
        }
    }
    
    /* Backpatch labels */
    backpatch_labels(codegen);
    
    /* Copy method code to main bytecode buffer */
    code_start = codegen->bytecode->size;

    if (code_start + codegen->context->code->size > codegen->bytecode->capacity) {
        codegen_error(codegen, "Bytecode size exceeds maximum");
        return -1;
    }
    
    memcpy(&codegen->bytecode->data[code_start],
           codegen->context->code->data,
           codegen->context->code->size);
    codegen->bytecode->size += codegen->context->code->size;
    code_length = codegen->context->code->size;

    /* Add or update method in method table */
    {
        uint16_t method_idx;
        uint16_t name_idx;
        int found = 0;
        
        /* Search for existing placeholder entry */
        name_idx = find_or_add_utf8(codegen, method_name);
        for (method_idx = 0; method_idx < codegen->method_count; method_idx++) {
            if (codegen->methods[method_idx].name_index == name_idx) {
                /* Found existing entry - update it */
                codegen->methods[method_idx].descriptor_index = 0; /* TODO: type descriptor */
                codegen->methods[method_idx].code_offset = code_start;
                codegen->methods[method_idx].code_length = code_length;
                codegen->methods[method_idx].max_stack = codegen->context->max_stack;
                codegen->methods[method_idx].max_locals = codegen->context->max_locals;
                codegen->methods[method_idx].flags =
                    (is_static ? METHOD_STATIC : 0) |
                    (is_public ? METHOD_PUBLIC : 0);

                found = 1;
                break;
            }
        }
        
        /* If not found, add new entry */
        if (!found && codegen->method_count < 64) {
            codegen->methods[codegen->method_count].name_index = name_idx;
            codegen->methods[codegen->method_count].descriptor_index = 0; /* TODO: type descriptor */
            codegen->methods[codegen->method_count].code_offset = code_start;
            codegen->methods[codegen->method_count].code_length = code_length;
            codegen->methods[codegen->method_count].max_stack = codegen->context->max_stack;
            codegen->methods[codegen->method_count].max_locals = codegen->context->max_locals;
            codegen->methods[codegen->method_count].flags =
                (is_static ? METHOD_STATIC : 0) |
                (is_public ? METHOD_PUBLIC : 0);

            codegen->method_count++;
        }
    }
    
    /* Cleanup context */
    if (codegen->context->code) {
        if (codegen->context->code->data) {
            free(codegen->context->code->data);
        }
        free(codegen->context->code);
    }
    free(codegen->context->labels);
    free(codegen->context);
    codegen->context = NULL;
    codegen->current_method = NULL;
    
    return 0;
}

/* Generate code for statement */
int generate_statement(CodeGenerator* codegen, ASTNode* stmt_node) {
    uint16_t node_type;
    
    if (!codegen || !stmt_node) {
        return -1;
    }
    
    /* Save node type BEFORE any operations that might corrupt memory */
    node_type = stmt_node->type;
    
    switch (node_type) {
        case NODE_BLOCK:
            return generate_block(codegen, stmt_node);
        
        case NODE_VAR_DECL:
            return generate_var_decl(codegen, stmt_node);
        
        case NODE_IF:
            return generate_if_stmt(codegen, stmt_node);
        
        case NODE_WHILE:
            return generate_while_stmt(codegen, stmt_node);
        
        case NODE_FOR:
            return generate_for_stmt(codegen, stmt_node);
        
        case NODE_RETURN:
            return generate_return_stmt(codegen, stmt_node);
        
        case NODE_EXPR_STMT: {
            ASTNode* expr_node = codegen_get_node(codegen, stmt_node->data.expr_stmt.expr);
            if (expr_node) {
                int16_t stack_before = codegen->context->current_stack;
                generate_expression(codegen, expr_node);
                /* Pop result if expression left something on stack */
                if (codegen->context->current_stack > stack_before) {
                    emit_opcode(codegen, OP_POP);
                    update_stack(codegen, -1);
                }
            }
            return 0;
        }
        
        default:
            codegen_error(codegen, "Unknown statement type");
            return -1;
    }
}

/* Generate code for block */
int generate_block(CodeGenerator* codegen, ASTNode* block_node) {
    uint16_t stmt_idx;
    ASTNode* stmt_node;
    uint16_t stmt_count;
    uint16_t total_stmt_count;  /* Save block's stmt_count before it gets overwritten */
    
    if (!codegen || !block_node) {
        return -1;
    }
    
    
    
    /* Save block data BEFORE any codegen_get_node calls that may overwrite block_node */
    stmt_idx = block_node->data.block.first_stmt;
    total_stmt_count = block_node->data.block.stmt_count;
    stmt_count = 0;
    
    while (stmt_idx != 0 && stmt_count < total_stmt_count) {
        uint16_t next_sibling;
        
        
        
        stmt_node = codegen_get_node(codegen, stmt_idx);
        if (!stmt_node) {
            
            break;
        }
        
        
        
        /* Save next_sibling BEFORE calling generate_statement, which may overwrite stmt_node */
        next_sibling = stmt_node->next_sibling;
        
        generate_statement(codegen, stmt_node);
        
        stmt_idx = next_sibling;
        stmt_count++;
        
    }
    
    
    
    
    
    return 0;
}

/* Generate code for variable declaration */
int generate_var_decl(CodeGenerator* codegen, ASTNode* var_node) {
    ASTNode* init_expr;
    const char* var_name;
    uint16_t local_idx;
    uint16_t init_expr_idx;
    
    if (!codegen || !var_node) {
        return -1;
    }
    
    /* Get variable name and save init_expr index before reading other nodes */
    var_name = codegen_get_string(codegen, var_node->data.var_decl.name);
    init_expr_idx = var_node->data.var_decl.init_expr;
    
    if (!var_name) {
        return -1;
    }

    /* Get local index */

    local_idx = get_local_index(codegen, var_name);

    /* Update max_locals */
    if (local_idx + 1 > codegen->context->max_locals) {
        codegen->context->max_locals = local_idx + 1;
    }
    
    /* Generate initializer if present */
    if (init_expr_idx != 0) {
        init_expr = codegen_get_node(codegen, init_expr_idx);
        if (init_expr) {
            generate_expression(codegen, init_expr);
            
            /* Store to local variable */
            if (local_idx <= 2) {
                emit_opcode(codegen, OP_STORE_0 + local_idx);
            } else {
                emit_opcode(codegen, OP_STORE_LOCAL);
                emit_u1(codegen, (uint8_t)local_idx);
            }
            update_stack(codegen, -1);
        }
    }
    
    return 0;
}

/* Generate code for if statement */
int generate_if_stmt(CodeGenerator* codegen, ASTNode* if_node) {
    ASTNode* cond_node;
    ASTNode* then_node;
    ASTNode* else_node;
    uint16_t else_label;
    uint16_t end_label;
    uint16_t condition_index;
    uint16_t then_index;
    uint16_t else_index;
    
    if (!codegen || !if_node) {
        return -1;
    }
    
    /* CRITICAL: Save all indices BEFORE any recursive calls */
    condition_index = if_node->data.if_stmt.condition;
    then_index = if_node->data.if_stmt.then_stmt;
    else_index = if_node->data.if_stmt.else_stmt;
    
    /* Generate condition */
    cond_node = codegen_get_node(codegen, condition_index);
    if (cond_node) {
        generate_expression(codegen, cond_node);
    }
    
    /* Create labels */
    else_label = create_label(codegen);
    end_label = create_label(codegen);
    
    /* Jump to else if condition is false */
    emit_jump(codegen, OP_IF_FALSE, else_label);
    update_stack(codegen, -1);
    
    /* Generate then branch */
    then_node = codegen_get_node(codegen, then_index);
    if (then_node) {
        generate_statement(codegen, then_node);
    }
    
    /* Jump to end */
    if (else_index != 0) {
        emit_jump(codegen, OP_GOTO, end_label);
    }
    
    /* Else label */
    emit_label(codegen, else_label);
    
    /* Generate else branch if present */
    if (else_index != 0) {
        else_node = codegen_get_node(codegen, else_index);
        if (else_node) {
            generate_statement(codegen, else_node);
        }
        emit_label(codegen, end_label);
    }
    
    return 0;
}

/* Generate code for while statement */
int generate_while_stmt(CodeGenerator* codegen, ASTNode* while_node) {
    ASTNode* cond_node;
    ASTNode* body_node;
    uint16_t loop_start;
    uint16_t loop_end;
    uint16_t condition_index;
    uint16_t body_index;
    
    if (!codegen || !while_node) {
        return -1;
    }
    
    /* CRITICAL: Save all indices BEFORE any recursive calls */
    condition_index = while_node->data.while_stmt.condition;
    body_index = while_node->data.while_stmt.body;
    
    /* Create labels */
    loop_start = create_label(codegen);
    loop_end = create_label(codegen);
    
    /* Loop start */
    emit_label(codegen, loop_start);
    
    /* Generate condition */
    cond_node = codegen_get_node(codegen, condition_index);
    if (cond_node) {
        generate_expression(codegen, cond_node);
    }
    
    /* Jump to end if condition is false */
    emit_jump(codegen, OP_IF_FALSE, loop_end);
    update_stack(codegen, -1);
    
    /* Generate body */
    body_node = codegen_get_node(codegen, body_index);
    if (body_node) {
        generate_statement(codegen, body_node);
    }
    
    /* Jump back to start */
    emit_jump(codegen, OP_GOTO, loop_start);
    
    /* Loop end */
    emit_label(codegen, loop_end);
    
    return 0;
}

/* Generate code for for statement */
int generate_for_stmt(CodeGenerator* codegen, ASTNode* for_node) {
    ASTNode* init_node;
    ASTNode* cond_node;
    ASTNode* update_node;
    ASTNode* body_node;
    uint16_t loop_start;
    uint16_t loop_end;
    uint16_t init_index;
    uint16_t condition_index;
    uint16_t update_index;
    uint16_t body_index;
    
    if (!codegen || !for_node) {
        return -1;
    }
    
    /* CRITICAL: Save all indices BEFORE any recursive calls */
    init_index = for_node->data.for_stmt.init;
    condition_index = for_node->data.for_stmt.condition;
    update_index = for_node->data.for_stmt.update;
    body_index = for_node->data.for_stmt.body;
    
    /* Generate init (optional) */
    if (init_index != 0) {
        init_node = codegen_get_node(codegen, init_index);
        if (init_node) {
            generate_expression(codegen, init_node);
            /* Pop the result if it's an expression statement */
            emit_opcode(codegen, OP_POP);
            update_stack(codegen, -1);
        }
    }
    
    /* Create labels */
    loop_start = create_label(codegen);
    loop_end = create_label(codegen);
    
    /* Loop start */
    emit_label(codegen, loop_start);
    
    /* Generate condition (optional, if missing assume true) */
    if (condition_index != 0) {
        cond_node = codegen_get_node(codegen, condition_index);
        if (cond_node) {
            generate_expression(codegen, cond_node);
            
            /* Jump to end if condition is false */
            emit_jump(codegen, OP_IF_FALSE, loop_end);
            update_stack(codegen, -1);
        }
    }
    
    /* Generate body */
    body_node = codegen_get_node(codegen, body_index);
    if (body_node) {
        generate_statement(codegen, body_node);
    }
    
    /* Generate update (optional) */
    if (update_index != 0) {
        update_node = codegen_get_node(codegen, update_index);
        if (update_node) {
            generate_expression(codegen, update_node);
            /* Pop the result */
            emit_opcode(codegen, OP_POP);
            update_stack(codegen, -1);
        }
    }
    
    /* Jump back to start */
    emit_jump(codegen, OP_GOTO, loop_start);
    
    /* Loop end */
    emit_label(codegen, loop_end);
    
    return 0;
}

/* Generate code for return statement */
int generate_return_stmt(CodeGenerator* codegen, ASTNode* return_node) {
    ASTNode* expr_node;
    
    if (!codegen || !return_node) {
        return -1;
    }
    
    if (return_node->data.return_stmt.expr != 0) {
        /* Return with value */
        expr_node = codegen_get_node(codegen, return_node->data.return_stmt.expr);
        if (expr_node) {
            generate_expression(codegen, expr_node);
        }
        emit_opcode(codegen, OP_RETURN_VALUE);
        update_stack(codegen, -1);
    } else {
        /* Return void */
        emit_opcode(codegen, OP_RETURN);
    }
    
    return 0;
}

/* Generate code for expression */
int generate_expression(CodeGenerator* codegen, ASTNode* expr_node) {
    if (!codegen || !expr_node) {
        return -1;
    }
    
    
    
    switch (expr_node->type) {
        case NODE_LITERAL_INT:
            emit_opcode(codegen, OP_PUSH_INT);
            emit_u2(codegen, (uint16_t)expr_node->data.literal_int.int_value);
            update_stack(codegen, 1);
            return 0;
        
        case NODE_LITERAL_BOOL:
            emit_opcode(codegen, OP_PUSH_INT);
            emit_u2(codegen, expr_node->data.literal_bool.bool_value);
            update_stack(codegen, 1);
            return 0;
        
        case NODE_LITERAL_STRING: {
            const char* str_value;
            uint16_t const_idx;
            
            str_value = codegen_get_string(codegen, expr_node->data.literal_string.str_offset);
            if (!str_value) {
                codegen_error(codegen, "Invalid string literal");
                return -1;
            }
            
            const_idx = find_or_add_utf8(codegen, str_value);
            if (const_idx == 0xFFFF) {
                codegen_error(codegen, "Failed to add string constant");
                return -1;
            }
            
            emit_opcode(codegen, OP_PUSH_CONST);
            emit_u2(codegen, const_idx);
            update_stack(codegen, 1);
            return 0;
        }
        
        case NODE_IDENTIFIER:
            return generate_identifier(codegen, expr_node);
        
        case NODE_BINARY_OP:
            return generate_binary_op(codegen, expr_node);
        
        case NODE_UNARY_OP:
            return generate_unary_op(codegen, expr_node);
        
        case NODE_POSTFIX_OP:
            return generate_postfix_op(codegen, expr_node);
        
        case NODE_ASSIGN:
            return generate_assignment(codegen, expr_node);
        
        case NODE_CALL:
            return generate_method_call(codegen, expr_node);
        
        case NODE_NEW: {
            ASTNode* size_node;
            uint16_t size_idx = expr_node->next_sibling;
            uint8_t elem_type = (uint8_t)expr_node->data.new_expr.class_name;
            
            size_node = codegen_get_node(codegen, size_idx);
            if (!size_node) {
                codegen_error(codegen, "Invalid array size expression");
                return -1;
            }
            generate_expression(codegen, size_node);
            emit_opcode(codegen, OP_NEW_ARRAY);
            emit_u1(codegen, elem_type);
            return 0;
        }
        
        case NODE_ARRAY_ACCESS: {
            ASTNode* array_node;
            ASTNode* index_node;
            uint16_t array_idx = expr_node->data.array_access.array;
            uint16_t index_idx = expr_node->data.array_access.index;
            
            array_node = codegen_get_node(codegen, array_idx);
            if (!array_node) {
                codegen_error(codegen, "Invalid array access");
                return -1;
            }
            generate_expression(codegen, array_node);
            
            index_node = codegen_get_node(codegen, index_idx);
            if (!index_node) {
                codegen_error(codegen, "Invalid array access");
                return -1;
            }
            generate_expression(codegen, index_node);
            emit_opcode(codegen, OP_ARRAY_LOAD);
            update_stack(codegen, -1);
            return 0;
        }
        
        case NODE_FIELD_ACCESS: {
            const char* field_name;
            ASTNode* object_node;
            uint16_t field_name_off = expr_node->data.field_access.field_name;
            uint16_t object_idx = expr_node->data.field_access.object;
            
            field_name = codegen_get_string(codegen, field_name_off);
            if (!field_name) {
                codegen_error(codegen, "Invalid field access");
                return -1;
            }
            
            object_node = codegen_get_node(codegen, object_idx);
            if (!object_node) {
                codegen_error(codegen, "Invalid field access");
                return -1;
            }
            
            if (strcmp(field_name, "length") == 0) {
                generate_expression(codegen, object_node);
                emit_opcode(codegen, OP_ARRAY_LENGTH);
                return 0;
            }
            codegen_error(codegen, "Unsupported field access");
            return -1;
        }
        
        default:
            codegen_error(codegen, "Unknown expression type");
            return -1;
    }
}

/* Generate code for binary operation */
int generate_binary_op(CodeGenerator* codegen, ASTNode* binop_node) {
    ASTNode* left_node;
    ASTNode* right_node;
    uint16_t op;
    uint16_t left_index;
    uint16_t right_index;
    
    if (!codegen || !binop_node) {
        return -1;
    }
    
    /* CRITICAL: Save operator and operand indices BEFORE any recursive calls
     * because generate_expression() may overwrite the binop_node memory */
    {
        unsigned char* op_ptr = (unsigned char*)&(binop_node->data.binary_op.op);
        op = op_ptr[0] | (op_ptr[1] << 8);
        left_index = binop_node->data.binary_op.left;
        right_index = binop_node->data.binary_op.right;
    }
    
    /* Generate left operand */
    left_node = codegen_get_node(codegen, left_index);
    if (left_node) {
        generate_expression(codegen, left_node);
    }
    
    /* Generate right operand */
    right_node = codegen_get_node(codegen, right_index);
    if (right_node) {
        generate_expression(codegen, right_node);
    }
    
    /* Generate operation - map AST operators to VM opcodes */
    {
        switch (op) {
            case (uint16_t)BINOP_ADD:
                emit_opcode(codegen, OP_ADD);
                break;
            case (uint16_t)BINOP_SUB:
                emit_opcode(codegen, OP_SUB);
                break;
            case (uint16_t)BINOP_MUL:
                emit_opcode(codegen, OP_MUL);
                break;
            case (uint16_t)BINOP_DIV:
                emit_opcode(codegen, OP_DIV);
                break;
            case (uint16_t)BINOP_MOD:
                emit_opcode(codegen, OP_MOD);
                break;
            case (uint16_t)BINOP_EQ:
                emit_opcode(codegen, OP_CMP_EQ);
                break;
            case (uint16_t)BINOP_NE:
                emit_opcode(codegen, OP_CMP_NE);
                break;
            case (uint16_t)BINOP_LT:
                emit_opcode(codegen, OP_CMP_LT);
                break;
            case (uint16_t)BINOP_LE:
                emit_opcode(codegen, OP_CMP_LE);
                break;
            case (uint16_t)BINOP_GT:
                emit_opcode(codegen, OP_CMP_GT);
                break;
            case (uint16_t)BINOP_GE:
                emit_opcode(codegen, OP_CMP_GE);
                break;
            case (uint16_t)BINOP_AND:
            case (uint16_t)BINOP_OR:
                /* Logical operations handled differently */
                codegen_error(codegen, "Logical operations not yet implemented");
                return -1;
            default:
                codegen_error(codegen, "Unknown binary operator");
                return -1;
        }
    }
    
    update_stack(codegen, -1); /* Two operands consumed, one result produced */
    return 0;
}

/* Generate code for unary operation */
int generate_unary_op(CodeGenerator* codegen, ASTNode* unop_node) {
    ASTNode* operand_node;
    
    if (!codegen || !unop_node) {
        return -1;
    }
    
    /* Generate operand */
    operand_node = codegen_get_node(codegen, unop_node->data.unary_op.operand);
    if (operand_node) {
        generate_expression(codegen, operand_node);
    }
    
    /* Generate operation */
    switch (unop_node->data.unary_op.op) {
        case UNOP_NEG:
            emit_opcode(codegen, OP_NEG);
            break;
        case UNOP_NOT:
            /* Logical not: push 0, compare equal */
            emit_opcode(codegen, OP_PUSH_INT);
            emit_u2(codegen, 0);
            update_stack(codegen, 1);
            emit_opcode(codegen, OP_CMP_EQ);
            update_stack(codegen, -1);
            break;
        default:
            codegen_error(codegen, "Unknown unary operator");
            return -1;
    }
    
    return 0;
}

/* Generate code for postfix operation */
int generate_postfix_op(CodeGenerator* codegen, ASTNode* postop_node) {
    ASTNode* operand_node;
    const char* var_name;
    uint16_t local_idx;
    uint16_t operand_idx;
    uint16_t op;
    
    if (!codegen || !postop_node) {
        return -1;
    }
    
    /* CRITICAL: Save operand index and operator BEFORE any codegen_get_node calls */
    operand_idx = postop_node->data.postfix_op.operand;
    op = postop_node->data.postfix_op.op;
    
    /* Get operand (must be an identifier) */
    operand_node = codegen_get_node(codegen, operand_idx);
    if (!operand_node || operand_node->type != NODE_IDENTIFIER) {
        codegen_error(codegen, "Postfix operator requires variable");
        return -1;
    }
    
    /* Get variable name and index */
    var_name = codegen_get_string(codegen, operand_node->data.identifier.name);
    if (!var_name) {
        return -1;
    }
    
    local_idx = get_local_index(codegen, var_name);
    if (local_idx == 0xFFFF) {
        codegen_error(codegen, "Undefined variable");
        return -1;
    }
    
    /* Load current value */
    if (local_idx <= 2) {
        emit_opcode(codegen, OP_LOAD_0 + local_idx);
    } else {
        emit_opcode(codegen, OP_LOAD_LOCAL);
        emit_u1(codegen, (uint8_t)local_idx);
    }
    update_stack(codegen, 1);
    
    /* Duplicate for return value (postfix returns old value) */
    emit_opcode(codegen, OP_DUP);
    update_stack(codegen, 1);
    
    /* Push 1 */
    emit_opcode(codegen, OP_PUSH_INT);
    emit_u2(codegen, 1);
    update_stack(codegen, 1);
    
    /* Add or subtract */
    if (op == POSTOP_INC) {
        emit_opcode(codegen, OP_ADD);
    } else {  /* POSTOP_DEC */
        emit_opcode(codegen, OP_SUB);
    }
    update_stack(codegen, -1);
    
    /* Store back to variable */
    if (local_idx <= 2) {
        emit_opcode(codegen, OP_STORE_0 + local_idx);
    } else {
        emit_opcode(codegen, OP_STORE_LOCAL);
        emit_u1(codegen, (uint8_t)local_idx);
    }
    update_stack(codegen, -1);
    
    /* Stack now has the old value (before increment/decrement) */
    return 0;
}

/* Generate code for assignment */
int generate_assignment(CodeGenerator* codegen, ASTNode* assign_node) {
    ASTNode* target_node;
    ASTNode* value_node;
    const char* var_name;
    uint16_t local_idx;
    uint16_t target_index;
    uint16_t value_index;
    uint16_t target_type;
    uint16_t target_name_off;
    uint16_t array_idx;
    uint16_t index_idx;
    
    if (!codegen || !assign_node) {
        return -1;
    }
    
    target_index = assign_node->data.assign.target;
    value_index = assign_node->data.assign.value;
    
    target_node = codegen_get_node(codegen, target_index);
    if (!target_node) {
        codegen_error(codegen, "Invalid assignment");
        return -1;
    }
    
    /* Save target information before any further codegen_get_node() calls */
    target_type = target_node->type;
    target_name_off = 0;
    array_idx = 0;
    index_idx = 0;
    
    if (target_type == NODE_IDENTIFIER) {
        target_name_off = target_node->data.identifier.name;
    } else if (target_type == NODE_ARRAY_ACCESS) {
        array_idx = target_node->data.array_access.array;
        index_idx = target_node->data.array_access.index;
    }
    
    value_node = codegen_get_node(codegen, value_index);
    if (!value_node) {
        codegen_error(codegen, "Invalid assignment");
        return -1;
    }
    
    if (target_type == NODE_ARRAY_ACCESS) {
        ASTNode* array_node;
        ASTNode* index_node;
        
        array_node = codegen_get_node(codegen, array_idx);
        if (!array_node) {
            codegen_error(codegen, "Invalid array assignment target");
            return -1;
        }
        generate_expression(codegen, array_node);
        
        index_node = codegen_get_node(codegen, index_idx);
        if (!index_node) {
            codegen_error(codegen, "Invalid array assignment target");
            return -1;
        }
        generate_expression(codegen, index_node);
        
        value_node = codegen_get_node(codegen, value_index);
        if (!value_node) {
            codegen_error(codegen, "Invalid assignment");
            return -1;
        }
        generate_expression(codegen, value_node);
        emit_opcode(codegen, OP_ARRAY_STORE);
        update_stack(codegen, -3);
        return 0;
    }
    
    generate_expression(codegen, value_node);
    emit_opcode(codegen, OP_DUP);
    update_stack(codegen, 1);
    
    if (target_type != NODE_IDENTIFIER) {
        codegen_error(codegen, "Invalid assignment target");
        return -1;
    }
    
    var_name = codegen_get_string(codegen, target_name_off);
    if (!var_name) {
        return -1;
    }
    
    local_idx = get_local_index(codegen, var_name);
    
    if (local_idx <= 2) {
        emit_opcode(codegen, OP_STORE_0 + local_idx);
    } else {
        emit_opcode(codegen, OP_STORE_LOCAL);
        emit_u1(codegen, (uint8_t)local_idx);
    }
    update_stack(codegen, -1);
    
    return 0;
}

/* Generate code for method call */
int generate_method_call(CodeGenerator* codegen, ASTNode* call_node) {
    const char* method_name;
    uint16_t arg_idx;
    ASTNode* arg_node;
    ASTNode* object_node;
    uint16_t arg_count;
    uint16_t total_arg_count;
    uint16_t method_idx;
    uint16_t object_idx;
    int is_native;
    NodeType arg_node_type;
    uint16_t saved_first_arg;
    Symbol* method_sym;
    int returns_value;
    const char* first_arg_name;
    uint16_t i;
    int first_arg_is_string;
    
    if (!codegen || !call_node) {
        return -1;
    }
    
    arg_idx = call_node->data.call.first_arg;
    total_arg_count = call_node->data.call.arg_count;
    object_idx = call_node->data.call.object;
    saved_first_arg = arg_idx;
    
    method_name = codegen_get_string(codegen, call_node->data.call.method_name);
    if (!method_name) {
        codegen_error(codegen, "Invalid method name");
        return -1;
    }
    
    if (object_idx != 0) {
        object_node = codegen_get_node(codegen, object_idx);
        (void)object_node;
    }
    
    is_native = 0;
    if (strcmp(method_name, "println") == 0) {
        is_native = 1;
    }
    
    arg_node_type = NODE_PROGRAM;
    first_arg_name = NULL;
    first_arg_is_string = 0;
    if (saved_first_arg != 0) {
        arg_node = codegen_get_node(codegen, saved_first_arg);
        if (arg_node) {
            arg_node_type = arg_node->type;
            if (arg_node_type == NODE_IDENTIFIER) {
                first_arg_name = codegen_get_string(codegen, arg_node->data.identifier.name);
                if (first_arg_name) {
                    for (i = 0; i < codegen->symtable->symbol_count; i++) {
                        Symbol* sym = &codegen->symtable->symbols[i];
                        const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                            sym_name &&
                            strcmp(sym_name, first_arg_name) == 0 &&
                            sym->type.kind == TYPE_CLASS) {
                            const char* class_name = symtable_get_string(codegen->symtable, sym->type.class_name);
                            if (class_name && strcmp(class_name, "String") == 0) {
                                first_arg_is_string = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    
    arg_count = 0;
    arg_idx = saved_first_arg;
    while (arg_idx != 0 && arg_count < total_arg_count) {
        uint16_t next_arg_idx;
        
        arg_node = codegen_get_node(codegen, arg_idx);
        if (!arg_node) {
            break;
        }
        
        next_arg_idx = arg_node->next_sibling;
        generate_expression(codegen, arg_node);
        
        arg_idx = next_arg_idx;
        arg_count++;
    }
    
    method_idx = find_method_index(codegen, method_name, is_native);
    if (method_idx == 0xFFFF) {
        codegen_error(codegen, "Failed to add method reference");
        return -1;
    }
    
    if (is_native && codegen->methods[method_idx].descriptor_index == 0) {
        uint16_t desc_idx;
        char descriptor[32];
        
        if (arg_node_type == NODE_LITERAL_STRING || first_arg_is_string) {
            strcpy(descriptor, "(Ljava/lang/String;)V");
        } else if (arg_node_type == NODE_LITERAL_INT ||
                   arg_node_type == NODE_IDENTIFIER ||
                   arg_node_type == NODE_BINARY_OP ||
                   arg_node_type == NODE_UNARY_OP ||
                   arg_node_type == NODE_POSTFIX_OP ||
                   arg_node_type == NODE_CALL) {
            strcpy(descriptor, "(I)V");
        } else {
            strcpy(descriptor, "(I)V");
        }
        
        desc_idx = find_or_add_utf8(codegen, descriptor);
        if (desc_idx != 0xFFFF) {
            codegen->methods[method_idx].descriptor_index = desc_idx;
        }
    }
    
    returns_value = 0;
    if (!is_native) {
        method_sym = symtable_lookup(codegen->symtable, method_name);
        if (method_sym && method_sym->kind == SYM_METHOD && method_sym->type.kind != TYPE_VOID) {
            returns_value = 1;
        }
    }
    
    emit_opcode(codegen, OP_INVOKE_STATIC);
    emit_u2(codegen, method_idx);
    emit_u1(codegen, (uint8_t)arg_count);
    
    update_stack(codegen, -(int16_t)arg_count);
    if (returns_value) {
        update_stack(codegen, 1);
    }
    
    return 0;
}

/* Generate code for identifier */
int generate_identifier(CodeGenerator* codegen, ASTNode* id_node) {
    const char* var_name;
    uint16_t local_idx;
    
    if (!codegen || !id_node) {
        return -1;
    }
    
    /* Get variable name */
    var_name = codegen_get_string(codegen, id_node->data.identifier.name);
    if (!var_name) {
        return -1;
    }
    
    /* Get local index */
    local_idx = get_local_index(codegen, var_name);
    
    /* Load from local variable */
    if (local_idx <= 2) {
        emit_opcode(codegen, OP_LOAD_0 + local_idx);
    } else {
        emit_opcode(codegen, OP_LOAD_LOCAL);
        emit_u1(codegen, (uint8_t)local_idx);
    }
    update_stack(codegen, 1);
    
    return 0;
}

/* Emit opcode */
int emit_opcode(CodeGenerator* codegen, uint8_t opcode) {
    if (!codegen || !codegen->context || !codegen->context->code) {
        return -1;
    }
    
    if (codegen->context->code->size >= codegen->context->code->capacity) {
        codegen_error(codegen, "Code size exceeds maximum");
        return -1;
    }
    
    codegen->context->code->data[codegen->context->code->size++] = opcode;
    return 0;
}

/* Emit byte */
int emit_u1(CodeGenerator* codegen, uint8_t value) {
    if (!codegen || !codegen->context || !codegen->context->code) {
        return -1;
    }
    
    if (codegen->context->code->size >= codegen->context->code->capacity) {
        codegen_error(codegen, "Code size exceeds maximum");
        return -1;
    }
    
    codegen->context->code->data[codegen->context->code->size++] = value;
    return 0;
}

/* Emit word */
int emit_u2(CodeGenerator* codegen, uint16_t value) {
    if (!codegen || !codegen->context || !codegen->context->code) {
        return -1;
    }
    
    if (codegen->context->code->size + 1 >= codegen->context->code->capacity) {
        codegen_error(codegen, "Code size exceeds maximum");
        return -1;
    }
    
    /* Little-endian */
    codegen->context->code->data[codegen->context->code->size++] = (uint8_t)(value & 0xFF);
    codegen->context->code->data[codegen->context->code->size++] = (uint8_t)(value >> 8);
    return 0;
}

/* Add UTF8 constant */
uint16_t add_utf8_constant(CodeGenerator* codegen, const char* str) {
    uint16_t len;
    uint16_t idx;
    
    if (!codegen || !str || !codegen->constants) {
        return 0xFFFF;
    }
    
    len = strlen(str);
    
    /* Check capacity */
    if (codegen->constants->count >= 256) {
        return 0xFFFF;
    }
    
    if (codegen->constants->string_size + len + 1 > 4096) {
        return 0xFFFF;
    }
    
    /* Add constant */
    idx = codegen->constants->count;
    codegen->constants->constants[idx].tag = CONST_UTF8;
    codegen->constants->constants[idx].length = len;
    codegen->constants->constants[idx].data.utf8_data = 
        &codegen->constants->string_data[codegen->constants->string_size];
    
    /* Copy string */
    strcpy(&codegen->constants->string_data[codegen->constants->string_size], str);
    codegen->constants->string_size += len + 1;
    
    codegen->constants->count++;
    return idx;
}

/* Add integer constant */
uint16_t add_int_constant(CodeGenerator* codegen, int16_t value) {
    uint16_t idx;
    
    if (!codegen || !codegen->constants) {
        return 0xFFFF;
    }
    
    if (codegen->constants->count >= 256) {
        return 0xFFFF;
    }
    
    idx = codegen->constants->count;
    codegen->constants->constants[idx].tag = CONST_INTEGER;
    codegen->constants->constants[idx].length = 2;
    codegen->constants->constants[idx].data.int_value = value;
    
    codegen->constants->count++;
    return idx;
}

/* Find or add UTF8 constant */
uint16_t find_or_add_utf8(CodeGenerator* codegen, const char* str) {
    uint16_t i;
    
    if (!codegen || !str || !codegen->constants) {
        return 0xFFFF;
    }
    
    /* Search for existing constant */
    for (i = 0; i < codegen->constants->count; i++) {
        if (codegen->constants->constants[i].tag == CONST_UTF8) {
            if (strcmp(codegen->constants->constants[i].data.utf8_data, str) == 0) {
                return i;
            }
        }
    }
    
    /* Not found, add new constant */
    return add_utf8_constant(codegen, str);
}

/* Find or create method index */
uint16_t find_method_index(CodeGenerator* codegen, const char* method_name, int is_native) {
    uint16_t i;
    uint16_t name_idx;
    
    if (!codegen || !method_name) {
        return 0xFFFF;
    }
    
    for (i = 0; i < codegen->method_count; i++) {
        name_idx = codegen->methods[i].name_index;
        
        if (name_idx < codegen->constants->count) {
            if (codegen->constants->constants[name_idx].tag == CONST_UTF8) {
                const char* existing_name = codegen->constants->constants[name_idx].data.utf8_data;
                
                if (strcmp(existing_name, method_name) == 0) {
                    int is_method_native = (codegen->methods[i].flags & METHOD_NATIVE) != 0;
                    if (is_native == is_method_native) {
                        return i;
                    }
                }
            }
        }
    }
    
    if (codegen->method_count >= 64) {
        return 0xFFFF;
    }
    
    name_idx = find_or_add_utf8(codegen, method_name);
    if (name_idx == 0xFFFF) {
        return 0xFFFF;
    }
    
    codegen->methods[codegen->method_count].name_index = name_idx;
    codegen->methods[codegen->method_count].descriptor_index = 0;
    codegen->methods[codegen->method_count].code_offset = 0;
    codegen->methods[codegen->method_count].code_length = 0;
    codegen->methods[codegen->method_count].max_stack = 0;
    codegen->methods[codegen->method_count].max_locals = 0;
    codegen->methods[codegen->method_count].flags = is_native ? METHOD_NATIVE : 0;
    
    codegen->method_count++;
    return (uint16_t)(codegen->method_count - 1);
}

/* Create label */
uint16_t create_label(CodeGenerator* codegen) {
    uint16_t idx;
    
    if (!codegen || !codegen->context || !codegen->context->labels) {
        return 0xFFFF;
    }
    
    if (codegen->context->labels->count >= 128) {
        return 0xFFFF;
    }
    
    idx = codegen->context->labels->count;
    codegen->context->labels->labels[idx].offset = 0;
    codegen->context->labels->labels[idx].target = -1;
    codegen->context->labels->count++;
    
    return idx;
}

/* Emit label */
int emit_label(CodeGenerator* codegen, uint16_t label_index) {
    if (!codegen || !codegen->context || !codegen->context->labels) {
        return -1;
    }
    
    if (label_index >= codegen->context->labels->count) {
        return -1;
    }
    
    /* Set label target to current code position */
    codegen->context->labels->labels[label_index].target = codegen->context->code->size;
    
    return 0;
}

/* Emit jump */
int emit_jump(CodeGenerator* codegen, uint8_t opcode, uint16_t label_index) {
    if (!codegen || !codegen->context || !codegen->context->labels) {
        return -1;
    }
    
    if (label_index >= codegen->context->labels->count) {
        return -1;
    }
    
    /* Emit jump opcode */
    emit_opcode(codegen, opcode);
    
    /* Record offset for backpatching */
    codegen->context->labels->labels[label_index].offset = codegen->context->code->size;
    
    /* Emit placeholder offset */
    emit_u2(codegen, 0);
    
    return 0;
}

/* Backpatch labels */
int backpatch_labels(CodeGenerator* codegen) {
    uint16_t i;
    Label* label;
    int16_t offset;
    
    if (!codegen || !codegen->context || !codegen->context->labels) {
        return -1;
    }
    
    /* Backpatch all labels */
    for (i = 0; i < codegen->context->labels->count; i++) {
        label = &codegen->context->labels->labels[i];
        
        if (label->offset > 0 && label->target >= 0) {
            /* Calculate relative offset */
            offset = label->target - (label->offset + 2);
            
            /* Patch offset in code */
            codegen->context->code->data[label->offset] = (uint8_t)(offset & 0xFF);
            codegen->context->code->data[label->offset + 1] = (uint8_t)(offset >> 8);
        }
    }
    
    return 0;
}

/* Get local variable index */
uint16_t get_local_index(CodeGenerator* codegen, const char* name) {
    Symbol* sym;
    Symbol* best_sym;
    uint16_t i;
    uint16_t param_count;
    uint16_t best_scope;
    const char* sym_name;
    uint16_t result;
    
    if (!codegen || !name || !codegen->symtable) {
        return 0xFFFF;
    }
    
    /* Prefer the innermost local/param symbol with matching name.
     * Generic symtable_lookup() can return class/method symbols too,
     * because symbols are preserved after scope exit for codegen. */
    best_sym = NULL;
    best_scope = 0;
    
    for (i = 0; i < codegen->symtable->symbol_count; i++) {
        sym = &codegen->symtable->symbols[i];
        if (sym->kind != SYM_LOCAL && sym->kind != SYM_PARAM) {
            continue;
        }
        
        sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
        if (!sym_name) {
            continue;
        }
        
        if (strcmp(sym_name, name) != 0) {
            continue;
        }
        
        if (!best_sym || sym->scope_level >= best_scope) {
            best_sym = sym;
            best_scope = sym->scope_level;
        }
    }
    
    if (!best_sym) {
        return 0xFFFF;
    }
    
    /* Get index based on symbol kind */
    if (best_sym->kind == SYM_PARAM) {
        result = best_sym->data.param_data.index;
        return result;
    } else if (best_sym->kind == SYM_LOCAL) {
        /* Local variables come after parameters */
        if (codegen->current_method) {
            param_count = codegen->current_method->data.method_data.param_count;
            result = param_count + best_sym->data.local_data.index;
            return result;
        }
        result = best_sym->data.local_data.index;
        return result;
    }
    
    return 0xFFFF;
}

/* Update stack depth */
void update_stack(CodeGenerator* codegen, int16_t delta) {
    if (!codegen || !codegen->context) {
        return;
    }
    
    codegen->context->current_stack += delta;
    
    if (codegen->context->current_stack > codegen->context->max_stack) {
        codegen->context->max_stack = codegen->context->current_stack;
    }
}

/* Write .djc file */
int write_djc_file(CodeGenerator* codegen) {
    DJCHeader header;
    uint16_t i;
    
    if (!codegen || !codegen->output_file) {
        return -1;
    }
    
    /* Write header */
    header.magic = DJC_MAGIC;
    header.version = DJC_VERSION;
    header.constant_pool_count = codegen->constants->count;
    header.method_count = codegen->method_count;
    header.field_count = codegen->field_count;
    header.code_size = codegen->bytecode->size;
    
    if (fwrite(&header, sizeof(DJCHeader), 1, codegen->output_file) != 1) {
        return -1;
    }
    
    /* Write constant pool */
    for (i = 0; i < codegen->constants->count; i++) {
        DJCConstant* c = &codegen->constants->constants[i];
        
        if (fwrite(&c->tag, sizeof(uint8_t), 1, codegen->output_file) != 1) {
            return -1;
        }
        if (fwrite(&c->length, sizeof(uint16_t), 1, codegen->output_file) != 1) {
            return -1;
        }
        
        if (c->tag == CONST_UTF8) {
            if (fwrite(c->data.utf8_data, 1, c->length, codegen->output_file) != c->length) {
                return -1;
            }
        } else if (c->tag == CONST_INTEGER) {
            if (fwrite(&c->data.int_value, sizeof(int16_t), 1, codegen->output_file) != 1) {
                return -1;
            }
        }
    }
    
    /* Write method table */
    for (i = 0; i < codegen->method_count; i++) {
        DJCMethod* m = &codegen->methods[i];
        
        /* Write each field individually to avoid padding issues */
        if (fwrite(&m->name_index, sizeof(uint16_t), 1, codegen->output_file) != 1) return -1;
        if (fwrite(&m->descriptor_index, sizeof(uint16_t), 1, codegen->output_file) != 1) return -1;
        if (fwrite(&m->code_offset, sizeof(uint16_t), 1, codegen->output_file) != 1) return -1;
        if (fwrite(&m->code_length, sizeof(uint16_t), 1, codegen->output_file) != 1) return -1;
        if (fwrite(&m->max_stack, sizeof(uint8_t), 1, codegen->output_file) != 1) return -1;
        if (fwrite(&m->max_locals, sizeof(uint8_t), 1, codegen->output_file) != 1) return -1;
        if (fwrite(&m->flags, sizeof(uint8_t), 1, codegen->output_file) != 1) return -1;
    }
    
    /* Write field table */
    for (i = 0; i < codegen->field_count; i++) {
        DJCField* f = &codegen->fields[i];
        
        /* Write each field individually to avoid padding issues */
        if (fwrite(&f->name_index, sizeof(uint16_t), 1, codegen->output_file) != 1) return -1;
        if (fwrite(&f->descriptor_index, sizeof(uint16_t), 1, codegen->output_file) != 1) return -1;
        if (fwrite(&f->flags, sizeof(uint8_t), 1, codegen->output_file) != 1) return -1;
    }
    
    /* Write bytecode */
    if (codegen->bytecode->size > 0) {
        if (fwrite(codegen->bytecode->data, 1, codegen->bytecode->size, codegen->output_file) != codegen->bytecode->size) {
            return -1;
        }
    }
    
    return 0;
}


