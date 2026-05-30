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
static uint16_t get_expression_type(CodeGenerator* codegen, ASTNode* expr_node);
static uint16_t get_array_element_type(CodeGenerator* codegen, ASTNode* array_node);
static int generate_switch_stmt(CodeGenerator* codegen, ASTNode* switch_node);
static int generate_case_comparison(CodeGenerator* codegen, ASTNode* case_node, uint16_t switch_expr_type,
                                    uint16_t end_label, uint16_t this_body_label, uint16_t next_body_label);

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
    
    /* Allocate line number table structure */
    codegen->line_numbers = (LineNumberTable*)malloc(sizeof(LineNumberTable));
    if (!codegen->line_numbers) {
        free(codegen->bytecode->data);
        free(codegen->bytecode);
        free(codegen->constants);
        fclose(codegen->ast_file);
        fclose(codegen->output_file);
        symtable_cleanup(codegen->symtable);
        free(codegen->symtable);
        return -1;
    }
    memset(codegen->line_numbers, 0, sizeof(LineNumberTable));
    
    /* Allocate line number entries (initial capacity: 256 entries) */
    codegen->line_numbers->capacity = 256;
    codegen->line_numbers->entries = (CodeGenLineEntry*)malloc(
        sizeof(CodeGenLineEntry) * codegen->line_numbers->capacity);
    if (!codegen->line_numbers->entries) {
        free(codegen->line_numbers);
        free(codegen->bytecode->data);
        free(codegen->bytecode);
        free(codegen->constants);
        fclose(codegen->ast_file);
        fclose(codegen->output_file);
        symtable_cleanup(codegen->symtable);
        free(codegen->symtable);
        return -1;
    }
    codegen->line_numbers->count = 0;
    codegen->line_numbers->current_line = 0;
    
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
        /* Ensure all data is flushed before closing */
        fflush(codegen->output_file);
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
    
    if (codegen->line_numbers) {
        if (codegen->line_numbers->entries) {
            free(codegen->line_numbers->entries);
        }
        free(codegen->line_numbers);
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
    
    /* Match semantic_get_node(): node_count is reused as a stable on-disk
     * offset to the first AST node. The old code used mutable pool_size,
     * but semantic analysis can extend the shared string pool and write that
     * enlarged pool to the symbol file only. Codegen must still seek using
     * the original AST file layout.
     */
    file_pos = ftell(codegen->ast_file);
    if (file_pos < 0) {
        return NULL;
    }
    if (codegen->node_count == 0) {
        codegen->node_count = (uint16_t)file_pos;
    }
    
    file_pos = (long)codegen->node_count + (long)(node_index - 1) * (long)sizeof(ASTNode);
    fseek(codegen->ast_file, file_pos, SEEK_SET);
    
    if (fread(&codegen->nodes[0], sizeof(ASTNode), 1, codegen->ast_file) != 1) {
        return NULL;
    }
    
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
/**
 * Add line number entry
 * Records the mapping between bytecode PC and source line number
 */
static void add_line_number_entry(CodeGenerator* codegen, uint16_t line_no) {
    CodeGenLineEntry* new_entries;
    uint16_t new_capacity;
    uint16_t current_pc;
    
    if (!codegen || !codegen->line_numbers || !codegen->bytecode) {
        return;
    }
    
    /* Skip if line number hasn't changed */
    if (line_no == codegen->line_numbers->current_line) {
        return;
    }
    
    /* Get current PC (bytecode offset)
     * If we're in a method context, add the global bytecode size to get the absolute PC
     * Otherwise, just use the global bytecode size */
    if (codegen->context && codegen->context->code) {
        current_pc = codegen->bytecode->size + codegen->context->code->size;
    } else {
        current_pc = codegen->bytecode->size;
    }
    
    /* Expand capacity if needed */
    if (codegen->line_numbers->count >= codegen->line_numbers->capacity) {
        new_capacity = codegen->line_numbers->capacity * 2;
        new_entries = (CodeGenLineEntry*)realloc(
            codegen->line_numbers->entries,
            sizeof(CodeGenLineEntry) * new_capacity);
        if (!new_entries) {
            return;  /* Silently fail - line numbers are optional */
        }
        codegen->line_numbers->entries = new_entries;
        codegen->line_numbers->capacity = new_capacity;
    }
    
    /* Add new entry */
    codegen->line_numbers->entries[codegen->line_numbers->count].pc = current_pc;
    codegen->line_numbers->entries[codegen->line_numbers->count].line_no = line_no;
    codegen->line_numbers->count++;
    codegen->line_numbers->current_line = line_no;
}

/**
 * Update current line number from AST node
 * Should be called before generating code for each statement
 */
static void update_line_number(CodeGenerator* codegen, ASTNode* node) {
    if (!codegen || !node) {
        return;
    }
    
    /* Add line number entry if line has changed */
    if (node->line > 0) {
        add_line_number_entry(codegen, node->line);
    }
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
    codegen->current_method = NULL;
    {
        uint16_t i;
        for (i = 0; i < codegen->symtable->symbol_count; i++) {
            Symbol* sym = &codegen->symtable->symbols[i];
            const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
            if (sym->kind == SYM_METHOD && sym_name && strcmp(sym_name, method_name) == 0) {
                codegen->current_method = sym;
                break;
            }
        }
    }
    if (!codegen->current_method) {
        codegen_error(codegen, "Method symbol not found");
        return -1;
    }
    
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
    
    /* Allocate code buffer data separately (4KB - reduced for DOS memory constraints) */
    codegen->context->code->capacity = 4096;
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
        
        /* Search for existing non-native placeholder entry.
         * Native methods can share the same name (e.g. println(int) and
         * println(String)), and user methods must never overwrite them.
         */
        name_idx = find_or_add_utf8(codegen, method_name);
        for (method_idx = 0; method_idx < codegen->method_count; method_idx++) {
            if ((codegen->methods[method_idx].flags & METHOD_NATIVE) == 0 &&
                codegen->methods[method_idx].name_index == name_idx) {
                /* Found existing user-method entry - update it */
                codegen->methods[method_idx].descriptor_index =
                    build_method_descriptor(codegen, codegen->current_method);
                codegen->methods[method_idx].code_offset = code_start;
                codegen->methods[method_idx].code_length = code_length;
                codegen->methods[method_idx].max_stack = codegen->context->max_stack;
                {
                    uint8_t base_locals = (uint8_t)((codegen->current_method->data.method_data.param_count >
                                                     codegen->context->max_locals)
                                                        ? codegen->current_method->data.method_data.param_count
                                                        : codegen->context->max_locals);
                    /* For instance methods, add 1 for 'this' parameter */
                    codegen->methods[method_idx].max_locals = is_static ? base_locals : (base_locals + 1);
                }
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
            codegen->methods[codegen->method_count].descriptor_index =
                build_method_descriptor(codegen, codegen->current_method);
            codegen->methods[codegen->method_count].code_offset = code_start;
            codegen->methods[codegen->method_count].code_length = code_length;
            codegen->methods[codegen->method_count].max_stack = codegen->context->max_stack;
            {
                uint8_t base_locals = (uint8_t)((codegen->current_method->data.method_data.param_count >
                                                 codegen->context->max_locals)
                                                    ? codegen->current_method->data.method_data.param_count
                                                    : codegen->context->max_locals);
                /* For instance methods, add 1 for 'this' parameter */
                codegen->methods[codegen->method_count].max_locals = is_static ? base_locals : (base_locals + 1);
            }
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
    
    /* Update line number for this statement */
    update_line_number(codegen, stmt_node);
    
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
                ASTNode expr_copy;
                uint16_t expr_type;
                int is_long_assign = 0;
                int is_float_assign = 0;
                
                memcpy(&expr_copy, expr_node, sizeof(ASTNode));
                expr_type = expr_copy.type;
                
                /* Check if this is a long or float assignment */
                if (expr_type == NODE_ASSIGN) {
                    ASTNode* target_node = codegen_get_node(codegen, expr_copy.data.assign.target);
                    if (target_node && target_node->type == NODE_IDENTIFIER) {
                        const char* var_name = codegen_get_string(codegen, target_node->data.identifier.name);
                        if (var_name) {
                            uint16_t i;
                            for (i = 0; i < codegen->symtable->symbol_count; i++) {
                                Symbol* sym = &codegen->symtable->symbols[i];
                                const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                                if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                                    sym_name && strcmp(sym_name, var_name) == 0) {
                                    if (sym->type.kind == TYPE_LONG) {
                                        is_long_assign = 1;
                                    } else if (sym->type.kind == TYPE_FLOAT) {
                                        is_float_assign = 1;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
                
                generate_expression(codegen, &expr_copy);

                if (expr_type == NODE_BINARY_OP ||
                    expr_type == NODE_UNARY_OP ||
                    expr_type == NODE_IDENTIFIER ||
                    expr_type == NODE_LITERAL_INT ||
                    expr_type == NODE_LITERAL_BOOL ||
                    expr_type == NODE_LITERAL_STRING ||
                    expr_type == NODE_NEW ||
                    expr_type == NODE_ARRAY_ACCESS ||
                    expr_type == NODE_FIELD_ACCESS ||
                    expr_type == NODE_ASSIGN) {
                    if (is_long_assign || is_float_assign) {
                        /* Long/float assignment leaves 2 words on stack, pop both */
                        emit_opcode(codegen, OP_POP);
                        update_stack(codegen, -1);
                        emit_opcode(codegen, OP_POP);
                        update_stack(codegen, -1);
                    } else {
                        emit_opcode(codegen, OP_POP);
                        update_stack(codegen, -1);
                    }
                }
            }
            return 0;
        }
        
        case NODE_TRY: {
            /* Try-catch-finally implementation with forward reference */
            ASTNode* try_block_node;
            ASTNode* catch_node;
            ASTNode* catch_block_node;
            ASTNode* finally_node;
            ASTNode* finally_block_node;
            ASTNode try_block_copy;
            ASTNode catch_block_copy;
            uint16_t try_begin_pos;
            uint16_t catch_begin_pos;
            uint16_t catch_offset;
            uint16_t try_block_idx;
            uint16_t catch_block_idx;
            
            /* Emit OP_TRY_BEGIN with placeholder offset */
            try_begin_pos = codegen->context->code->size;
            emit_opcode(codegen, OP_TRY_BEGIN);
            emit_u2(codegen, 0);  /* Placeholder for catch offset */
            
            /* Generate try block */
            try_block_idx = stmt_node->data.try_stmt.try_block;
            try_block_node = codegen_get_node(codegen, try_block_idx);
            if (try_block_node) {
                memcpy(&try_block_copy, try_block_node, sizeof(ASTNode));
                generate_statement(codegen, &try_block_copy);
            } else {
                /* Try block node is NULL - this is an error */
                codegen_error(codegen, "Try block node is NULL");
                return -1;
            }
            emit_opcode(codegen, OP_TRY_END);
            
            /* Generate catch block if present */
            if (stmt_node->data.try_stmt.catch_clause != 0) {
                uint16_t skip_catch_label;
                
                /* Emit GOTO to skip catch block if no exception */
                skip_catch_label = create_label(codegen);
                emit_jump(codegen, OP_GOTO, skip_catch_label);
                
                emit_opcode(codegen, OP_CATCH_BEGIN);
                
                /* Record catch block code position AFTER emitting OP_CATCH_BEGIN */
                catch_begin_pos = codegen->context->code->size;
                
                /* Patch the catch offset in OP_TRY_BEGIN */
                catch_offset = catch_begin_pos;
                codegen->context->code->data[try_begin_pos + 1] = (uint8_t)(catch_offset & 0xFF);
                codegen->context->code->data[try_begin_pos + 2] = (uint8_t)((catch_offset >> 8) & 0xFF);
                
                catch_node = codegen_get_node(codegen, stmt_node->data.try_stmt.catch_clause);
                if (catch_node && catch_node->data.catch_clause.exception_var != 0) {
                    const char* exception_var_name;
                    uint16_t var_index;
                    
                    /* Get exception variable name from string pool */
                    exception_var_name = codegen_get_string(codegen, catch_node->data.catch_clause.exception_var);
                    if (exception_var_name) {
                        /* Get local variable index for exception */
                        var_index = get_local_index(codegen, exception_var_name);
                        
                        if (var_index != 0xFFFF) {
                            /* Store exception reference from stack to local variable */
                            emit_opcode(codegen, OP_STORE_LOCAL);
                            emit_u1(codegen, (uint8_t)var_index);
                        }
                    }
                }
                if (catch_node) {
                    catch_block_idx = catch_node->data.catch_clause.catch_block;
                    catch_block_node = codegen_get_node(codegen, catch_block_idx);
                    if (catch_block_node) {
                        memcpy(&catch_block_copy, catch_block_node, sizeof(ASTNode));
                        generate_statement(codegen, &catch_block_copy);
                    } else {
                        /* Catch block node is NULL - this is an error */
                        codegen_error(codegen, "Catch block node is NULL");
                        return -1;
                    }
                } else {
                    /* Catch clause node is NULL - this is an error */
                    codegen_error(codegen, "Catch clause node is NULL");
                    return -1;
                }
                emit_opcode(codegen, OP_CATCH_END);
                
                /* Emit label after catch block */
                emit_label(codegen, skip_catch_label);
            }
            
            /* Generate finally block if present */
            if (stmt_node->data.try_stmt.finally_block != 0) {
                emit_opcode(codegen, OP_FINALLY_BEGIN);
                finally_node = codegen_get_node(codegen, stmt_node->data.try_stmt.finally_block);
                if (finally_node) {
                    finally_block_node = codegen_get_node(codegen, finally_node->data.finally_block.finally_block);
                    if (finally_block_node) {
                        generate_statement(codegen, finally_block_node);
                    }
                }
                emit_opcode(codegen, OP_FINALLY_END);
            }
            
            return 0;
        }
        
        case NODE_THROW: {
            /* Generate throw statement */
            ASTNode* exception_expr_node = codegen_get_node(codegen, stmt_node->data.throw_stmt.exception_expr);
            if (exception_expr_node) {
                /* Evaluate exception expression and push to stack */
                generate_expression(codegen, exception_expr_node);
                /* Emit throw opcode */
                emit_opcode(codegen, OP_THROW);
                update_stack(codegen, -1);  /* Pop exception from stack */
            }
            return 0;
        }
        
        case NODE_SWITCH:
            return generate_switch_stmt(codegen, stmt_node);
        
        case NODE_BREAK:
            /* Generate GOTO to break label */
            if (codegen->break_label == 0) {
                codegen_error(codegen, "break statement not in loop or switch");
                return -1;
            }
            emit_jump(codegen, OP_GOTO, codegen->break_label);
            return 0;
        
        case NODE_CONTINUE:
            /* Generate GOTO to continue label */
            if (codegen->continue_label == 0) {
                codegen_error(codegen, "continue statement not in loop");
                return -1;
            }
            emit_jump(codegen, OP_GOTO, codegen->continue_label);
            return 0;
        
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
        
        /* Copy stmt_node to avoid corruption during generate_statement */
        {
            ASTNode stmt_copy;
            memcpy(&stmt_copy, stmt_node, sizeof(ASTNode));
            generate_statement(codegen, &stmt_copy);
        }
        
        stmt_idx = next_sibling;
        stmt_count++;
        
    }
    
    
    
    
    
    return 0;
}

/* Generate code for variable declaration */
int generate_var_decl(CodeGenerator* codegen, ASTNode* var_node) {
    ASTNode* init_expr;
    const char* var_name;
    char var_name_copy[64];
    uint16_t local_idx;
    uint16_t init_expr_idx;
    int is_long;
    int is_float;
    uint16_t type_kind;
    uint16_t symbol_index;
    
    if (!codegen || !var_node) {
        return -1;
    }
    
    /* Get variable name and save init_expr index before reading other nodes */
    var_name = codegen_get_string(codegen, var_node->data.var_decl.name);
    init_expr_idx = var_node->data.var_decl.init_expr;
    
    if (!var_name) {
        return -1;
    }
    
    /* CRITICAL: Copy variable name to local buffer to protect from buffer reuse */
    strncpy(var_name_copy, var_name, sizeof(var_name_copy) - 1);
    var_name_copy[sizeof(var_name_copy) - 1] = '\0';
    var_name = var_name_copy;

    /* Get local index */
    local_idx = get_local_index(codegen, var_name);
    
    /* Check if this is a long or float variable (both use 2 slots) */
    /* IMPORTANT: Find symbol with highest scope_level (innermost scope) */
    is_long = 0;
    is_float = 0;
    symbol_index = 0xFFFF;
    {
        uint16_t i;
        uint16_t best_scope = 0;
        for (i = 0; i < codegen->symtable->symbol_count; i++) {
            Symbol* sym = &codegen->symtable->symbols[i];
            const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
            if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                sym_name && strcmp(sym_name, var_name) == 0) {
                /* Choose symbol with highest scope_level (innermost scope) */
                if (symbol_index == 0xFFFF || sym->scope_level >= best_scope) {
                    symbol_index = i;
                    best_scope = sym->scope_level;
                }
            }
        }
        
        /* Now access the symbol using the saved index */
        if (symbol_index != 0xFFFF) {
            Symbol* sym = &codegen->symtable->symbols[symbol_index];
            type_kind = sym->type.kind;
            if (type_kind == TYPE_LONG) {
                is_long = 1;
            } else if (type_kind == TYPE_FLOAT) {
                is_float = 1;
            }
        }
        /* Update max_locals (long and float variables use 2 slots) */
        if (is_long || is_float) {
            if (local_idx + 2 > codegen->context->max_locals) {
                codegen->context->max_locals = local_idx + 2;
            }
        } else {
            if (local_idx + 1 > codegen->context->max_locals) {
                codegen->context->max_locals = local_idx + 1;
            }
        }
        
        /* Generate initializer if present */
        if (init_expr_idx != 0) {
            init_expr = codegen_get_node(codegen, init_expr_idx);
            if (init_expr) {
                generate_expression(codegen, init_expr);
                
                /* Store to local variable */
                if (is_long) {
                    emit_opcode(codegen, OP_STORE_LONG);
                    emit_u1(codegen, (uint8_t)local_idx);
                    update_stack(codegen, -2);
                } else if (is_float) {
                    emit_opcode(codegen, OP_STORE_FLOAT);
                    emit_u1(codegen, (uint8_t)local_idx);
                    update_stack(codegen, -2);
                } else {
                    if (local_idx <= 2) {
                        emit_opcode(codegen, OP_STORE_0 + local_idx);
                    } else {
                        emit_opcode(codegen, OP_STORE_LOCAL);
                        emit_u1(codegen, (uint8_t)local_idx);
                    }
                    update_stack(codegen, -1);
                }
            }
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
    uint16_t saved_break_label;
    uint16_t saved_continue_label;
    
    if (!codegen || !while_node) {
        return -1;
    }
    
    /* CRITICAL: Save all indices BEFORE any recursive calls */
    condition_index = while_node->data.while_stmt.condition;
    body_index = while_node->data.while_stmt.body;
    
    /* Create labels */
    loop_start = create_label(codegen);
    loop_end = create_label(codegen);
    
    /* Save previous break/continue labels */
    saved_break_label = codegen->break_label;
    saved_continue_label = codegen->continue_label;
    
    /* Set break/continue labels for this loop */
    codegen->break_label = loop_end;
    codegen->continue_label = loop_start;
    
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
    
    /* Restore previous break/continue labels */
    codegen->break_label = saved_break_label;
    codegen->continue_label = saved_continue_label;
    
    return 0;
}

/* Generate code for for statement */
int generate_for_stmt(CodeGenerator* codegen, ASTNode* for_node) {
    ASTNode* init_node;
    ASTNode* cond_node;
    ASTNode* update_node;
    ASTNode* body_node;
    uint16_t loop_start;
    uint16_t loop_update;
    uint16_t loop_end;
    uint16_t init_index;
    uint16_t condition_index;
    uint16_t update_index;
    uint16_t body_index;
    uint16_t saved_break_label;
    uint16_t saved_continue_label;
    
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
            if (init_node->type == NODE_VAR_DECL) {
                if (generate_var_decl(codegen, init_node) != 0) {
                    return -1;
                }
            } else {
                if (generate_expression(codegen, init_node) != 0) {
                    return -1;
                }
                /* Pop the result if it's an expression statement */
                emit_opcode(codegen, OP_POP);
                update_stack(codegen, -1);
            }
        }
    }
    
    /* Create labels */
    loop_start = create_label(codegen);
    loop_update = create_label(codegen);
    loop_end = create_label(codegen);
    
    /* Save previous break/continue labels */
    saved_break_label = codegen->break_label;
    saved_continue_label = codegen->continue_label;
    
    /* Set break/continue labels for this loop */
    codegen->break_label = loop_end;
    codegen->continue_label = loop_update;
    
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
    
    /* Update label (for continue) */
    emit_label(codegen, loop_update);
    
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
    
    /* Restore previous break/continue labels */
    codegen->break_label = saved_break_label;
    codegen->continue_label = saved_continue_label;
    
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
        
        case NODE_LITERAL_LONG: {
            /* Push 32-bit long constant [high:2] [low:2] */
            int32_t long_val = expr_node->data.literal_long.long_value;
            uint16_t high = (uint16_t)((long_val >> 16) & 0xFFFF);
            uint16_t low = (uint16_t)(long_val & 0xFFFF);
            emit_opcode(codegen, OP_PUSH_LONG);
            emit_u2(codegen, high);
            emit_u2(codegen, low);
            update_stack(codegen, 2);  /* Long takes 2 stack slots */
            return 0;
        }
        
        case NODE_LITERAL_FLOAT: {
            /* Push 32-bit float constant [high:2] [low:2] */
            float float_val = expr_node->data.literal_float.float_value;
            uint32_t float_bits;
            uint16_t high, low;
            
            /* Convert float to bits using memcpy for safe type punning */
            memcpy(&float_bits, &float_val, sizeof(float));
            high = (uint16_t)((float_bits >> 16) & 0xFFFF);
            low = (uint16_t)(float_bits & 0xFFFF);
            
            emit_opcode(codegen, OP_FCONST);
            emit_u2(codegen, high);
            emit_u2(codegen, low);
            update_stack(codegen, 2);  /* Float takes 2 stack slots */
            return 0;
        }
        
        case NODE_LITERAL_BOOL:
            emit_opcode(codegen, OP_PUSH_INT);
            emit_u2(codegen, expr_node->data.literal_bool.bool_value);
            update_stack(codegen, 1);
            return 0;
        
        case NODE_LITERAL_NULL:
            emit_opcode(codegen, OP_ACONST_NULL);
            update_stack(codegen, 1);
            return 0;
        
        case NODE_LITERAL_STRING: {
            const char* str_value;
            uint16_t const_idx;
            uint16_t literal_off;
            
            literal_off = expr_node->data.literal_string.str_offset;
            if (literal_off < codegen->pool_size) {
                str_value = &codegen->string_pool[literal_off];
            } else {
                str_value = NULL;
            }
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
        
        case NODE_THIS:
            /* 'this' keyword - load local variable 0 (this reference) */
            emit_opcode(codegen, OP_LOAD_0);  /* Optimized: load local[0] */
            update_stack(codegen, 1);
            return 0;
        
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
            uint16_t class_name_value;
            uint16_t size_idx;
            
            /* Save class_name value before any codegen_get_node calls */
            class_name_value = expr_node->data.new_expr.class_name;
            size_idx = expr_node->next_sibling;
            
            /* Check if it's an array (class_name stores element type) */
            if (class_name_value == TYPE_INT || class_name_value == TYPE_BOOLEAN ||
                class_name_value == TYPE_LONG || class_name_value == TYPE_FLOAT) {
                /* Array creation: new int[size], new boolean[size], new long[size], or new float[size] */
                ASTNode* size_node;
                uint8_t elem_type = (uint8_t)class_name_value;
                
                size_node = codegen_get_node(codegen, size_idx);
                if (!size_node) {
                    codegen_error(codegen, "Invalid array size expression");
                    return -1;
                }
                generate_expression(codegen, size_node);
                
                /* Emit appropriate opcode based on element type */
                if (elem_type == TYPE_LONG) {
                    emit_opcode(codegen, OP_NEW_LONG_ARRAY);
                } else if (elem_type == TYPE_FLOAT) {
                    emit_opcode(codegen, OP_NEW_FLOAT_ARRAY);
                } else {
                    emit_opcode(codegen, OP_NEW_ARRAY);
                    emit_u1(codegen, elem_type);
                }
                return 0;
            }
            
            /* Object creation: new ClassName(args...) */
            {
                const char* class_name;
                uint16_t class_idx;
                uint16_t arg_count;
                uint16_t arg_idx;
                ASTNode* arg_node;
                
                class_name = codegen_get_string(codegen, class_name_value);
                if (!class_name) {
                    codegen_error(codegen, "Invalid class name in new expression");
                    return -1;
                }
                
                /* Add class name to constant pool */
                class_idx = find_or_add_utf8(codegen, class_name);
                if (class_idx == 0xFFFF) {
                    codegen_error(codegen, "Failed to add class constant");
                    return -1;
                }
                
                /* Generate code for constructor arguments */
                arg_count = expr_node->data.new_expr.arg_count;
                arg_idx = expr_node->data.new_expr.first_arg;
                
                /* Count actual words needed (long types need 2 words) */
                {
                    uint16_t word_count = 0;
                    uint16_t temp_idx = arg_idx;
                    uint16_t arg_type;
                    ASTNode* temp_node;
                    
                    while (temp_idx != 0) {
                        temp_node = codegen_get_node(codegen, temp_idx);
                        if (!temp_node) {
                            codegen_error(codegen, "Invalid constructor argument");
                            return -1;
                        }
                        
                        /* Check if argument is long type using type detection */
                        arg_type = get_expression_type(codegen, temp_node);
                        if (arg_type == TYPE_LONG) {
                            word_count += 2;  /* Long needs 2 words */
                        } else {
                            word_count += 1;  /* Other types need 1 word */
                        }
                        
                        temp_idx = temp_node->next_sibling;
                    }
                    
                    /* Generate code for each argument */
                    while (arg_idx != 0) {
                        arg_node = codegen_get_node(codegen, arg_idx);
                        if (!arg_node) {
                            codegen_error(codegen, "Invalid constructor argument");
                            return -1;
                        }
                        
                        if (generate_expression(codegen, arg_node) != 0) {
                            return -1;
                        }
                        
                        arg_idx = arg_node->next_sibling;
                    }
                    
                    /* Emit OP_NEW with class index and word count */
                    emit_opcode(codegen, OP_NEW);
                    emit_u2(codegen, class_idx);
                    emit_u1(codegen, (uint8_t)word_count);
                    update_stack(codegen, 1 - word_count);  /* Args consumed, object pushed */
                }
                return 0;
            }
        }
        
        case NODE_ARRAY_ACCESS: {
            ASTNode array_expr_copy;
            ASTNode index_expr_copy;
            uint16_t array_idx = expr_node->data.array_access.array;
            uint16_t index_idx = expr_node->data.array_access.index;
            uint16_t elem_type;
            
            /* Copy array node before any codegen_get_node calls */
            {
                ASTNode* array_src = codegen_get_node(codegen, array_idx);
                if (!array_src) {
                    codegen_error(codegen, "Invalid array access");
                    return -1;
                }
                memcpy(&array_expr_copy, array_src, sizeof(ASTNode));
            }
            
            /* Copy index node */
            {
                ASTNode* index_src = codegen_get_node(codegen, index_idx);
                if (!index_src) {
                    codegen_error(codegen, "Invalid array access");
                    return -1;
                }
                memcpy(&index_expr_copy, index_src, sizeof(ASTNode));
            }
            
            /* Generate array and index expressions */
            generate_expression(codegen, &array_expr_copy);
            generate_expression(codegen, &index_expr_copy);
            
            /* Update line number before array access (where exception may occur) */
            update_line_number(codegen, expr_node);
            
            /* Determine element type and emit appropriate load opcode */
            elem_type = get_array_element_type(codegen, &array_expr_copy);
            if (elem_type == TYPE_LONG) {
                emit_opcode(codegen, OP_LARRAY_LOAD);
                update_stack(codegen, 0);  /* Pops 2 (array, index), pushes 2 (high, low) */
            } else if (elem_type == TYPE_FLOAT) {
                emit_opcode(codegen, OP_FARRAY_LOAD);
                update_stack(codegen, 0);  /* Pops 2 (array, index), pushes 2 (high, low) */
            } else {
                emit_opcode(codegen, OP_ARRAY_LOAD);
                update_stack(codegen, -1);  /* Pops 2 (array, index), pushes 1 (value) */
            }
            return 0;
        }
        
        case NODE_FIELD_ACCESS: {
            const char* field_name;
            ASTNode* object_node;
            uint16_t field_name_off = expr_node->data.field_access.field_name;
            uint16_t object_idx = expr_node->data.field_access.object;
            uint16_t field_idx;
            Symbol* field_sym;
            uint16_t i;
            
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
            
            /* Array.length special case */
            if (strcmp(field_name, "length") == 0) {
                generate_expression(codegen, object_node);
                emit_opcode(codegen, OP_ARRAY_LENGTH);
                return 0;
            }
            
            /* Object field access */
            /* Generate object reference */
            generate_expression(codegen, object_node);
            
            /* Find field symbol */
            field_sym = NULL;
            for (i = 0; i < codegen->symtable->symbol_count; i++) {
                Symbol* sym = &codegen->symtable->symbols[i];
                const char* sym_name;
                
                if (sym->kind != SYM_FIELD) {
                    continue;
                }
                
                sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                if (sym_name && strcmp(sym_name, field_name) == 0) {
                    field_sym = sym;
                    break;
                }
            }
            
            if (!field_sym) {
                codegen_error(codegen, "Undefined field in code generation");
                return -1;
            }
            
            /* Add field name to constant pool */
            field_idx = find_or_add_utf8(codegen, field_name);
            if (field_idx == 0xFFFF) {
                codegen_error(codegen, "Failed to add field constant");
                return -1;
            }
            
            /* Emit OP_GET_FIELD */
            emit_opcode(codegen, OP_GET_FIELD);
            emit_u2(codegen, field_idx);
            /* Stack: object -> value (no change in depth) */
            return 0;
        }
        
        case NODE_CAST: {
            ASTNode* cast_expr_node;
            TypeInfo target_type;
            uint16_t expr_idx;
            TypeKind source_kind;
            uint16_t i;
            const char* var_name;
            const char* method_name;
            
            /* Save cast data before any codegen_get_node calls */
            expr_idx = expr_node->data.cast.expr;
            target_type = expr_node->data.cast.target_type;
            
            /* Generate code for the expression being cast */
            cast_expr_node = codegen_get_node(codegen, expr_idx);
            if (!cast_expr_node) {
                codegen_error(codegen, "Invalid cast expression");
                return -1;
            }
            
            /* Determine source type from expression */
            source_kind = TYPE_VOID;
            
            if (cast_expr_node->type == NODE_LITERAL_INT || cast_expr_node->type == NODE_LITERAL_BOOL) {
                source_kind = TYPE_INT;
            } else if (cast_expr_node->type == NODE_LITERAL_LONG) {
                source_kind = TYPE_LONG;
            } else if (cast_expr_node->type == NODE_LITERAL_FLOAT) {
                source_kind = TYPE_FLOAT;
            } else if (cast_expr_node->type == NODE_IDENTIFIER) {
                /* Look up variable type in symbol table */
                var_name = codegen_get_string(codegen, cast_expr_node->data.identifier.name);
                if (var_name && codegen->symtable) {
                    for (i = 0; i < codegen->symtable->symbol_count; i++) {
                        Symbol* sym = &codegen->symtable->symbols[i];
                        const char* sym_name;
                        
                        if (sym->kind != SYM_LOCAL && sym->kind != SYM_PARAM && sym->kind != SYM_FIELD) {
                            continue;
                        }
                        
                        sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if (sym_name && strcmp(sym_name, var_name) == 0) {
                            source_kind = sym->type.kind;
                            break;
                        }
                    }
                }
            } else if (cast_expr_node->type == NODE_CALL) {
                /* Determine return type of method call */
                method_name = codegen_get_string(codegen, cast_expr_node->data.call.method_name);
                if (method_name) {
                    /* Check if it's a Math method that returns float */
                    if (strcmp(method_name, "abs") == 0 ||
                        strcmp(method_name, "min") == 0 ||
                        strcmp(method_name, "max") == 0 ||
                        strcmp(method_name, "sqrt") == 0 ||
                        strcmp(method_name, "sin") == 0 ||
                        strcmp(method_name, "cos") == 0 ||
                        strcmp(method_name, "tan") == 0 ||
                        strcmp(method_name, "pow") == 0 ||
                        strcmp(method_name, "exp") == 0 ||
                        strcmp(method_name, "log") == 0 ||
                        strcmp(method_name, "floor") == 0 ||
                        strcmp(method_name, "ceil") == 0) {
                        source_kind = TYPE_FLOAT;
                    }
                    /* Add more method return types as needed */
                }
            }
            
            /* Generate the expression */
            if (generate_expression(codegen, cast_expr_node) != 0) {
                return -1;
            }
            
            /* Emit appropriate cast opcode based on source and target types */
            if (source_kind == TYPE_INT && target_type.kind == TYPE_LONG) {
                emit_opcode(codegen, OP_I2L);
                update_stack(codegen, 1);  /* int (1 slot) -> long (2 slots) */
            } else if (source_kind == TYPE_INT && target_type.kind == TYPE_FLOAT) {
                emit_opcode(codegen, OP_I2F);
                update_stack(codegen, 1);  /* int (1 slot) -> float (2 slots) */
            } else if (source_kind == TYPE_LONG && target_type.kind == TYPE_INT) {
                emit_opcode(codegen, OP_L2I);
                update_stack(codegen, -1);  /* long (2 slots) -> int (1 slot) */
            } else if (source_kind == TYPE_LONG && target_type.kind == TYPE_FLOAT) {
                emit_opcode(codegen, OP_L2F);
                /* long (2 slots) -> float (2 slots), no stack change */
            } else if (source_kind == TYPE_FLOAT && target_type.kind == TYPE_INT) {
                emit_opcode(codegen, OP_F2I);
                update_stack(codegen, -1);  /* float (2 slots) -> int (1 slot) */
            } else if (source_kind == TYPE_FLOAT && target_type.kind == TYPE_LONG) {
                emit_opcode(codegen, OP_F2L);
                /* float (2 slots) -> long (2 slots), no stack change */
            }
            /* If source == target, no cast opcode needed (identity cast) */
            
            return 0;
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
    int is_string_concat;
    int left_is_string;
    int right_is_string;
    int left_is_int;
    int right_is_int;
    int left_is_long;
    int left_is_float;
    int right_is_long;
    int right_is_float;
    
    if (!codegen || !binop_node) {
        return -1;
    }
    
    /* CRITICAL: Save operator and operand indices BEFORE any recursive calls
     * because generate_expression() may overwrite the binop_node memory */
    op = binop_node->data.binary_op.op;
    left_index = binop_node->data.binary_op.left;
    right_index = binop_node->data.binary_op.right;
    
    /* Handle logical operators with short-circuit evaluation */
    if (op == (uint16_t)BINOP_AND || op == (uint16_t)BINOP_OR) {
        uint16_t skip_label;
        uint16_t end_label;
        
        /* Generate left operand */
        left_node = codegen_get_node(codegen, left_index);
        if (left_node) {
            generate_expression(codegen, left_node);
        }
        
        /* Duplicate for the result */
        emit_opcode(codegen, OP_DUP);
        update_stack(codegen, 1);
        
        /* Create labels */
        skip_label = codegen->context->code->size;
        
        if (op == (uint16_t)BINOP_AND) {
            /* AND: if left is false, skip right evaluation */
            emit_opcode(codegen, OP_IF_FALSE);
            emit_u2(codegen, 0);  /* Placeholder for offset */
        } else {
            /* OR: if left is true, skip right evaluation */
            emit_opcode(codegen, OP_IF_TRUE);
            emit_u2(codegen, 0);  /* Placeholder for offset */
        }
        
        /* Pop the duplicated value */
        emit_opcode(codegen, OP_POP);
        update_stack(codegen, -1);
        
        /* Generate right operand */
        right_node = codegen_get_node(codegen, right_index);
        if (right_node) {
            generate_expression(codegen, right_node);
        }
        
        /* Patch the jump offset */
        end_label = codegen->context->code->size;
        {
            int16_t offset = (int16_t)(end_label - skip_label - 3);
            codegen->context->code->data[skip_label + 1] = (uint8_t)(offset & 0xFF);
            codegen->context->code->data[skip_label + 2] = (uint8_t)((offset >> 8) & 0xFF);
        }
        
        update_stack(codegen, -1); /* Two operands consumed, one result produced */
        return 0;
    }
    
    is_string_concat = 0;
    left_is_string = 0;
    right_is_string = 0;
    left_is_int = 0;
    right_is_int = 0;
    left_is_long = 0;
    left_is_float = 0;
    right_is_long = 0;
    right_is_float = 0;
    
    if (op == (uint16_t)BINOP_ADD) {
        ASTNode* test_node;
        uint16_t left_type;
        uint16_t right_type;
        
        /* Get left operand type (must save before getting right operand) */
        test_node = codegen_get_node(codegen, left_index);
        left_type = test_node ? test_node->type : 0xFFFF;
        
        /* Get right operand type */
        test_node = codegen_get_node(codegen, right_index);
        right_type = test_node ? test_node->type : 0xFFFF;
        
        /* Check if left operand is String */
        if (left_type == NODE_LITERAL_STRING) {
            left_is_string = 1;
        } else if (left_type == NODE_BINARY_OP) {
            /* If left is a binary operation with ADD operator, check if it's String concatenation */
            test_node = codegen_get_node(codegen, left_index);
            if (test_node && test_node->data.binary_op.op == BINOP_ADD) {
                /* Recursively check if this binary op produces a String result */
                uint16_t left_left_idx = test_node->data.binary_op.left;
                uint16_t left_right_idx = test_node->data.binary_op.right;
                ASTNode* left_left_node = codegen_get_node(codegen, left_left_idx);
                NodeType left_left_type = left_left_node ? left_left_node->type : NODE_PROGRAM;
                uint16_t left_left_name_idx = (left_left_node && left_left_node->type == NODE_IDENTIFIER) ?
                                               left_left_node->data.identifier.name : 0;
                ASTNode* left_right_node = codegen_get_node(codegen, left_right_idx);
                NodeType left_right_type = left_right_node ? left_right_node->type : NODE_PROGRAM;
                uint16_t left_right_name_idx = (left_right_node && left_right_node->type == NODE_IDENTIFIER) ?
                                                left_right_node->data.identifier.name : 0;
                
                /* If either operand of the nested binary op is a String literal, result is String */
                if (left_left_type == NODE_LITERAL_STRING || left_right_type == NODE_LITERAL_STRING) {
                    left_is_string = 1;
                }
                /* Check if either operand is a String variable */
                else if (left_left_type == NODE_IDENTIFIER || left_right_type == NODE_IDENTIFIER) {
                    uint16_t i;
                    if (left_left_type == NODE_IDENTIFIER) {
                        const char* var_name = codegen_get_string(codegen, left_left_name_idx);
                        if (var_name) {
                            for (i = 0; i < codegen->symtable->symbol_count; i++) {
                                Symbol* sym = &codegen->symtable->symbols[i];
                                const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                                if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                                    sym_name && strcmp(sym_name, var_name) == 0) {
                                    if (sym->type.kind == TYPE_CLASS) {
                                        const char* class_name = NULL;
                                        if (sym->type.class_name < codegen->pool_size) {
                                            class_name = codegen_get_string(codegen, sym->type.class_name);
                                        }
                                        if (class_name && (strcmp(class_name, "String") == 0 || strcmp(class_name, "Exception") == 0)) {
                                            left_is_string = 1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (!left_is_string && left_right_type == NODE_IDENTIFIER) {
                        const char* var_name = codegen_get_string(codegen, left_right_name_idx);
                        if (var_name) {
                            for (i = 0; i < codegen->symtable->symbol_count; i++) {
                                Symbol* sym = &codegen->symtable->symbols[i];
                                const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                                if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                                    sym_name && strcmp(sym_name, var_name) == 0) {
                                    if (sym->type.kind == TYPE_CLASS) {
                                        const char* class_name = NULL;
                                        if (sym->type.class_name < codegen->pool_size) {
                                            class_name = codegen_get_string(codegen, sym->type.class_name);
                                        }
                                        if (class_name && (strcmp(class_name, "String") == 0 || strcmp(class_name, "Exception") == 0)) {
                                            left_is_string = 1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if (left_type == NODE_IDENTIFIER) {
            /* Get left node again to access identifier data */
            test_node = codegen_get_node(codegen, left_index);
            if (test_node) {
                const char* left_name = codegen_get_string(codegen, test_node->data.identifier.name);
                if (left_name) {
                    Symbol* left_sym = NULL;
                    uint16_t best_scope = 0;
                    uint16_t i;
                    for (i = 0; i < codegen->symtable->symbol_count; i++) {
                        Symbol* sym = &codegen->symtable->symbols[i];
                        const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                            sym_name && strcmp(sym_name, left_name) == 0) {
                            if (!left_sym || sym->scope_level >= best_scope) {
                                left_sym = sym;
                                best_scope = sym->scope_level;
                            }
                        }
                    }
                    if (left_sym && left_sym->type.kind == TYPE_CLASS) {
                        /* Check if it's String or Exception class */
                        const char* class_name = NULL;
                        if (left_sym->type.class_name < codegen->pool_size) {
                            class_name = codegen_get_string(codegen, left_sym->type.class_name);
                        }
                        if (class_name && (strcmp(class_name, "String") == 0 || strcmp(class_name, "Exception") == 0)) {
                            left_is_string = 1;
                        }
                    } else if (left_sym && left_sym->type.kind == TYPE_INT) {
                        left_is_int = 1;
                    }
                }
            }
        } else if (left_type == NODE_LITERAL_INT) {
            left_is_int = 1;
        } else if (left_type == NODE_LITERAL_LONG) {
            left_is_long = 1;
        } else if (left_type == NODE_LITERAL_FLOAT) {
            left_is_float = 1;
        } else if (left_type == NODE_IDENTIFIER) {
            /* Check if left identifier is long or float */
            test_node = codegen_get_node(codegen, left_index);
            if (test_node) {
                const char* left_name = codegen_get_string(codegen, test_node->data.identifier.name);
                if (left_name) {
                    Symbol* left_sym = NULL;
                    uint16_t best_scope = 0;
                    uint16_t i;
                    for (i = 0; i < codegen->symtable->symbol_count; i++) {
                        Symbol* sym = &codegen->symtable->symbols[i];
                        const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                            sym_name && strcmp(sym_name, left_name) == 0) {
                            if (!left_sym || sym->scope_level >= best_scope) {
                                left_sym = sym;
                                best_scope = sym->scope_level;
                            }
                        }
                    }
                    if (left_sym) {
                        if (left_sym->type.kind == TYPE_LONG) {
                            left_is_long = 1;
                        } else if (left_sym->type.kind == TYPE_FLOAT) {
                            left_is_float = 1;
                        }
                    }
                }
            }
        }
        
        /* Check if right operand is String */
        if (right_type == NODE_LITERAL_STRING) {
            right_is_string = 1;
        } else if (right_type == NODE_BINARY_OP) {
            /* If right is a binary operation with ADD operator, check if it's String concatenation */
            test_node = codegen_get_node(codegen, right_index);
            if (test_node && test_node->data.binary_op.op == BINOP_ADD) {
                /* Recursively check if this binary op produces a String result */
                uint16_t right_left_idx = test_node->data.binary_op.left;
                uint16_t right_right_idx = test_node->data.binary_op.right;
                ASTNode* right_left_node = codegen_get_node(codegen, right_left_idx);
                NodeType right_left_type = right_left_node ? right_left_node->type : NODE_PROGRAM;
                uint16_t right_left_name_idx = (right_left_node && right_left_node->type == NODE_IDENTIFIER) ?
                                                right_left_node->data.identifier.name : 0;
                ASTNode* right_right_node = codegen_get_node(codegen, right_right_idx);
                NodeType right_right_type = right_right_node ? right_right_node->type : NODE_PROGRAM;
                uint16_t right_right_name_idx = (right_right_node && right_right_node->type == NODE_IDENTIFIER) ?
                                                 right_right_node->data.identifier.name : 0;
                
                /* If either operand of the nested binary op is a String literal, result is String */
                if (right_left_type == NODE_LITERAL_STRING || right_right_type == NODE_LITERAL_STRING) {
                    right_is_string = 1;
                }
                /* Check if either operand is a String variable */
                else if (right_left_type == NODE_IDENTIFIER || right_right_type == NODE_IDENTIFIER) {
                    uint16_t i;
                    if (right_left_type == NODE_IDENTIFIER) {
                        const char* var_name = codegen_get_string(codegen, right_left_name_idx);
                        if (var_name) {
                            for (i = 0; i < codegen->symtable->symbol_count; i++) {
                                Symbol* sym = &codegen->symtable->symbols[i];
                                const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                                if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                                    sym_name && strcmp(sym_name, var_name) == 0) {
                                    if (sym->type.kind == TYPE_CLASS) {
                                        const char* class_name = NULL;
                                        if (sym->type.class_name < codegen->pool_size) {
                                            class_name = codegen_get_string(codegen, sym->type.class_name);
                                        }
                                        if (class_name && (strcmp(class_name, "String") == 0 || strcmp(class_name, "Exception") == 0)) {
                                            right_is_string = 1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (!right_is_string && right_right_type == NODE_IDENTIFIER) {
                        const char* var_name = codegen_get_string(codegen, right_right_name_idx);
                        if (var_name) {
                            for (i = 0; i < codegen->symtable->symbol_count; i++) {
                                Symbol* sym = &codegen->symtable->symbols[i];
                                const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                                if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                                    sym_name && strcmp(sym_name, var_name) == 0) {
                                    if (sym->type.kind == TYPE_CLASS) {
                                        const char* class_name = NULL;
                                        if (sym->type.class_name < codegen->pool_size) {
                                            class_name = codegen_get_string(codegen, sym->type.class_name);
                                        }
                                        if (class_name && (strcmp(class_name, "String") == 0 || strcmp(class_name, "Exception") == 0)) {
                                            right_is_string = 1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if (right_type == NODE_IDENTIFIER) {
            /* Get right node again to access identifier data */
            test_node = codegen_get_node(codegen, right_index);
            if (test_node) {
                const char* right_name = codegen_get_string(codegen, test_node->data.identifier.name);
                if (right_name) {
                    Symbol* right_sym = NULL;
                    uint16_t best_scope = 0;
                    uint16_t i;
                    for (i = 0; i < codegen->symtable->symbol_count; i++) {
                        Symbol* sym = &codegen->symtable->symbols[i];
                        const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                            sym_name && strcmp(sym_name, right_name) == 0) {
                            if (!right_sym || sym->scope_level >= best_scope) {
                                right_sym = sym;
                                best_scope = sym->scope_level;
                            }
                        }
                    }
                    if (right_sym && right_sym->type.kind == TYPE_CLASS) {
                        /* Check if it's String or Exception class */
                        const char* class_name = NULL;
                        if (right_sym->type.class_name < codegen->pool_size) {
                            class_name = codegen_get_string(codegen, right_sym->type.class_name);
                        }
                        if (class_name && (strcmp(class_name, "String") == 0 || strcmp(class_name, "Exception") == 0)) {
                            right_is_string = 1;
                        }
                    } else if (right_sym && right_sym->type.kind == TYPE_INT) {
                        right_is_int = 1;
                    } else if (right_sym && right_sym->type.kind == TYPE_LONG) {
                        right_is_long = 1;
                    } else if (right_sym && right_sym->type.kind == TYPE_FLOAT) {
                        right_is_float = 1;
                    }
                }
            }
        } else if (right_type == NODE_LITERAL_INT) {
            right_is_int = 1;
        } else if (right_type == NODE_LITERAL_LONG) {
            right_is_long = 1;
        } else if (right_type == NODE_LITERAL_FLOAT) {
            right_is_float = 1;
        }
        
        /* String concatenation: String + any numeric type */
        if ((left_is_string && (right_is_string || right_is_int || right_is_long || right_is_float)) ||
            ((left_is_int || left_is_long || left_is_float) && right_is_string)) {
            is_string_concat = 1;
        }
    }
    
    /* Check if left operand is an Exception variable for string concatenation */
    {
        int left_handled = 0;
        if (is_string_concat && left_is_string) {
            ASTNode* test_node = codegen_get_node(codegen, left_index);
            if (test_node && test_node->type == NODE_IDENTIFIER) {
                const char* left_var_name = codegen_get_string(codegen, test_node->data.identifier.name);
                if (left_var_name) {
                    Symbol* left_sym = NULL;
                    uint16_t best_scope = 0;
                    uint16_t i;
                    for (i = 0; i < codegen->symtable->symbol_count; i++) {
                        Symbol* sym = &codegen->symtable->symbols[i];
                        const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                            sym_name && strcmp(sym_name, left_var_name) == 0) {
                            if (!left_sym || sym->scope_level >= best_scope) {
                                left_sym = sym;
                                best_scope = sym->scope_level;
                            }
                        }
                    }
                    if (left_sym && left_sym->type.kind == TYPE_CLASS) {
                        const char* class_name = NULL;
                        if (left_sym->type.class_name < codegen->pool_size) {
                            class_name = codegen_get_string(codegen, left_sym->type.class_name);
                        }
                        if (class_name && strcmp(class_name, "Exception") == 0) {
                            /* Load Exception variable and convert to string */
                            uint16_t local_idx = get_local_index(codegen, left_var_name);
                            if (local_idx != 0xFFFF) {
                                emit_opcode(codegen, OP_LOAD_LOCAL);
                                emit_u1(codegen, (uint8_t)local_idx);
                                emit_opcode(codegen, OP_EXCEPTION_TO_STRING);
                                update_stack(codegen, 1);
                                left_handled = 1;
                            }
                        }
                    }
                }
            }
        }
        
        /* Generate left operand if not already handled */
        if (!left_handled) {
            left_node = codegen_get_node(codegen, left_index);
            if (left_node) {
                generate_expression(codegen, left_node);
            }
        }
    }
    
    /* Convert left operand to String if needed */
    if (is_string_concat && left_is_int) {
        uint16_t tostring_idx;
        uint16_t tostring_desc_idx;
        
        tostring_idx = find_method_index(codegen, "Integer.toString", 1);
        if (tostring_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Integer.toString method reference");
            return -1;
        }
        
        tostring_desc_idx = find_or_add_utf8(codegen, "(I)Ljava/lang/String;");
        if (tostring_desc_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Integer.toString descriptor");
            return -1;
        }
        codegen->methods[tostring_idx].descriptor_index = tostring_desc_idx;
        
        emit_opcode(codegen, OP_INVOKE_STATIC);
        emit_u2(codegen, tostring_idx);
        emit_u1(codegen, 1);
        /* Stack: int -> String (no change in stack depth) */
    } else if (is_string_concat && left_is_long) {
        uint16_t tostring_idx;
        uint16_t tostring_desc_idx;
        
        tostring_idx = find_method_index(codegen, "Long.toString", 1);
        if (tostring_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Long.toString method reference");
            return -1;
        }
        
        tostring_desc_idx = find_or_add_utf8(codegen, "(J)Ljava/lang/String;");
        if (tostring_desc_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Long.toString descriptor");
            return -1;
        }
        codegen->methods[tostring_idx].descriptor_index = tostring_desc_idx;
        
        emit_opcode(codegen, OP_INVOKE_STATIC);
        emit_u2(codegen, tostring_idx);
        emit_u1(codegen, 1);
        /* Stack: long -> String (no change in stack depth) */
    } else if (is_string_concat && left_is_float) {
        uint16_t tostring_idx;
        uint16_t tostring_desc_idx;
        
        tostring_idx = find_method_index(codegen, "Float.toString", 1);
        if (tostring_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Float.toString method reference");
            return -1;
        }
        
        tostring_desc_idx = find_or_add_utf8(codegen, "(F)Ljava/lang/String;");
        if (tostring_desc_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Float.toString descriptor");
            return -1;
        }
        codegen->methods[tostring_idx].descriptor_index = tostring_desc_idx;
        
        emit_opcode(codegen, OP_INVOKE_STATIC);
        emit_u2(codegen, tostring_idx);
        emit_u1(codegen, 1);
        /* Stack: float -> String (no change in stack depth) */
    }
    
    /* Check if right operand is an Exception variable for string concatenation */
    {
        int right_handled = 0;
        if (is_string_concat && right_is_string) {
            ASTNode* test_node = codegen_get_node(codegen, right_index);
            if (test_node && test_node->type == NODE_IDENTIFIER) {
                const char* right_var_name = codegen_get_string(codegen, test_node->data.identifier.name);
                if (right_var_name) {
                    Symbol* right_sym = NULL;
                    uint16_t best_scope = 0;
                    uint16_t i;
                    for (i = 0; i < codegen->symtable->symbol_count; i++) {
                        Symbol* sym = &codegen->symtable->symbols[i];
                        const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                            sym_name && strcmp(sym_name, right_var_name) == 0) {
                            if (!right_sym || sym->scope_level >= best_scope) {
                                right_sym = sym;
                                best_scope = sym->scope_level;
                            }
                        }
                    }
                    if (right_sym && right_sym->type.kind == TYPE_CLASS) {
                        const char* class_name = NULL;
                        if (right_sym->type.class_name < codegen->pool_size) {
                            class_name = codegen_get_string(codegen, right_sym->type.class_name);
                        }
                        if (class_name && strcmp(class_name, "Exception") == 0) {
                            /* Load Exception variable and convert to string */
                            uint16_t local_idx = get_local_index(codegen, right_var_name);
                            if (local_idx != 0xFFFF) {
                                emit_opcode(codegen, OP_LOAD_LOCAL);
                                emit_u1(codegen, (uint8_t)local_idx);
                                emit_opcode(codegen, OP_EXCEPTION_TO_STRING);
                                update_stack(codegen, 1);
                                right_handled = 1;
                            }
                        }
                    }
                }
            }
        }
        
        /* Generate right operand if not already handled */
        if (!right_handled) {
            right_node = codegen_get_node(codegen, right_index);
            if (right_node) {
                generate_expression(codegen, right_node);
            }
        }
    }
    
    /* Convert right operand to String if needed */
    if (is_string_concat && right_is_int) {
        uint16_t tostring_idx;
        uint16_t tostring_desc_idx;
        
        tostring_idx = find_method_index(codegen, "Integer.toString", 1);
        if (tostring_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Integer.toString method reference");
            return -1;
        }
        
        tostring_desc_idx = find_or_add_utf8(codegen, "(I)Ljava/lang/String;");
        if (tostring_desc_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Integer.toString descriptor");
            return -1;
        }
        codegen->methods[tostring_idx].descriptor_index = tostring_desc_idx;
        
        emit_opcode(codegen, OP_INVOKE_STATIC);
        emit_u2(codegen, tostring_idx);
        emit_u1(codegen, 1);
        /* Stack: int -> String (no change in stack depth) */
    } else if (is_string_concat && right_is_long) {
        uint16_t tostring_idx;
        uint16_t tostring_desc_idx;
        
        tostring_idx = find_method_index(codegen, "Long.toString", 1);
        if (tostring_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Long.toString method reference");
            return -1;
        }
        
        tostring_desc_idx = find_or_add_utf8(codegen, "(J)Ljava/lang/String;");
        if (tostring_desc_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Long.toString descriptor");
            return -1;
        }
        codegen->methods[tostring_idx].descriptor_index = tostring_desc_idx;
        
        emit_opcode(codegen, OP_INVOKE_STATIC);
        emit_u2(codegen, tostring_idx);
        emit_u1(codegen, 1);
        /* Stack: long -> String (no change in stack depth) */
    } else if (is_string_concat && right_is_float) {
        uint16_t tostring_idx;
        uint16_t tostring_desc_idx;
        
        tostring_idx = find_method_index(codegen, "Float.toString", 1);
        if (tostring_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Float.toString method reference");
            return -1;
        }
        
        tostring_desc_idx = find_or_add_utf8(codegen, "(F)Ljava/lang/String;");
        if (tostring_desc_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add Float.toString descriptor");
            return -1;
        }
        codegen->methods[tostring_idx].descriptor_index = tostring_desc_idx;
        
        emit_opcode(codegen, OP_INVOKE_STATIC);
        emit_u2(codegen, tostring_idx);
        emit_u1(codegen, 1);
        /* Stack: float -> String (no change in stack depth) */
    }
    
    if (is_string_concat) {
        uint16_t method_idx;
        uint16_t desc_idx;
        
        method_idx = find_method_index(codegen, "concat", 1);
        if (method_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add concat method reference");
            return -1;
        }
        
        desc_idx = find_or_add_utf8(codegen, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
        if (desc_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add concat descriptor");
            return -1;
        }
        codegen->methods[method_idx].descriptor_index = desc_idx;
        
        
        emit_opcode(codegen, OP_INVOKE_STATIC);
        emit_u2(codegen, method_idx);
        emit_u1(codegen, 2);
        
        update_stack(codegen, -1); /* Two operands consumed, one result produced */
        return 0;
    }
    
    /* Generate operation - map AST operators to VM opcodes */
    /* Check if we need Float or Long operations by examining operand types */
    {
        int use_float_ops = 0;
        int use_long_ops = 0;
        ASTNode* check_left;
        ASTNode* check_right;
        
        /* Check if either operand is a float or long type */
        check_left = codegen_get_node(codegen, left_index);
        check_right = codegen_get_node(codegen, right_index);
        
        /* Float has highest priority */
        if (check_left && check_left->type == NODE_LITERAL_FLOAT) {
            use_float_ops = 1;
        }
        if (check_right && check_right->type == NODE_LITERAL_FLOAT) {
            use_float_ops = 1;
        }
        
        /* Check if operands are unary operations on float literals (e.g., -0.01f) */
        if (check_left && check_left->type == NODE_UNARY_OP) {
            ASTNode* unary_operand = codegen_get_node(codegen, check_left->data.unary_op.operand);
            if (unary_operand && unary_operand->type == NODE_LITERAL_FLOAT) {
                use_float_ops = 1;
            }
        }
        if (check_right && check_right->type == NODE_UNARY_OP) {
            ASTNode* unary_operand = codegen_get_node(codegen, check_right->data.unary_op.operand);
            if (unary_operand && unary_operand->type == NODE_LITERAL_FLOAT) {
                use_float_ops = 1;
            }
        }
        
        /* Check if operands are Math method calls (which return float) */
        if (check_left && check_left->type == NODE_CALL) {
            const char* method_name = codegen_get_string(codegen, check_left->data.call.method_name);
            if (method_name && (strcmp(method_name, "abs") == 0 ||
                                strcmp(method_name, "min") == 0 ||
                                strcmp(method_name, "max") == 0 ||
                                strcmp(method_name, "sqrt") == 0 ||
                                strcmp(method_name, "sin") == 0 ||
                                strcmp(method_name, "cos") == 0 ||
                                strcmp(method_name, "tan") == 0 ||
                                strcmp(method_name, "pow") == 0 ||
                                strcmp(method_name, "exp") == 0 ||
                                strcmp(method_name, "log") == 0 ||
                                strcmp(method_name, "floor") == 0 ||
                                strcmp(method_name, "ceil") == 0)) {
                use_float_ops = 1;
            }
        }
        if (check_right && check_right->type == NODE_CALL) {
            const char* method_name = codegen_get_string(codegen, check_right->data.call.method_name);
            if (method_name && (strcmp(method_name, "abs") == 0 ||
                                strcmp(method_name, "min") == 0 ||
                                strcmp(method_name, "max") == 0 ||
                                strcmp(method_name, "sqrt") == 0 ||
                                strcmp(method_name, "sin") == 0 ||
                                strcmp(method_name, "cos") == 0 ||
                                strcmp(method_name, "tan") == 0 ||
                                strcmp(method_name, "pow") == 0 ||
                                strcmp(method_name, "exp") == 0 ||
                                strcmp(method_name, "log") == 0 ||
                                strcmp(method_name, "floor") == 0 ||
                                strcmp(method_name, "ceil") == 0)) {
                use_float_ops = 1;
            }
        }
        
        /* Then check for long */
        if (!use_float_ops) {
            if (check_left && check_left->type == NODE_LITERAL_LONG) {
                use_long_ops = 1;
            }
            if (check_right && check_right->type == NODE_LITERAL_LONG) {
                use_long_ops = 1;
            }
            
            /* Check if operands are unary operations on long literals (e.g., -100L) */
            if (check_left && check_left->type == NODE_UNARY_OP) {
                ASTNode* unary_operand = codegen_get_node(codegen, check_left->data.unary_op.operand);
                if (unary_operand && unary_operand->type == NODE_LITERAL_LONG) {
                    use_long_ops = 1;
                }
            }
            if (check_right && check_right->type == NODE_UNARY_OP) {
                ASTNode* unary_operand = codegen_get_node(codegen, check_right->data.unary_op.operand);
                if (unary_operand && unary_operand->type == NODE_LITERAL_LONG) {
                    use_long_ops = 1;
                }
            }
        }
        
        /* Check if operands are identifiers with long type */
        if (check_left && check_left->type == NODE_IDENTIFIER) {
            const char* var_name = codegen_get_string(codegen, check_left->data.identifier.name);
            if (var_name) {
                uint16_t i;
                /* Search in reverse order to find innermost scope first */
                for (i = codegen->symtable->symbol_count; i > 0; i--) {
                    Symbol* sym = &codegen->symtable->symbols[i - 1];
                    const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                    if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                        sym_name && strcmp(sym_name, var_name) == 0) {
                        if (sym->type.kind == TYPE_FLOAT) {
                            use_float_ops = 1;
                        } else if (sym->type.kind == TYPE_LONG) {
                            use_long_ops = 1;
                        }
                        break;
                    }
                }
            }
        }
        /* Check if left operand is array access with long or float element type */
        if (check_left && check_left->type == NODE_ARRAY_ACCESS) {
            uint16_t elem_type = get_array_element_type(codegen, check_left);
            if (elem_type == TYPE_FLOAT) {
                use_float_ops = 1;
            } else if (elem_type == TYPE_LONG) {
                use_long_ops = 1;
            }
        }
        if (check_right && check_right->type == NODE_IDENTIFIER) {
            const char* var_name = codegen_get_string(codegen, check_right->data.identifier.name);
            if (var_name) {
                uint16_t i;
                /* Search in reverse order to find innermost scope first */
                for (i = codegen->symtable->symbol_count; i > 0; i--) {
                    Symbol* sym = &codegen->symtable->symbols[i - 1];
                    const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                    if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                        sym_name && strcmp(sym_name, var_name) == 0) {
                        if (sym->type.kind == TYPE_FLOAT) {
                            use_float_ops = 1;
                        } else if (sym->type.kind == TYPE_LONG) {
                            use_long_ops = 1;
                        }
                        break;
                    }
                }
            }
        }
        /* Check if right operand is array access with long or float element type */
        if (check_right && check_right->type == NODE_ARRAY_ACCESS) {
            uint16_t elem_type = get_array_element_type(codegen, check_right);
            if (elem_type == TYPE_FLOAT) {
                use_float_ops = 1;
            } else if (elem_type == TYPE_LONG) {
                use_long_ops = 1;
            }
        }
        
        switch (op) {
            case (uint16_t)BINOP_ADD:
                if (use_float_ops) {
                    emit_opcode(codegen, OP_FADD);
                } else {
                    emit_opcode(codegen, use_long_ops ? OP_LADD : OP_ADD);
                }
                break;
            case (uint16_t)BINOP_SUB:
                if (use_float_ops) {
                    emit_opcode(codegen, OP_FSUB);
                } else {
                    emit_opcode(codegen, use_long_ops ? OP_LSUB : OP_SUB);
                }
                break;
            case (uint16_t)BINOP_MUL:
                if (use_float_ops) {
                    emit_opcode(codegen, OP_FMUL);
                } else {
                    emit_opcode(codegen, use_long_ops ? OP_LMUL : OP_MUL);
                }
                break;
            case (uint16_t)BINOP_DIV:
                if (use_float_ops) {
                    emit_opcode(codegen, OP_FDIV);
                } else {
                    emit_opcode(codegen, use_long_ops ? OP_LDIV : OP_DIV);
                }
                break;
            case (uint16_t)BINOP_MOD:
                if (use_float_ops) {
                    emit_opcode(codegen, OP_FREM);
                } else {
                    emit_opcode(codegen, use_long_ops ? OP_LMOD : OP_MOD);
                }
                break;
            case (uint16_t)BINOP_EQ:
                if (use_float_ops) {
                    /* For float comparison: FCMPG followed by CMP_EQ with 0 */
                    emit_opcode(codegen, OP_FCMPG);
                    update_stack(codegen, -3);  /* Consumes 2 floats (4 slots), pushes 1 int (1 slot) */
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_EQ);
                    update_stack(codegen, -1);
                } else if (use_long_ops) {
                    /* For long comparison: LCMP followed by CMP_EQ with 0 */
                    emit_opcode(codegen, OP_LCMP);
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_EQ);
                    update_stack(codegen, -1);
                } else {
                    emit_opcode(codegen, OP_CMP_EQ);
                }
                break;
            case (uint16_t)BINOP_NE:
                if (use_float_ops) {
                    emit_opcode(codegen, OP_FCMPG);
                    update_stack(codegen, -3);  /* Consumes 2 floats (4 slots), pushes 1 int (1 slot) */
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_NE);
                    update_stack(codegen, -1);
                } else if (use_long_ops) {
                    emit_opcode(codegen, OP_LCMP);
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_NE);
                    update_stack(codegen, -1);
                } else {
                    emit_opcode(codegen, OP_CMP_NE);
                }
                break;
            case (uint16_t)BINOP_LT:
                if (use_float_ops) {
                    emit_opcode(codegen, OP_FCMPG);
                    update_stack(codegen, -3);  /* Consumes 2 floats (4 slots), pushes 1 int (1 slot) */
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_LT);
                    update_stack(codegen, -1);
                } else if (use_long_ops) {
                    emit_opcode(codegen, OP_LCMP);
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_LT);
                    update_stack(codegen, -1);
                } else {
                    emit_opcode(codegen, OP_CMP_LT);
                }
                break;
            case (uint16_t)BINOP_LE:
                if (use_float_ops) {
                    emit_opcode(codegen, OP_FCMPG);
                    update_stack(codegen, -3);  /* Consumes 2 floats (4 slots), pushes 1 int (1 slot) */
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_LE);
                    update_stack(codegen, -1);
                } else if (use_long_ops) {
                    emit_opcode(codegen, OP_LCMP);
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_LE);
                    update_stack(codegen, -1);
                } else {
                    emit_opcode(codegen, OP_CMP_LE);
                }
                break;
            case (uint16_t)BINOP_GT:
                if (use_float_ops) {
                    emit_opcode(codegen, OP_FCMPG);
                    update_stack(codegen, -3);  /* Consumes 2 floats (4 slots), pushes 1 int (1 slot) */
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_GT);
                    update_stack(codegen, -1);
                } else if (use_long_ops) {
                    emit_opcode(codegen, OP_LCMP);
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_GT);
                    update_stack(codegen, -1);
                } else {
                    emit_opcode(codegen, OP_CMP_GT);
                }
                break;
            case (uint16_t)BINOP_GE:
                if (use_float_ops) {
                    emit_opcode(codegen, OP_FCMPG);
                    update_stack(codegen, -3);  /* Consumes 2 floats (4 slots), pushes 1 int (1 slot) */
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_GE);
                    update_stack(codegen, -1);
                } else if (use_long_ops) {
                    emit_opcode(codegen, OP_LCMP);
                    emit_opcode(codegen, OP_PUSH_INT);
                    emit_u2(codegen, 0);
                    update_stack(codegen, 1);
                    emit_opcode(codegen, OP_CMP_GE);
                    update_stack(codegen, -1);
                } else {
                    emit_opcode(codegen, OP_CMP_GE);
                }
                break;
            case (uint16_t)BINOP_AND:
            case (uint16_t)BINOP_OR:
                /* Logical operations handled above with short-circuit evaluation */
                codegen_error(codegen, "Logical AND/OR should have been handled earlier");
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
    uint16_t op;
    uint16_t operand_index;
    
    if (!codegen || !unop_node) {
        return -1;
    }
    
    /* CRITICAL: Save operator and operand index BEFORE any recursive calls
     * because generate_expression() may overwrite the unop_node memory */
    op = unop_node->data.unary_op.op;
    operand_index = unop_node->data.unary_op.operand;
    
    /* Generate operand */
    operand_node = codegen_get_node(codegen, operand_index);
    if (operand_node) {
        generate_expression(codegen, operand_node);
    }
    
    /* Generate operation */
    switch (op) {
        case UNOP_NEG: {
            /* Check if operand is long or float type */
            int is_long = 0;
            int is_float = 0;
            
            if (operand_node && operand_node->type == NODE_LITERAL_LONG) {
                is_long = 1;
            } else if (operand_node && operand_node->type == NODE_LITERAL_FLOAT) {
                is_float = 1;
            } else if (operand_node && operand_node->type == NODE_IDENTIFIER) {
                const char* var_name = codegen_get_string(codegen, operand_node->data.identifier.name);
                if (var_name) {
                    uint16_t i;
                    uint16_t best_scope = 0;
                    uint16_t symbol_index = 0xFFFF;
                    
                    /* Find symbol with highest scope_level (innermost scope) */
                    for (i = 0; i < codegen->symtable->symbol_count; i++) {
                        Symbol* sym = &codegen->symtable->symbols[i];
                        const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                            sym_name && strcmp(sym_name, var_name) == 0) {
                            if (symbol_index == 0xFFFF || sym->scope_level >= best_scope) {
                                symbol_index = i;
                                best_scope = sym->scope_level;
                            }
                        }
                    }
                    
                    /* Check type of the found symbol */
                    if (symbol_index != 0xFFFF) {
                        Symbol* sym = &codegen->symtable->symbols[symbol_index];
                        if (sym->type.kind == TYPE_LONG) {
                            is_long = 1;
                        } else if (sym->type.kind == TYPE_FLOAT) {
                            is_float = 1;
                        }
                    }
                }
            }
            
            /* Emit appropriate negation opcode */
            if (is_float) {
                emit_opcode(codegen, OP_FNEG);
            } else if (is_long) {
                emit_opcode(codegen, OP_LNEG);
            } else {
                emit_opcode(codegen, OP_NEG);
            }
            break;
        }
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
    ASTNode value_expr_copy;
    const char* var_name;
    uint16_t local_idx;
    uint16_t target_index;
    uint16_t value_index;
    uint16_t target_type;
    uint16_t target_name_off;
    uint16_t array_idx;
    uint16_t index_idx;
    uint16_t assign_op;
    uint16_t i;
    Symbol* field_sym;
    uint16_t j;
    
    if (!codegen || !assign_node) {
        return -1;
    }
    
    assign_op = assign_node->data.assign.op;
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
    } else if (target_type == NODE_FIELD_ACCESS) {
        /* Save field access info */
        uint16_t field_name_off = target_node->data.field_access.field_name;
        uint16_t object_idx = target_node->data.field_access.object;
        
        /* Handle field assignment: object.field = value */
        if (assign_op == 0) {
            /* Simple assignment */
            ASTNode* object_node;
            const char* field_name;
            uint16_t field_idx;
            
            /* Generate object reference */
            object_node = codegen_get_node(codegen, object_idx);
            if (!object_node) {
                codegen_error(codegen, "Invalid field assignment target");
                return -1;
            }
            generate_expression(codegen, object_node);
            
            /* Duplicate object reference for OP_PUT_FIELD */
            emit_opcode(codegen, OP_DUP);
            update_stack(codegen, 1);
            
            /* Generate value */
            value_node = codegen_get_node(codegen, value_index);
            if (!value_node) {
                codegen_error(codegen, "Invalid field assignment value");
                return -1;
            }
            generate_expression(codegen, value_node);
            
            /* Get field name */
            field_name = codegen_get_string(codegen, field_name_off);
            if (!field_name) {
                codegen_error(codegen, "Invalid field name");
                return -1;
            }
            
            /* Add field name to constant pool */
            field_idx = find_or_add_utf8(codegen, field_name);
            if (field_idx == 0xFFFF) {
                codegen_error(codegen, "Failed to add field constant");
                return -1;
            }
            
            /* Stack: [object_ref, object_ref, value]
             * OP_PUT_FIELD pops value and object_ref
             * Leaves one object_ref on stack as assignment result
             */
            emit_opcode(codegen, OP_PUT_FIELD);
            emit_u2(codegen, field_idx);
            update_stack(codegen, -2); /* object and value consumed */
            
            return 0;
        } else {
            codegen_error(codegen, "Compound assignment to fields not yet supported");
            return -1;
        }
    }
    
    value_node = codegen_get_node(codegen, value_index);
    if (!value_node) {
        codegen_error(codegen, "Invalid assignment");
        return -1;
    }
    memcpy(&value_expr_copy, value_node, sizeof(ASTNode));
    
    if (target_type == NODE_ARRAY_ACCESS) {
        if (assign_op == 0) {
            ASTNode array_expr_copy;
            ASTNode index_expr_copy;
            uint16_t elem_type;
            
            {
                ASTNode* array_src = codegen_get_node(codegen, array_idx);
                if (!array_src) {
                    codegen_error(codegen, "Invalid array assignment target");
                    return -1;
                }
                memcpy(&array_expr_copy, array_src, sizeof(ASTNode));
            }
            
            {
                ASTNode* index_src = codegen_get_node(codegen, index_idx);
                if (!index_src) {
                    codegen_error(codegen, "Invalid array assignment target");
                    return -1;
                }
                memcpy(&index_expr_copy, index_src, sizeof(ASTNode));
            }
            
            if (generate_expression(codegen, &array_expr_copy) != 0) {
                return -1;
            }
            if (generate_expression(codegen, &index_expr_copy) != 0) {
                return -1;
            }
            if (generate_expression(codegen, &value_expr_copy) != 0) {
                return -1;
            }
            
            /* Determine element type and emit appropriate store opcode */
            elem_type = get_array_element_type(codegen, &array_expr_copy);
            if (elem_type == TYPE_LONG) {
                emit_opcode(codegen, OP_LARRAY_STORE);
                update_stack(codegen, -2);  /* Pops 4 (array, index, high, low), pushes 2 (high, low) */
            } else if (elem_type == TYPE_FLOAT) {
                emit_opcode(codegen, OP_FARRAY_STORE);
                update_stack(codegen, -2);  /* Pops 4 (array, index, high, low), pushes 2 (high, low) */
            } else {
                emit_opcode(codegen, OP_ARRAY_STORE);
                update_stack(codegen, -3);  /* Pops 3 (array, index, value), pushes 1 (value) */
            }
            return 0;
        }
        
        if (assign_op == 1 || assign_op == 2) {
            codegen_error(codegen, "Compound assignment for array elements is not yet codegen-safe");
            return -1;
        }
        
        codegen_error(codegen, "Unsupported assignment operator");
        return -1;
    }
    
    if (target_type != NODE_IDENTIFIER) {
        codegen_error(codegen, "Invalid assignment target");
        return -1;
    }
    
    var_name = codegen_get_string(codegen, target_name_off);
    if (!var_name) {
        return -1;
    }
    
    local_idx = get_local_index(codegen, var_name);
    
    /* If not a local variable, check if it's a field */
    if (local_idx == 0xFFFF) {
        field_sym = NULL;
        
        for (j = 0; j < codegen->symtable->symbol_count; j++) {
            Symbol* sym = &codegen->symtable->symbols[j];
            if (sym->kind == SYM_FIELD) {
                const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                if (sym_name && strcmp(sym_name, var_name) == 0) {
                    field_sym = sym;
                    break;
                }
            }
        }
        
        if (field_sym) {
            /* It's a field - generate implicit 'this.field = value' */
            if (assign_op == 0) {
                uint16_t field_idx;
                
                /* Load 'this' (always at local index 0 in instance methods) */
                emit_opcode(codegen, OP_LOAD_0);
                update_stack(codegen, 1);
                
                /* Duplicate 'this' for OP_PUT_FIELD */
                emit_opcode(codegen, OP_DUP);
                update_stack(codegen, 1);
                
                /* Generate value */
                if (generate_expression(codegen, &value_expr_copy) != 0) {
                    return -1;
                }
                
                /* Add field name to constant pool */
                field_idx = find_or_add_utf8(codegen, var_name);
                if (field_idx == 0xFFFF) {
                    codegen_error(codegen, "Failed to add field constant");
                    return -1;
                }
                
                /* Stack: [this, this, value]
                 * OP_PUT_FIELD pops value and this
                 * Leaves one this on stack as assignment result
                 */
                emit_opcode(codegen, OP_PUT_FIELD);
                emit_u2(codegen, field_idx);
                update_stack(codegen, -2);
                
                return 0;
            } else {
                codegen_error(codegen, "Compound assignment to fields not yet supported");
                return -1;
            }
        }
        
        /* Neither local nor field */
        codegen_error(codegen, "Undefined variable");
        return -1;
    }
    
    if (assign_op == 0) {
        /* Check target type */
        /* Search in reverse order to find innermost scope first */
        int is_float = 0;
        int is_long = 0;
        for (i = codegen->symtable->symbol_count; i > 0; i--) {
            Symbol* sym = &codegen->symtable->symbols[i - 1];
            const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
            if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                sym_name && strcmp(sym_name, var_name) == 0) {
                if (sym->type.kind == TYPE_FLOAT) {
                    is_float = 1;
                } else if (sym->type.kind == TYPE_LONG) {
                    is_long = 1;
                }
                break;
            }
        }
        
        if (generate_expression(codegen, &value_expr_copy) != 0) {
            return -1;
        }
        
        if (is_float) {
            /* Float assignment: duplicate 2-word value by storing and reloading */
            emit_opcode(codegen, OP_STORE_FLOAT);
            emit_u1(codegen, (uint8_t)local_idx);
            update_stack(codegen, -2);
            /* Reload for expression result */
            emit_opcode(codegen, OP_LOAD_FLOAT);
            emit_u1(codegen, (uint8_t)local_idx);
            update_stack(codegen, 2);
        } else if (is_long) {
            /* Long assignment: duplicate 2-word value by storing and reloading */
            emit_opcode(codegen, OP_STORE_LONG);
            emit_u1(codegen, (uint8_t)local_idx);
            update_stack(codegen, -2);
            /* Reload for expression result */
            emit_opcode(codegen, OP_LOAD_LONG);
            emit_u1(codegen, (uint8_t)local_idx);
            update_stack(codegen, 2);
        } else {
            /* Int/boolean/reference assignment */
            emit_opcode(codegen, OP_DUP);
            update_stack(codegen, 1);
            if (local_idx <= 2) {
                emit_opcode(codegen, OP_STORE_0 + local_idx);
            } else {
                emit_opcode(codegen, OP_STORE_LOCAL);
                emit_u1(codegen, (uint8_t)local_idx);
            }
            update_stack(codegen, -1);
        }
        return 0;
    }
    
    if (assign_op == 1 || assign_op == 2) {
        Symbol* value_sym = NULL;
        const char* class_name = NULL;
        int target_is_string = 0;
        uint16_t method_idx;
        uint16_t desc_idx;
        
        if (local_idx == 0xFFFF) {
            codegen_error(codegen, "Undefined assignment target");
            return -1;
        }
        
        for (i = 0; i < codegen->symtable->symbol_count; i++) {
            Symbol* sym = &codegen->symtable->symbols[i];
            const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
            if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                sym_name && strcmp(sym_name, var_name) == 0) {
                value_sym = sym;
            }
        }
        
        if (value_sym && value_sym->type.kind == TYPE_CLASS) {
            class_name = symtable_get_string(codegen->symtable, value_sym->type.class_name);
            if (class_name && strcmp(class_name, "String") == 0) {
                target_is_string = 1;
            }
        }
        
        if (target_is_string && assign_op == 1) {
            if (local_idx <= 2) {
                emit_opcode(codegen, OP_LOAD_0 + local_idx);
            } else {
                emit_opcode(codegen, OP_LOAD_LOCAL);
                emit_u1(codegen, (uint8_t)local_idx);
            }
            update_stack(codegen, 1);
            
            if (generate_expression(codegen, &value_expr_copy) != 0) {
                return -1;
            }
            
            method_idx = find_method_index(codegen, "concat", 1);
            if (method_idx == 0xFFFF) {
                codegen_error(codegen, "Failed to add concat method reference");
                return -1;
            }
            
            desc_idx = find_or_add_utf8(codegen, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
            if (desc_idx == 0xFFFF) {
                codegen_error(codegen, "Failed to add concat descriptor");
                return -1;
            }
            codegen->methods[method_idx].descriptor_index = desc_idx;
            
            emit_opcode(codegen, OP_INVOKE_STATIC);
            emit_u2(codegen, method_idx);
            emit_u1(codegen, 2);
            update_stack(codegen, -1);
            
            emit_opcode(codegen, OP_DUP);
            update_stack(codegen, 1);
            
            if (local_idx <= 2) {
                emit_opcode(codegen, OP_STORE_0 + local_idx);
            } else {
                emit_opcode(codegen, OP_STORE_LOCAL);
                emit_u1(codegen, (uint8_t)local_idx);
            }
            update_stack(codegen, -1);
            return 0;
        }
        
        if (local_idx <= 2) {
            emit_opcode(codegen, OP_LOAD_0 + local_idx);
        } else {
            emit_opcode(codegen, OP_LOAD_LOCAL);
            emit_u1(codegen, (uint8_t)local_idx);
        }
        update_stack(codegen, 1);
        
        if (generate_expression(codegen, &value_expr_copy) != 0) {
            return -1;
        }
        if (assign_op == 1) {
            emit_opcode(codegen, OP_ADD);
        } else {
            emit_opcode(codegen, OP_SUB);
        }
        update_stack(codegen, -1);
        
        emit_opcode(codegen, OP_DUP);
        update_stack(codegen, 1);
        
        if (local_idx <= 2) {
            emit_opcode(codegen, OP_STORE_0 + local_idx);
        } else {
            emit_opcode(codegen, OP_STORE_LOCAL);
            emit_u1(codegen, (uint8_t)local_idx);
        }
        update_stack(codegen, -1);
        return 0;
    }
    
    codegen_error(codegen, "Unsupported assignment operator");
    return -1;
}

/* Generate code for method call */
int generate_method_call(CodeGenerator* codegen, ASTNode* call_node) {
    const char* method_name;
    uint16_t arg_idx;
    ASTNode* arg_node;
    uint16_t arg_count;
    uint16_t total_arg_count;
    uint16_t method_idx;
    uint16_t object_idx;
    int is_native;
    int is_math_method;
    NodeType arg_node_type;
    uint16_t saved_first_arg;
    Symbol* method_sym;
    int returns_value;
    const char* first_arg_name;
    uint16_t i;
    int first_arg_is_string;
    int is_string_length;
    int is_string_caseconv;
    int is_string_compare;
    int is_string_equals;
    int is_string_compareto;
    int is_string_indexof;
    int is_string_lastindexof;
    int is_string_substr;
    uint16_t invoke_arg_count;
    
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
    
    is_native = 0;
    is_string_length = 0;
    is_string_caseconv = 0;
    is_string_compare = 0;
    is_string_equals = 0;
    is_string_compareto = 0;
    is_string_indexof = 0;
    is_string_lastindexof = 0;
    is_string_substr = 0;
    if (strcmp(method_name, "println") == 0 || strcmp(method_name, "print") == 0 ||
        strcmp(method_name, "printInt") == 0 || strcmp(method_name, "printLong") == 0) {
        is_native = 1;
    } else if (strcmp(method_name, "concat") == 0 && object_idx == 0) {
        is_native = 1;
    } else if (strcmp(method_name, "getType") == 0 || strcmp(method_name, "getMessage") == 0) {
        /* Exception.getType() and Exception.getMessage() are native */
        is_native = 1;
    } else if (object_idx != 0) {
        /* Check if this is Math.method(), Integer.method(), or Http.method() call */
        ASTNode* obj_node = codegen_get_node(codegen, object_idx);
        if (obj_node && obj_node->type == NODE_IDENTIFIER) {
            const char* obj_name = codegen_get_string(codegen, obj_node->data.identifier.name);
            if (obj_name && (strcmp(obj_name, "Math") == 0 || strcmp(obj_name, "Integer") == 0 || strcmp(obj_name, "Http") == 0)) {
                is_native = 1;
            }
        }
    }
    
    /* Check if this is a Math method call that needs argument type conversion */
    is_math_method = 0;
    if (object_idx != 0) {
        ASTNode* obj_node = codegen_get_node(codegen, object_idx);
        if (obj_node && obj_node->type == NODE_IDENTIFIER) {
            const char* obj_name = codegen_get_string(codegen, obj_node->data.identifier.name);
            if (obj_name && strcmp(obj_name, "Math") == 0) {
                is_math_method = 1;
            }
        }
    }
    
    if (!is_native && strcmp(method_name, "concat") == 0 && object_idx == 0) {
        is_native = 1;
    } else if (object_idx != 0 && strcmp(method_name, "length") == 0) {
        is_native = 1;
        is_string_length = 1;
    } else if (object_idx != 0 &&
               (strcmp(method_name, "toUpperCase") == 0 ||
                strcmp(method_name, "toLowerCase") == 0)) {
        is_native = 1;
        is_string_caseconv = 1;
    } else if (object_idx != 0 &&
               (strcmp(method_name, "startsWith") == 0 ||
                strcmp(method_name, "endsWith") == 0)) {
        is_native = 1;
        is_string_compare = 1;
    } else if (object_idx != 0 && strcmp(method_name, "equals") == 0) {
        is_native = 1;
        is_string_equals = 1;
    } else if (object_idx != 0 && strcmp(method_name, "compareTo") == 0) {
        is_native = 1;
        is_string_compareto = 1;
    } else if (object_idx != 0 && strcmp(method_name, "indexOf") == 0) {
        is_native = 1;
        is_string_indexof = 1;
    } else if (object_idx != 0 && strcmp(method_name, "lastIndexOf") == 0) {
        is_native = 1;
        is_string_lastindexof = 1;
    } else if (object_idx != 0 && strcmp(method_name, "substr") == 0) {
        is_native = 1;
        is_string_substr = 1;
    } else if (strcmp(method_name, "open") == 0 ||
               strcmp(method_name, "readLine") == 0 ||
               strcmp(method_name, "writeLine") == 0 ||
               strcmp(method_name, "close") == 0) {
        /* Check if this is File.method() call */
        if (object_idx != 0) {
            ASTNode* obj_node = codegen_get_node(codegen, object_idx);
            if (obj_node && obj_node->type == NODE_IDENTIFIER) {
                const char* obj_name = codegen_get_string(codegen, obj_node->data.identifier.name);
                if (obj_name && strcmp(obj_name, "File") == 0) {
                    is_native = 1;
                }
            }
        }
    } else if (strcmp(method_name, "init") == 0 ||
               strcmp(method_name, "create") == 0 ||
               strcmp(method_name, "send") == 0 ||
               strcmp(method_name, "recv") == 0 ||
               strcmp(method_name, "close") == 0 ||
               strcmp(method_name, "isConnected") == 0) {
        /* Check if this is Socket.method() call */
        if (object_idx != 0) {
            ASTNode* obj_node = codegen_get_node(codegen, object_idx);
            if (obj_node && obj_node->type == NODE_IDENTIFIER) {
                const char* obj_name = codegen_get_string(codegen, obj_node->data.identifier.name);
                if (obj_name && strcmp(obj_name, "Socket") == 0) {
                    is_native = 1;
                }
            }
        }
    }
    
    arg_node_type = NODE_PROGRAM;
    first_arg_name = NULL;
    first_arg_is_string = 0;
    if (!is_string_length && !is_string_caseconv && saved_first_arg != 0) {
        arg_node = codegen_get_node(codegen, saved_first_arg);
        if (arg_node) {
            arg_node_type = arg_node->type;
            
            /* If arg is an expression statement, get the actual expression */
            if (arg_node_type == NODE_EXPR_STMT) {
                uint16_t expr_idx = arg_node->data.expr_stmt.expr;
                arg_node = codegen_get_node(codegen, expr_idx);
                if (arg_node) {
                    arg_node_type = arg_node->type;
                }
            }
            
            if (arg_node_type == NODE_LITERAL_STRING) {
                first_arg_is_string = 1;
            }
            /* Check if method call returns String (e.g., concat result) */
            else if (arg_node_type == NODE_CALL) {  /* NODE_CALL */
                const char* call_method_name = codegen_get_string(codegen, arg_node->data.call.method_name);
                if (call_method_name) {
                    /* Check if this is a String-returning method */
                    if (strcmp(call_method_name, "concat") == 0 ||
                        strcmp(call_method_name, "toUpperCase") == 0 ||
                        strcmp(call_method_name, "toLowerCase") == 0 ||
                        strcmp(call_method_name, "substr") == 0 ||
                        strcmp(call_method_name, "readLine") == 0) {
                        first_arg_is_string = 1;
                    }
                }
            }
            /* Check if binary operation result is String (e.g., String + int) */
            else if (arg_node_type == NODE_BINARY_OP) {
                /* For binary operations, check if it's a String concatenation:
                 * - If either operand is a String literal, result is String
                 * - If operator is ADD (+), check operand types
                 */
                /* Save values before calling codegen_get_node (which reuses buffer) */
                uint16_t left_idx = arg_node->data.binary_op.left;
                uint16_t right_idx = arg_node->data.binary_op.right;
                BinaryOp op = arg_node->data.binary_op.op;
                
                ASTNode* left_node = codegen_get_node(codegen, left_idx);
                NodeType left_type = left_node ? left_node->type : NODE_PROGRAM;
                uint16_t left_name_idx = (left_node && left_node->type == NODE_IDENTIFIER) ?
                                         left_node->data.identifier.name : 0;
                
                ASTNode* right_node = codegen_get_node(codegen, right_idx);
                NodeType right_type = right_node ? right_node->type : NODE_PROGRAM;
                uint16_t right_name_idx = (right_node && right_node->type == NODE_IDENTIFIER) ?
                                          right_node->data.identifier.name : 0;

                if (op == BINOP_ADD) {
                    /* Check if either operand is a String literal */
                    if (left_type == NODE_LITERAL_STRING || right_type == NODE_LITERAL_STRING) {
                        first_arg_is_string = 1;
                    }
                    /* Check if either operand is a String variable */
                    if (left_type == NODE_IDENTIFIER) {
                        const char* left_name = codegen_get_string(codegen, left_name_idx);
                        if (left_name) {
                            for (i = 0; i < codegen->symtable->symbol_count; i++) {
                                Symbol* sym = &codegen->symtable->symbols[i];
                                const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                                if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                                    sym_name && strcmp(sym_name, left_name) == 0) {
                                    if (sym->type.kind == TYPE_CLASS) {
                                        first_arg_is_string = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if (right_type == NODE_IDENTIFIER && !first_arg_is_string) {
                        const char* right_name = codegen_get_string(codegen, right_name_idx);
                        if (right_name) {
                            for (i = 0; i < codegen->symtable->symbol_count; i++) {
                                Symbol* sym = &codegen->symtable->symbols[i];
                                const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                                if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                                    sym_name && strcmp(sym_name, right_name) == 0) {
                                    if (sym->type.kind == TYPE_CLASS) {
                                        first_arg_is_string = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (arg_node_type == NODE_IDENTIFIER) {
                Symbol* best_sym = NULL;
                uint16_t best_scope = 0;
                
                first_arg_name = codegen_get_string(codegen, arg_node->data.identifier.name);
                if (first_arg_name) {
                    for (i = 0; i < codegen->symtable->symbol_count; i++) {
                        Symbol* sym = &codegen->symtable->symbols[i];
                        const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                            sym_name &&
                            strcmp(sym_name, first_arg_name) == 0) {
                            if (!best_sym || sym->scope_level >= best_scope) {
                                best_sym = sym;
                                best_scope = sym->scope_level;
                            }
                        }
                    }
                    if (best_sym) {
                        if (best_sym->type.kind == TYPE_CLASS) {
                            first_arg_is_string = 1;
                        } else if (best_sym->type.kind == TYPE_FLOAT) {
                            /* Mark as float type for descriptor generation */
                            arg_node_type = NODE_LITERAL_FLOAT;
                        }
                    }
                }
            }
            else if (arg_node_type == NODE_ARRAY_ACCESS) {
                /* Check if array element is float type */
                uint16_t elem_type = get_array_element_type(codegen, arg_node);
                if (elem_type == TYPE_FLOAT) {
                    /* Mark as float type for descriptor generation */
                    arg_node_type = NODE_LITERAL_FLOAT;
                }
            }
            
        }
    }
    
    arg_count = 0;
    
    /* For instance method calls (non-native), push object reference first */
    if (object_idx != 0 && !is_native) {
        ASTNode* obj_node = codegen_get_node(codegen, object_idx);
        if (!obj_node) {
            codegen_error(codegen, "Invalid object in method call");
            return -1;
        }
        if (generate_expression(codegen, obj_node) != 0) {
            return -1;
        }
    }
    
    if (is_string_length || is_string_caseconv || is_string_compare || is_string_equals || is_string_compareto || is_string_indexof || is_string_lastindexof || is_string_substr) {
        ASTNode* recv_node = codegen_get_node(codegen, object_idx);
        if (!recv_node) {
            codegen_error(codegen, "Invalid String receiver");
            return -1;
        }
        if (generate_expression(codegen, recv_node) != 0) {
            return -1;
        }
        arg_count = 1;
        
        /* For string comparison, equals, compareTo, indexOf, and substr methods, also push the argument(s) */
        if (is_string_compare || is_string_equals || is_string_compareto || is_string_indexof || is_string_lastindexof || is_string_substr) {
            arg_idx = saved_first_arg;
            while (arg_idx != 0 && arg_count < total_arg_count + 1) {
                uint16_t next_arg_idx;
                
                arg_node = codegen_get_node(codegen, arg_idx);
                if (!arg_node) {
                    break;
                }
                
                next_arg_idx = arg_node->next_sibling;
                if (generate_expression(codegen, arg_node) != 0) {
                    return -1;
                }
                
                arg_idx = next_arg_idx;
                arg_count++;
            }
        }
    } else {
        arg_idx = saved_first_arg;
        while (arg_idx != 0 && arg_count < total_arg_count) {
            uint16_t next_arg_idx;
            TypeKind arg_type_kind;
            
            arg_node = codegen_get_node(codegen, arg_idx);
            if (!arg_node) {
                break;
            }
            
            next_arg_idx = arg_node->next_sibling;
            
            /* Save original argument node type before expression generation */
            {
                uint16_t original_arg_node_type = arg_node->type;
            
            /* Determine argument type before generating expression */
            arg_type_kind = TYPE_VOID;
            if (is_math_method) {
                /* Determine type of argument for Math methods */
                if (arg_node->type == NODE_LITERAL_INT || arg_node->type == NODE_LITERAL_BOOL) {
                    arg_type_kind = TYPE_INT;
                } else if (arg_node->type == NODE_LITERAL_LONG) {
                    arg_type_kind = TYPE_LONG;
                } else if (arg_node->type == NODE_LITERAL_FLOAT) {
                    arg_type_kind = TYPE_FLOAT;
                } else if (arg_node->type == NODE_IDENTIFIER) {
                    /* Look up variable type */
                    const char* var_name = codegen_get_string(codegen, arg_node->data.identifier.name);
                    if (var_name && codegen->symtable) {
                        for (i = 0; i < codegen->symtable->symbol_count; i++) {
                            Symbol* sym = &codegen->symtable->symbols[i];
                            const char* sym_name;
                            
                            if (sym->kind != SYM_LOCAL && sym->kind != SYM_PARAM && sym->kind != SYM_FIELD) {
                                continue;
                            }
                            
                            sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                            if (sym_name && strcmp(sym_name, var_name) == 0) {
                                arg_type_kind = sym->type.kind;
                                break;
                            }
                        }
                    }
                }
            }
            
            if (generate_expression(codegen, arg_node) != 0) {
                return -1;
            }
            
            /* For println, convert Exception variables to string marker
             * Only for direct Exception variable references, not for expressions like string concatenation
             */
            if ((strcmp(method_name, "println") == 0 || strcmp(method_name, "print") == 0) &&
                arg_node->type == NODE_IDENTIFIER &&
                original_arg_node_type == NODE_IDENTIFIER) {
                const char* arg_var_name;
                Symbol* arg_sym;
                uint16_t best_scope;
                uint16_t j;
                
                arg_var_name = codegen_get_string(codegen, arg_node->data.identifier.name);
                arg_sym = NULL;
                best_scope = 0;
                if (arg_var_name) {
                    for (j = 0; j < codegen->symtable->symbol_count; j++) {
                        Symbol* sym = &codegen->symtable->symbols[j];
                        const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                            sym_name && strcmp(sym_name, arg_var_name) == 0) {
                            if (!arg_sym || sym->scope_level >= best_scope) {
                                arg_sym = sym;
                                best_scope = sym->scope_level;
                            }
                        }
                    }
                    if (arg_sym && arg_sym->type.kind == TYPE_CLASS) {
                        const char* class_name = NULL;
                        if (arg_sym->type.class_name < codegen->pool_size) {
                            class_name = codegen_get_string(codegen, arg_sym->type.class_name);
                        }
                        if (class_name && strcmp(class_name, "Exception") == 0) {
                            /* Convert Exception reference to string marker for println */
                            emit_opcode(codegen, OP_EXCEPTION_TO_STRING);
                            /* Stack depth unchanged: exception ref -> string marker */
                        }
                    }
                }
            }
            
            }  /* End of original_arg_node_type scope */
            
            /* Insert implicit cast for Math methods if needed */
            if (is_math_method && arg_type_kind != TYPE_VOID && arg_type_kind != TYPE_FLOAT) {
                if (arg_type_kind == TYPE_INT) {
                    emit_opcode(codegen, OP_I2F);
                    update_stack(codegen, 1);  /* int (1 slot) -> float (2 slots) */
                } else if (arg_type_kind == TYPE_LONG) {
                    emit_opcode(codegen, OP_L2F);
                    /* long (2 slots) -> float (2 slots), no stack change */
                }
            }
            
            arg_idx = next_arg_idx;
            arg_count++;
        }
    }
    
    invoke_arg_count = arg_count;
    
    /* Adjust arg_count for long parameters (each long takes 2 words) */
    if (strcmp(method_name, "printLong") == 0) {
        /* printLong(long) takes 1 long argument = 2 words on stack */
        invoke_arg_count = 2;
    }
    
    method_idx = find_method_index(codegen, method_name, is_native);
    if (method_idx == 0xFFFF) {
        codegen_error(codegen, "Failed to add method reference");
        return -1;
    }
    
    
    if (is_native) {
        uint16_t desc_idx;
        char descriptor[80];  /* Increased buffer size to prevent overflow */
        
        if (is_string_length) {
            strcpy(descriptor, "(Ljava/lang/String;)I");
        } else if (is_string_caseconv) {
            strcpy(descriptor, "(Ljava/lang/String;)Ljava/lang/String;");
        } else if (is_string_compare || is_string_equals || is_string_compareto) {
            strcpy(descriptor, "(Ljava/lang/String;Ljava/lang/String;)I");
        } else if (is_string_indexof || is_string_lastindexof) {
            /* indexOf/lastIndexOf can have 1 or 2 arguments (not counting receiver) */
            /* arg_count includes receiver, so: 1-arg version has arg_count=2, 2-arg version has arg_count=3 */
            if (arg_count == 2) {
                strcpy(descriptor, "(Ljava/lang/String;Ljava/lang/String;)I");
            } else {
                strcpy(descriptor, "(Ljava/lang/String;Ljava/lang/String;I)I");
            }
        } else if (is_string_substr) {
            /* substr can have 1 or 2 arguments (not counting receiver) */
            /* arg_count includes receiver, so: 1-arg version has arg_count=2, 2-arg version has arg_count=3 */
            if (arg_count == 2) {
                strcpy(descriptor, "(Ljava/lang/String;I)Ljava/lang/String;");
            } else {
                strcpy(descriptor, "(Ljava/lang/String;II)Ljava/lang/String;");
            }
        } else if (strcmp(method_name, "concat") == 0) {
            strcpy(descriptor, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
        } else if (strcmp(method_name, "open") == 0) {
            /* File.open() can have 1 or 2 arguments */
            if (arg_count == 1) {
                strcpy(descriptor, "(Ljava/lang/String;)V");
            } else {
                strcpy(descriptor, "(Ljava/lang/String;Ljava/lang/String;)V");
            }
        } else if (strcmp(method_name, "readLine") == 0) {
            strcpy(descriptor, "()Ljava/lang/String;");
        } else if (strcmp(method_name, "writeLine") == 0) {
            strcpy(descriptor, "(Ljava/lang/String;)V");
        } else if (strcmp(method_name, "close") == 0) {
            /* Check if this is Socket.close(int) or File.close() */
            if (arg_count == 1) {
                /* Socket.close(int sock) */
                strcpy(descriptor, "(I)V");
            } else {
                /* File.close() */
                strcpy(descriptor, "()V");
            }
        } else if (strcmp(method_name, "init") == 0) {
            /* Socket.init() has no parameters */
            strcpy(descriptor, "()V");
        } else if (strcmp(method_name, "create") == 0) {
            /* Socket.create(String host, int port) returns int */
            strcpy(descriptor, "(Ljava/lang/String;I)I");
        } else if (strcmp(method_name, "send") == 0) {
            /* Socket.send(int sock, String data) returns int */
            strcpy(descriptor, "(ILjava/lang/String;)I");
        } else if (strcmp(method_name, "recv") == 0) {
            /* Socket.recv(int sock) returns String */
            strcpy(descriptor, "(I)Ljava/lang/String;");
        } else if (strcmp(method_name, "isConnected") == 0) {
            /* Socket.isConnected(int sock) returns int */
            strcpy(descriptor, "(I)I");
        } else if (strcmp(method_name, "printInt") == 0) {
            /* System.printInt(int) */
            strcpy(descriptor, "(I)V");
        } else if (strcmp(method_name, "printLong") == 0) {
            /* System.printLong(long) */
            strcpy(descriptor, "(J)V");
        } else if (strcmp(method_name, "parseInt") == 0) {
            /* Integer.parseInt(String) returns int */
            strcpy(descriptor, "(Ljava/lang/String;)I");
        } else if (strcmp(method_name, "getType") == 0) {
            /* Exception.getType() returns int */
            strcpy(descriptor, "()I");
        } else if (strcmp(method_name, "getMessage") == 0) {
            /* Exception.getMessage() returns String */
            strcpy(descriptor, "()Ljava/lang/String;");
        } else if (strcmp(method_name, "get") == 0) {
            /* Http.get can have 1 or 2 arguments */
            if (arg_count == 1) {
                /* Http.get(String url) returns String */
                strcpy(descriptor, "(Ljava/lang/String;)Ljava/lang/String;");
            } else {
                /* Http.get(String url, String headers) returns String */
                strcpy(descriptor, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
            }
        } else if (strcmp(method_name, "getStatusCode") == 0) {
            /* Http.getStatusCode(String) returns int */
            strcpy(descriptor, "(Ljava/lang/String;)I");
        } else if (strcmp(method_name, "post") == 0) {
            /* Http.post(String, String) returns String */
            strcpy(descriptor, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
        } else if (strcmp(method_name, "put") == 0) {
            /* Http.put(String, String) returns String */
            strcpy(descriptor, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
        } else if (strcmp(method_name, "delete") == 0) {
            /* Http.delete(String) returns String */
            strcpy(descriptor, "(Ljava/lang/String;)Ljava/lang/String;");
        } else if (strcmp(method_name, "abs") == 0 ||
                   strcmp(method_name, "sqrt") == 0 ||
                   strcmp(method_name, "sin") == 0 ||
                   strcmp(method_name, "cos") == 0 ||
                   strcmp(method_name, "tan") == 0 ||
                   strcmp(method_name, "exp") == 0 ||
                   strcmp(method_name, "log") == 0 ||
                   strcmp(method_name, "floor") == 0 ||
                   strcmp(method_name, "ceil") == 0) {
            /* Math.method(float) returns float */
            strcpy(descriptor, "(F)F");
            invoke_arg_count = 2;  /* Float takes 2 words on stack */
        } else if (strcmp(method_name, "min") == 0 ||
                   strcmp(method_name, "max") == 0 ||
                   strcmp(method_name, "pow") == 0) {
            /* Math.method(float, float) returns float */
            strcpy(descriptor, "(FF)F");
            invoke_arg_count = 4;  /* Two floats take 4 words on stack */
        } else if (arg_node_type == NODE_LITERAL_STRING || first_arg_is_string) {
            strcpy(descriptor, "(Ljava/lang/String;)V");
        } else if (arg_node_type == NODE_LITERAL_FLOAT) {
            /* System.out.println(float) */
            strcpy(descriptor, "(F)V");
            invoke_arg_count = 2;  /* Float takes 2 words on stack */
        } else if (arg_node_type == NODE_LITERAL_LONG) {
            /* System.out.println(long literal) */
            strcpy(descriptor, "(J)V");
            invoke_arg_count = 2;  /* Long takes 2 words on stack */
        } else if (arg_node_type == NODE_IDENTIFIER) {
            /* Check variable type for println */
            ASTNode* first_arg_node = codegen_get_node(codegen, saved_first_arg);
            if (first_arg_node) {
                const char* var_name = codegen_get_string(codegen, first_arg_node->data.identifier.name);
                TypeKind var_type = TYPE_INT;  /* Default to int */
                
                if (var_name && codegen->symtable) {
                    /* Search in reverse order to find the most recent (innermost scope) symbol */
                    for (i = codegen->symtable->symbol_count; i > 0; i--) {
                        Symbol* sym = &codegen->symtable->symbols[i - 1];
                        const char* sym_name;
                        
                        if (sym->kind != SYM_LOCAL && sym->kind != SYM_PARAM && sym->kind != SYM_FIELD) {
                            continue;
                        }
                        
                        sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if (sym_name && strcmp(sym_name, var_name) == 0) {
                            var_type = sym->type.kind;
                            break;
                        }
                    }
                }
                
                if (var_type == TYPE_LONG) {
                    strcpy(descriptor, "(J)V");
                    invoke_arg_count = 2;  /* Long takes 2 words on stack */
                } else if (var_type == TYPE_FLOAT) {
                    strcpy(descriptor, "(F)V");
                    invoke_arg_count = 2;  /* Float takes 2 words on stack */
                } else {
                    strcpy(descriptor, "(I)V");
                }
            } else {
                strcpy(descriptor, "(I)V");
            }
        } else {
            strcpy(descriptor, "(I)V");
        }
        
        
        desc_idx = find_or_add_utf8(codegen, descriptor);
        if (desc_idx != 0xFFFF) {
            codegen->methods[method_idx].descriptor_index = desc_idx;
        }
    }
    
    returns_value = 0;
    if (is_string_length || is_string_caseconv || is_string_compare || is_string_indexof ||
        is_string_lastindexof || is_string_substr || strcmp(method_name, "concat") == 0 ||
        strcmp(method_name, "readLine") == 0 || strcmp(method_name, "parseInt") == 0 ||
        strcmp(method_name, "getType") == 0 || strcmp(method_name, "getMessage") == 0) {
        returns_value = 1;
    } else if (strcmp(method_name, "abs") == 0 ||
               strcmp(method_name, "min") == 0 ||
               strcmp(method_name, "max") == 0 ||
               strcmp(method_name, "sqrt") == 0 ||
               strcmp(method_name, "sin") == 0 ||
               strcmp(method_name, "cos") == 0 ||
               strcmp(method_name, "tan") == 0 ||
               strcmp(method_name, "pow") == 0 ||
               strcmp(method_name, "exp") == 0 ||
               strcmp(method_name, "log") == 0 ||
               strcmp(method_name, "floor") == 0 ||
               strcmp(method_name, "ceil") == 0) {
        /* All Math methods return float (2 words) */
        returns_value = 2;  /* Float return value takes 2 words */
    } else if (!is_native) {
        method_sym = NULL;
        for (i = 0; i < codegen->symtable->symbol_count; i++) {
            Symbol* sym = &codegen->symtable->symbols[i];
            const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
            if (sym->kind == SYM_METHOD && sym_name && strcmp(sym_name, method_name) == 0) {
                method_sym = sym;
                break;
            }
        }
        if (method_sym && method_sym->type.kind != TYPE_VOID) {
            returns_value = 1;
        }
    }
    
    /* Check if this is an instance method call (non-native, non-special) */
    if (object_idx != 0 && !is_native) {
        /* Instance method call: object.method(...) */
        /* Object reference was already pushed before arguments */
        
        /* Emit OP_INVOKE_VIRTUAL with method name index (not method table index) */
        emit_opcode(codegen, OP_INVOKE_VIRTUAL);
        emit_u2(codegen, codegen->methods[method_idx].name_index);
        
        /* Stack: object + args consumed, result pushed if non-void */
        update_stack(codegen, -(int16_t)(invoke_arg_count + 1));
        if (returns_value) {
            update_stack(codegen, 1);
        }
    } else {
        /* Static method call or native method */
        emit_opcode(codegen, OP_INVOKE_STATIC);
        emit_u2(codegen, method_idx);
        emit_u1(codegen, (uint8_t)invoke_arg_count);
        
        update_stack(codegen, -(int16_t)invoke_arg_count);
        if (returns_value) {
            update_stack(codegen, 1);
        }
    }
    
    return 0;
}

/* Generate code for identifier */
int generate_identifier(CodeGenerator* codegen, ASTNode* id_node) {
    const char* var_name;
    uint16_t local_idx;
    Symbol* field_sym;
    uint16_t i;
    
    if (!codegen || !id_node) {
        return -1;
    }
    
    /* Get variable name */
    var_name = codegen_get_string(codegen, id_node->data.identifier.name);
    if (!var_name) {
        return -1;
    }
    
    /* Check if this is a builtin class name (Socket, File, System, Math, Integer, Http) */
    /* These are used for static method calls and should not be treated as variables */
    if (strcmp(var_name, "Socket") == 0 ||
        strcmp(var_name, "File") == 0 ||
        strcmp(var_name, "System") == 0 ||
        strcmp(var_name, "Math") == 0 ||
        strcmp(var_name, "Integer") == 0 ||
        strcmp(var_name, "Http") == 0) {
        /* For builtin classes, we don't push anything on the stack */
        /* The method call handler will deal with it */
        return 0;
    }
    
    /* Try to get local variable index */
    local_idx = get_local_index(codegen, var_name);
    
    /* DEBUG: Print variable name and index */
    if (local_idx != 0xFFFF) {
    }
    
    /* If found as local variable, load it */
    if (local_idx != 0xFFFF) {
        /* Check variable type */
        /* Search in reverse order to find innermost scope first */
        int is_float = 0;
        int is_long = 0;
        int is_exception = 0;
        for (i = codegen->symtable->symbol_count; i > 0; i--) {
            Symbol* sym = &codegen->symtable->symbols[i - 1];
            const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
            if ((sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) &&
                sym_name && strcmp(sym_name, var_name) == 0) {
                if (sym->type.kind == TYPE_FLOAT) {
                    is_float = 1;
                } else if (sym->type.kind == TYPE_LONG) {
                    is_long = 1;
                } else if (sym->type.kind == TYPE_CLASS) {
                    /* Check if it's an Exception type */
                    const char* class_name = symtable_get_string(codegen->symtable, sym->type.class_name);
                    if (class_name && strcmp(class_name, "Exception") == 0) {
                        is_exception = 1;
                    }
                }
                break;
            }
        }
        
        if (is_float) {
            /* Load float variable (uses 2 stack slots) */
            emit_opcode(codegen, OP_LOAD_FLOAT);
            emit_u1(codegen, (uint8_t)local_idx);
            update_stack(codegen, 2);
        } else if (is_long) {
            /* Load long variable (uses 2 stack slots) */
            emit_opcode(codegen, OP_LOAD_LONG);
            emit_u1(codegen, (uint8_t)local_idx);
            update_stack(codegen, 2);
        } else {
            /* Load int/boolean/reference variable */
            if (local_idx <= 2) {
                emit_opcode(codegen, OP_LOAD_0 + local_idx);
            } else {
                emit_opcode(codegen, OP_LOAD_LOCAL);
                emit_u1(codegen, (uint8_t)local_idx);
            }
            update_stack(codegen, 1);
            
            /* If it's an Exception variable, we need to handle it specially */
            /* For println: convert to 0xFFFF marker */
            /* For string concat: load "Exception" string constant */
            /* We'll handle this in the parent expression context */
            if (is_exception) {
                /* For now, just push the exception reference */
                /* The parent context will decide how to convert it */
            }
        }
        return 0;
    }
    
    /* Not a local variable - check if it's a field in current class */
    field_sym = NULL;
    for (i = 0; i < codegen->symtable->symbol_count; i++) {
        Symbol* sym = &codegen->symtable->symbols[i];
        if (sym->kind == SYM_FIELD) {
            const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
            if (sym_name && strcmp(sym_name, var_name) == 0) {
                field_sym = sym;
                break;
            }
        }
    }
    
    if (field_sym) {
        /* It's a field - generate implicit 'this.field' access */
        uint16_t field_idx;
        
        /* Load 'this' (always at local index 0 in instance methods) */
        emit_opcode(codegen, OP_LOAD_0);
        update_stack(codegen, 1);
        
        /* Add field name to constant pool */
        field_idx = find_or_add_utf8(codegen, var_name);
        if (field_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add field constant");
            return -1;
        }
        
        /* Emit OP_GET_FIELD */
        emit_opcode(codegen, OP_GET_FIELD);
        emit_u2(codegen, field_idx);
        /* Stack: this -> value (no change in depth) */
        return 0;
    }
    
    /* Neither local nor field - error */
    codegen_error(codegen, "Undefined variable");
    return -1;
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
    
    /* Debug: print opcode being emitted */
    if (opcode == 0x51 || opcode == 0x41) {
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

uint16_t build_method_descriptor(CodeGenerator* codegen, Symbol* method_sym) {
    char descriptor[64];
    char param_desc[8][20];
    uint16_t method_pos;
    uint16_t symbol_idx;
    uint16_t method_index;
    uint16_t param_count;
    uint16_t found_params;
    uint16_t param_idx;
    
    if (!codegen || !method_sym || !codegen->symtable || method_sym->kind != SYM_METHOD) {
        return 0xFFFF;
    }
    
    method_index = 0xFFFF;
    for (symbol_idx = 0; symbol_idx < codegen->symtable->symbol_count; symbol_idx++) {
        if (&codegen->symtable->symbols[symbol_idx] == method_sym) {
            method_index = symbol_idx;
            break;
        }
    }
    if (method_index == 0xFFFF) {
        return 0xFFFF;
    }
    
    param_count = method_sym->data.method_data.param_count;
    if (param_count > 8) {
        return 0xFFFF;
    }
    
    for (param_idx = 0; param_idx < 8; param_idx++) {
        param_desc[param_idx][0] = '\0';
    }
    
    found_params = 0;
    for (symbol_idx = method_index + 1; symbol_idx < codegen->symtable->symbol_count && found_params < param_count; symbol_idx++) {
        Symbol* sym = &codegen->symtable->symbols[symbol_idx];
        if (sym->kind == SYM_METHOD) {
            break;
        }
        if (sym->kind != SYM_PARAM) {
            continue;
        }
        if (sym->data.param_data.index >= param_count) {
            continue;
        }
        
        if (sym->type.kind == TYPE_INT) {
            strcpy(param_desc[sym->data.param_data.index], "I");
        } else if (sym->type.kind == TYPE_CLASS) {
            const char* class_name = symtable_get_string(codegen->symtable, sym->type.class_name);
            if (class_name && strcmp(class_name, "String") == 0) {
                strcpy(param_desc[sym->data.param_data.index], "Ljava/lang/String;");
            } else {
                return 0xFFFF;
            }
        } else {
            return 0xFFFF;
        }
        found_params++;
    }
    
    if (found_params != param_count) {
        return 0xFFFF;
    }
    
    method_pos = 0;
    descriptor[method_pos++] = '(';
    for (param_idx = 0; param_idx < param_count; param_idx++) {
        uint16_t len = (uint16_t)strlen(param_desc[param_idx]);
        if (len == 0) {
            return 0xFFFF;
        }
        strcpy(&descriptor[method_pos], param_desc[param_idx]);
        method_pos += len;
    }
    
    descriptor[method_pos++] = ')';
    if (method_sym->type.kind == TYPE_VOID) {
        descriptor[method_pos++] = 'V';
    } else if (method_sym->type.kind == TYPE_INT) {
        descriptor[method_pos++] = 'I';
    } else if (method_sym->type.kind == TYPE_CLASS) {
        const char* class_name = symtable_get_string(codegen->symtable, method_sym->type.class_name);
        if (class_name && strcmp(class_name, "String") == 0) {
            strcpy(&descriptor[method_pos], "Ljava/lang/String;");
            method_pos += 18;
        } else {
            return 0xFFFF;
        }
    } else {
        return 0xFFFF;
    }
    
    descriptor[method_pos] = '\0';
    return find_or_add_utf8(codegen, descriptor);
}

/* Find or create method index */
uint16_t find_method_index(CodeGenerator* codegen, const char* method_name, int is_native) {
    uint16_t i;
    uint16_t name_idx;
    uint16_t descriptor_idx;
    Symbol* target_method_sym;
    
    if (!codegen || !method_name) {
        return 0xFFFF;
    }
    
    if (is_native) {
        /* Native methods are keyed by name first; caller updates descriptor afterward.
         * For overloaded methods (like indexOf), we need to create separate entries.
         * We'll return the existing one only if it doesn't have a descriptor yet (descriptor_index == 0).
         */
        name_idx = find_or_add_utf8(codegen, method_name);
        if (name_idx == 0xFFFF) {
            return 0xFFFF;
        }
        
        /* Only reuse if descriptor hasn't been set yet (to support overloading) */
        for (i = 0; i < codegen->method_count; i++) {
            if ((codegen->methods[i].flags & METHOD_NATIVE) != 0 &&
                codegen->methods[i].name_index == name_idx &&
                codegen->methods[i].descriptor_index == 0) {
                return i;
            }
        }
        
        if (codegen->method_count >= 64) {
            return 0xFFFF;
        }
        
        codegen->methods[codegen->method_count].name_index = name_idx;
        codegen->methods[codegen->method_count].descriptor_index = 0;
        codegen->methods[codegen->method_count].code_offset = 0;
        codegen->methods[codegen->method_count].code_length = 0;
        codegen->methods[codegen->method_count].max_stack = 0;
        codegen->methods[codegen->method_count].max_locals = 0;
        codegen->methods[codegen->method_count].flags = METHOD_NATIVE;
        
        codegen->method_count++;
        return (uint16_t)(codegen->method_count - 1);
    }
    
    target_method_sym = NULL;
    for (i = 0; i < codegen->symtable->symbol_count; i++) {
        Symbol* sym = &codegen->symtable->symbols[i];
        const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
        if (sym->kind == SYM_METHOD && sym_name && strcmp(sym_name, method_name) == 0) {
            target_method_sym = sym;
            break;
        }
    }
    
    descriptor_idx = build_method_descriptor(codegen, target_method_sym);
    
    name_idx = find_or_add_utf8(codegen, method_name);
    if (name_idx == 0xFFFF) {
        return 0xFFFF;
    }
    
    for (i = 0; i < codegen->method_count; i++) {
        uint16_t existing_name_idx = codegen->methods[i].name_index;
        
        if (existing_name_idx < codegen->constants->count) {
            if (codegen->constants->constants[existing_name_idx].tag == CONST_UTF8) {
                const char* existing_name = codegen->constants->constants[existing_name_idx].data.utf8_data;
                
                if (strcmp(existing_name, method_name) == 0) {
                    int is_method_native = (codegen->methods[i].flags & METHOD_NATIVE) != 0;
                    if (!is_method_native) {
                        if (descriptor_idx == 0xFFFF || codegen->methods[i].descriptor_index == 0 ||
                            codegen->methods[i].descriptor_index == descriptor_idx) {
                            return i;
                        }
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
    codegen->methods[codegen->method_count].descriptor_index = (descriptor_idx == 0xFFFF) ? 0 : descriptor_idx;
    codegen->methods[codegen->method_count].code_offset = 0;
    codegen->methods[codegen->method_count].code_length = 0;
    codegen->methods[codegen->method_count].max_stack = 0;
    codegen->methods[codegen->method_count].max_locals = 0;
    codegen->methods[codegen->method_count].flags = 0;
    
    codegen->method_count++;
    return (uint16_t)(codegen->method_count - 1);
}

/* Create label */
uint16_t create_label(CodeGenerator* codegen) {
    uint16_t idx;
    
    if (!codegen || !codegen->context || !codegen->context->labels) {
        return 0;
    }
    
    if (codegen->context->labels->count >= 128) {
        return 0;
    }
    
    idx = codegen->context->labels->count;
    codegen->context->labels->labels[idx].offset_count = 0;
    codegen->context->labels->labels[idx].target = -1;
    codegen->context->labels->count++;
    
    /* Return idx + 1 so that 0 can be used as "no label" */
    return idx + 1;
}

/* Emit label */
int emit_label(CodeGenerator* codegen, uint16_t label_index) {
    uint16_t actual_index;
    
    if (!codegen || !codegen->context || !codegen->context->labels) {
        return -1;
    }
    
    if (label_index == 0) {
        return -1;  /* 0 is invalid label */
    }
    
    actual_index = label_index - 1;
    
    if (actual_index >= codegen->context->labels->count) {
        return -1;
    }
    
    /* Set label target to current code position */
    codegen->context->labels->labels[actual_index].target = codegen->context->code->size;
    
    return 0;
}

/* Emit jump */
int emit_jump(CodeGenerator* codegen, uint8_t opcode, uint16_t label_index) {
    Label* label;
    uint16_t actual_index;
    
    if (!codegen || !codegen->context || !codegen->context->labels) {
        return -1;
    }
    
    if (label_index == 0) {
        return -1;  /* 0 is invalid label */
    }
    
    actual_index = label_index - 1;
    
    if (actual_index >= codegen->context->labels->count) {
        return -1;
    }
    
    label = &codegen->context->labels->labels[actual_index];
    
    /* Check if we have space for another offset */
    if (label->offset_count >= 16) {
        codegen_error(codegen, "Too many jumps to same label");
        return -1;
    }
    
    /* Emit jump opcode */
    emit_opcode(codegen, opcode);
    
    /* Record offset for backpatching */
    label->offsets[label->offset_count] = codegen->context->code->size;
    label->offset_count++;
    
    /* Emit placeholder offset */
    emit_u2(codegen, 0);
    
    return 0;
}

/* Backpatch labels */
int backpatch_labels(CodeGenerator* codegen) {
    uint16_t i, j;
    Label* label;
    int16_t offset;
    uint16_t jump_offset;
    
    if (!codegen || !codegen->context || !codegen->context->labels) {
        return -1;
    }
    
    /* Backpatch all labels */
    for (i = 0; i < codegen->context->labels->count; i++) {
        label = &codegen->context->labels->labels[i];
        
        if (label->target >= 0) {
            /* Backpatch all jumps to this label */
            for (j = 0; j < label->offset_count; j++) {
                jump_offset = label->offsets[j];
                
                /* Calculate relative offset */
                offset = label->target - (jump_offset + 2);
                
                /* Patch offset in code */
                codegen->context->code->data[jump_offset] = (uint8_t)(offset & 0xFF);
                codegen->context->code->data[jump_offset + 1] = (uint8_t)(offset >> 8);
            }
        }
    }
    
    return 0;
}

/* Get expression type */
static uint16_t get_expression_type(CodeGenerator* codegen, ASTNode* expr_node) {
    Symbol* sym;
    const char* var_name;
    uint16_t i;
    
    if (!codegen || !expr_node) {
        return (uint16_t)TYPE_INT;  /* Default to int */
    }
    
    /* Check node type */
    switch (expr_node->type) {
        case NODE_LITERAL_LONG:
            return (uint16_t)TYPE_LONG;
        
        case NODE_LITERAL_FLOAT:
            return (uint16_t)TYPE_FLOAT;
            
        case NODE_LITERAL_INT:
        case NODE_LITERAL_BOOL:
            return (uint16_t)TYPE_INT;
        
        case NODE_LITERAL_NULL:
            return (uint16_t)TYPE_NULL;
            
        case NODE_IDENTIFIER:
            var_name = codegen_get_string(codegen, expr_node->data.identifier.name);
            if (var_name && codegen->symtable) {
                /* Find the symbol */
                for (i = 0; i < codegen->symtable->symbol_count; i++) {
                    sym = &codegen->symtable->symbols[i];
                    if (sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) {
                        const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                        if (sym_name && strcmp(sym_name, var_name) == 0) {
                            return (uint16_t)sym->type.kind;
                        }
                    }
                }
            }
            break;
            
        case NODE_ARRAY_ACCESS:
            return get_array_element_type(codegen, expr_node);
            
        default:
            break;
    }
    
    return (uint16_t)TYPE_INT;  /* Default to int if not found */
}

/* Get array element type from array expression */
static uint16_t get_array_element_type(CodeGenerator* codegen, ASTNode* array_node) {
    Symbol* sym;
    const char* var_name;
    uint16_t i;
    uint16_t result;
    ASTNode* base_array;
    
    if (!codegen || !array_node) {
        return (uint16_t)TYPE_INT;  /* Default to int */
    }
    
    /* If array node is NODE_ARRAY_ACCESS, get the base array identifier */
    if (array_node->type == NODE_ARRAY_ACCESS) {
        base_array = codegen_get_node(codegen, array_node->data.array_access.array);
        if (base_array) {
            /* Recursively get element type from base array */
            return get_array_element_type(codegen, base_array);
        }
    }
    
    /* If array node is an identifier, look up its type in symbol table */
    if (array_node->type == NODE_IDENTIFIER) {
        var_name = codegen_get_string(codegen, array_node->data.identifier.name);
        if (var_name && codegen->symtable) {
            /* Find the symbol */
            for (i = 0; i < codegen->symtable->symbol_count; i++) {
                sym = &codegen->symtable->symbols[i];
                if (sym->kind == SYM_LOCAL || sym->kind == SYM_PARAM) {
                    const char* sym_name = symtable_get_string(codegen->symtable, sym->name_offset);
                    if (sym_name && strcmp(sym_name, var_name) == 0) {
                        /* Found the symbol - check if it's an array */
                        if (sym->type.kind == TYPE_ARRAY) {
                            /* Return element type stored in class_name field */
                            result = sym->type.class_name;
                            return result;
                        }
                        break;
                    }
                }
            }
        }
    }
    
    return (uint16_t)TYPE_INT;  /* Default to int if not found */
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
        /* For instance methods, add 1 for 'this' parameter at local[0] */
        if (codegen->current_method && !codegen->current_method->data.method_data.is_static) {
            result++;
        }
        return result;
    } else if (best_sym->kind == SYM_LOCAL) {
        /* Local variables come after parameters */
        if (codegen->current_method) {
            param_count = codegen->current_method->data.method_data.param_count;
            result = param_count + best_sym->data.local_data.index;
            
            /* Adjust for long/float variables that use 2 slots */
            {
                uint16_t j;
                uint16_t actual_index = param_count;
                for (j = 0; j < codegen->symtable->symbol_count; j++) {
                    Symbol* local_sym = &codegen->symtable->symbols[j];
                    if (local_sym->kind == SYM_LOCAL &&
                        local_sym->data.local_data.index < best_sym->data.local_data.index) {
                        /* This local comes before our target variable */
                        if (local_sym->type.kind == TYPE_LONG || local_sym->type.kind == TYPE_FLOAT) {
                            actual_index += 2;  /* long/float use 2 slots */
                        } else {
                            actual_index += 1;
                        }
                    }
                }
                return actual_index;
            }
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
    header.line_number_table_count = codegen->line_numbers ? codegen->line_numbers->count : 0;
    
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
        const char* method_name = NULL;
        
        if (m->name_index < codegen->constants->count) {
            method_name = codegen->constants->constants[m->name_index].data.utf8_data;
        }
        
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
    
    /* Write line number table (version 0x0002+) */
    if (codegen->line_numbers && codegen->line_numbers->count > 0) {
        for (i = 0; i < codegen->line_numbers->count; i++) {
            CodeGenLineEntry* entry = &codegen->line_numbers->entries[i];
            
            /* Write PC */
            if (fwrite(&entry->pc, sizeof(uint16_t), 1, codegen->output_file) != 1) {
                return -1;
            }
            
            /* Write line number */
            if (fwrite(&entry->line_no, sizeof(uint16_t), 1, codegen->output_file) != 1) {
                return -1;
            }
        }
    }
    
    /* Flush output to ensure all data is written */
    if (fflush(codegen->output_file) != 0) {
        return -1;
    }
    
    return 0;
}




/* Check if a block ends with a break statement */
static int block_ends_with_break(CodeGenerator* codegen, ASTNode* block_node) {
    ASTNode* stmt_node;
    uint16_t stmt_idx;
    uint16_t stmt_count;
    uint16_t i;
    
    if (!block_node || block_node->type != NODE_BLOCK) {
        return 0;
    }
    
    stmt_count = block_node->data.block.stmt_count;
    if (stmt_count == 0) {
        /* Empty block - falls through */
        return 0;
    }
    
    /* Find the last statement in the block */
    stmt_idx = block_node->data.block.first_stmt;
    for (i = 1; i < stmt_count && stmt_idx != 0; i++) {
        stmt_node = codegen_get_node(codegen, stmt_idx);
        if (!stmt_node) {
            return 0;
        }
        stmt_idx = stmt_node->next_sibling;
    }
    
    /* Check if last statement is a break */
    if (stmt_idx != 0) {
        stmt_node = codegen_get_node(codegen, stmt_idx);
        if (stmt_node && stmt_node->type == NODE_BREAK) {
            return 1;
        }
    }
    
    return 0;
}

/* Generate case comparison and conditional jump */
static int generate_case_comparison(CodeGenerator* codegen, ASTNode* case_node, uint16_t switch_expr_type,
                                    uint16_t end_label, uint16_t this_body_label, uint16_t next_body_label) {
    ASTNode* value_node;
    ASTNode* stmt_node;
    uint16_t value_idx;
    uint16_t stmt_idx;
    uint16_t next_case_label;
    int has_break;
    
    if (!case_node || case_node->type != NODE_CASE) {
        return -1;
    }
    
    /* Save case data before any codegen_get_node calls */
    value_idx = case_node->data.case_label.value;
    stmt_idx = case_node->data.case_label.stmt;
    
    /* Get case value */
    value_node = codegen_get_node(codegen, value_idx);
    if (!value_node) {
        return -1;
    }
    
    /* Create label for next case comparison */
    next_case_label = create_label(codegen);
    
    /* Duplicate switch value on stack for comparison */
    /* For INT/STRING: use DUP twice */
    /* For LONG: load from temporary variable (stored in generate_switch_stmt) */
    if (switch_expr_type == TYPE_LONG) {
        /* Load switch value from temporary local variable 126 */
        emit_opcode(codegen, OP_LOAD_LONG);
        emit_u1(codegen, 126);
        codegen->context->current_stack += 2;  /* LONG uses 2 slots */
    } else {
        /* We need TWO copies: one for comparison, one to keep for next case */
        /* Stack: [switch_value] -> [switch_value, switch_value, switch_value] */
        emit_opcode(codegen, OP_DUP);
        codegen->context->current_stack++;
        emit_opcode(codegen, OP_DUP);
        codegen->context->current_stack++;
    }
    
    /* Generate case value expression */
    /* Stack: [switch_value, switch_value, switch_value, case_value] */
    if (generate_expression(codegen, value_node) != 0) {
        return -1;
    }
    
    /* Compare: if equal, jump to case body */
    if (switch_expr_type == TYPE_INT) {
        emit_opcode(codegen, OP_CMP_EQ);
        /* CMP_EQ: 2 consumed, 1 produced = net -1 */
        codegen->context->current_stack--;
    } else if (switch_expr_type == TYPE_LONG) {
        /* For long: use LCMP, push 0, then check if equal */
        /* LCMP consumes 4 slots (2 longs) and produces 1 slot (int result) */
        emit_opcode(codegen, OP_LCMP);
        codegen->context->current_stack -= 3;  /* 4 consumed, 1 produced = net -3 */
        emit_opcode(codegen, OP_PUSH_INT);
        emit_u2(codegen, 0);
        codegen->context->current_stack++;
        emit_opcode(codegen, OP_CMP_EQ);
        /* CMP_EQ: 2 consumed, 1 produced = net -1 */
        codegen->context->current_stack--;
    } else if (switch_expr_type == TYPE_CLASS) {
        /* For String: call String.equals() method */
        uint16_t method_idx;
        uint16_t desc_idx;
        char descriptor[80];
        
        /* Find or add "equals" method reference */
        method_idx = find_method_index(codegen, "equals", 1);  /* 1 = is_native */
        if (method_idx == 0xFFFF) {
            codegen_error(codegen, "Failed to add equals method reference");
            return -1;
        }
        
        /* Create descriptor for String.equals: (Ljava/lang/String;Ljava/lang/String;)I */
        strcpy(descriptor, "(Ljava/lang/String;Ljava/lang/String;)I");
        desc_idx = find_or_add_utf8(codegen, descriptor);
        if (desc_idx == 0) {
            codegen_error(codegen, "Failed to add equals descriptor");
            return -1;
        }
        
        /* Emit INVOKE_STATIC for String.equals */
        emit_opcode(codegen, OP_INVOKE_STATIC);
        emit_u2(codegen, method_idx);
        emit_u1(codegen, 2);  /* 2 arguments: receiver + parameter */
        
        /* Stack after: [switch_value, result] where result is 1 (equal) or 0 (not equal) */
        /* INVOKE_STATIC: 2 args consumed, 1 result pushed = net -1 */
        codegen->context->current_stack--;
    } else {
        codegen_error(codegen, "Unsupported switch expression type");
        return -1;
    }
    
    /* Stack after comparison: [switch_value, result] */
    /* If not equal (result is 0), jump to next case (switch_value stays on stack) */
    emit_jump(codegen, OP_IF_FALSE, next_case_label);
    codegen->context->current_stack--;  /* IF_FALSE consumes result */
    
    /* Equal: We matched! Pop switch value and execute case body */
    /* For INT/STRING: pop from stack */
    /* For LONG: no need to pop (using temporary variable) */
    if (switch_expr_type != TYPE_LONG) {
        emit_opcode(codegen, OP_POP);
        codegen->context->current_stack--;
    }
    
    /* Emit case body label (for fall-through from previous case) */
    emit_label(codegen, this_body_label);
    
    /* Check if case body is empty or ends with break */
    if (stmt_idx != 0) {
        stmt_node = codegen_get_node(codegen, stmt_idx);
        if (!stmt_node) {
            codegen_error(codegen, "Failed to get case body statement node");
            return -1;
        }
        has_break = block_ends_with_break(codegen, stmt_node);
    } else {
        /* Empty case body - will fall through */
        has_break = 0;
    }
    
    /* Generate case body */
    if (stmt_idx != 0) {
        stmt_node = codegen_get_node(codegen, stmt_idx);
        if (stmt_node) {
            if (generate_statement(codegen, stmt_node) != 0) {
                return -1;
            }
        } else {
            codegen_error(codegen, "Failed to get case body statement node");
            return -1;
        }
    }
    /* else: Empty case body - this is valid for fall-through */
    
    /* After case body: */
    /* - If has break: jump to end */
    /* - If no break and has next case: jump to next case body (skip its comparison) */
    /* - If no break and no next case: fall through to default/end */
    if (has_break) {
        emit_jump(codegen, OP_GOTO, end_label);
    } else if (next_body_label != 0) {
        /* Fall-through: jump directly to next case body, skipping its comparison */
        emit_jump(codegen, OP_GOTO, next_body_label);
    }
    /* else: No break and no next case - will fall through to default/end naturally */
    
    /* Emit label for next case comparison */
    emit_label(codegen, next_case_label);
    
    return 0;
}

/* Generate switch statement */
static int generate_switch_stmt(CodeGenerator* codegen, ASTNode* switch_node) {
    ASTNode* expr_node;
    ASTNode* case_node;
    ASTNode* default_node;
    uint16_t expr_idx;
    uint16_t case_idx;
    uint16_t default_idx;
    uint16_t switch_expr_type;
    uint16_t end_label;
    uint16_t default_label;
    uint16_t old_break_label;
    uint16_t first_case;
    uint8_t has_default;
    
    if (!switch_node || switch_node->type != NODE_SWITCH) {
        codegen_error(codegen, "Invalid switch node");
        return -1;
    }
    
    /* Save switch node data before any codegen_get_node calls */
    expr_idx = switch_node->data.switch_stmt.expr;
    first_case = switch_node->data.switch_stmt.first_case;
    has_default = switch_node->data.switch_stmt.has_default;
    default_idx = switch_node->data.switch_stmt.default_stmt;
    
    /* Create end label for break statements */
    end_label = create_label(codegen);
    
    /* Save old break label and set new one */
    old_break_label = codegen->break_label;
    codegen->break_label = end_label;
    
    /* Get switch expression */
    expr_node = codegen_get_node(codegen, expr_idx);
    if (!expr_node) {
        codegen_error(codegen, "Failed to get switch expression node");
        codegen->break_label = old_break_label;
        return -1;
    }
    
    /* Get switch expression type */
    switch_expr_type = get_expression_type(codegen, expr_node);
    
    /* Generate switch expression */
    if (generate_expression(codegen, expr_node) != 0) {
        codegen_error(codegen, "Failed to generate switch expression");
        codegen->break_label = old_break_label;
        return -1;
    }
    
    /* For LONG type, we need to store switch value in a temporary local variable */
    /* because OP_DUP only duplicates 1 slot, but LONG uses 2 slots */
    if (switch_expr_type == TYPE_LONG) {
        /* Use local variable 126 as temporary storage for switch value */
        /* 126+1=127 is within SHARED_LOCALS_SIZE (128) */
        /* This assumes user code doesn't use local 126-127 */
        emit_opcode(codegen, OP_STORE_LONG);
        emit_u1(codegen, 126);
        codegen->context->current_stack -= 2;  /* LONG uses 2 slots */
    }
    
    /* Create default label if needed */
    if (has_default) {
        default_label = create_label(codegen);
    } else {
        default_label = end_label;
    }
    
    /* First pass: Create body labels for all cases */
    /* We need to know the next case's body label for fall-through */
    {
        uint16_t* body_labels;
        uint16_t case_count = 0;
        uint16_t i;
        
        /* Count cases */
        case_idx = first_case;
        while (case_idx != 0) {
            case_node = codegen_get_node(codegen, case_idx);
            if (!case_node) {
                codegen->break_label = old_break_label;
                return -1;
            }
            case_count++;
            case_idx = case_node->data.case_label.next_case;
        }
        
        /* Allocate array for body labels */
        body_labels = (uint16_t*)malloc(case_count * sizeof(uint16_t));
        if (!body_labels) {
            codegen_error(codegen, "Failed to allocate memory for case body labels");
            codegen->break_label = old_break_label;
            return -1;
        }
        
        /* Create all body labels */
        for (i = 0; i < case_count; i++) {
            body_labels[i] = create_label(codegen);
        }
        
        /* Second pass: Generate case comparisons with body labels */
        case_idx = first_case;
        i = 0;
        while (case_idx != 0) {
            uint16_t next_case_idx;
            uint16_t this_body_label;
            uint16_t next_body_label;
            
            case_node = codegen_get_node(codegen, case_idx);
            if (!case_node) {
                free(body_labels);
                codegen->break_label = old_break_label;
                return -1;
            }
            
            /* Save next_case before calling generate_case_comparison */
            next_case_idx = case_node->data.case_label.next_case;
            
            /* Get this case's body label and next case's body label */
            this_body_label = body_labels[i];
            next_body_label = (i + 1 < case_count) ? body_labels[i + 1] : 0;
            
            if (generate_case_comparison(codegen, case_node, switch_expr_type, end_label,
                                        this_body_label, next_body_label) != 0) {
                free(body_labels);
                codegen->break_label = old_break_label;
                return -1;
            }
            
            /* Move to next case */
            case_idx = next_case_idx;
            i++;
        }
        
        free(body_labels);
    }
    
    /* If no case matched, jump to default or end */
    /* For INT/STRING: pop switch value from stack */
    /* For LONG: no need to pop (using temporary variable) */
    if (switch_expr_type != TYPE_LONG) {
        emit_opcode(codegen, OP_POP);
        codegen->context->current_stack--;
    }
    emit_jump(codegen, OP_GOTO, default_label);
    
    /* Generate default case if present */
    if (has_default) {
        emit_label(codegen, default_label);
        
        if (default_idx != 0) {
            default_node = codegen_get_node(codegen, default_idx);
            if (default_node) {
                if (generate_statement(codegen, default_node) != 0) {
                    codegen->break_label = old_break_label;
                    return -1;
                }
            }
        }
    }
    
    /* Emit end label */
    emit_label(codegen, end_label);
    
    /* Restore old break label */
    codegen->break_label = old_break_label;
    
    return 0;
}
