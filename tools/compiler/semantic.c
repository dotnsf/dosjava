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
static int current_scope_has_local_name(SemanticAnalyzer* analyzer, const char* name, SymbolKind kind);
static int is_string_type(SemanticAnalyzer* analyzer, TypeInfo type);
static int register_builtin_classes(SemanticAnalyzer* analyzer);
static uint16_t semantic_add_string(SemanticAnalyzer* analyzer, const char* str);
static int check_switch_stmt(SemanticAnalyzer* analyzer, ASTNode* switch_node);
static int check_case_label(SemanticAnalyzer* analyzer, ASTNode* case_node, TypeInfo switch_type);
static int is_constant_expression(ASTNode* expr_node);

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
    
    /* Register built-in classes */
    if (register_builtin_classes(analyzer) != 0) {
        symtable_cleanup(analyzer->symtable);
        free(analyzer->symtable);
        fclose(analyzer->ast_file);
        return -1;
    }
    
    return 0;
}

/* Register built-in classes */
static int register_builtin_classes(SemanticAnalyzer* analyzer) {
    Symbol class_sym, method_sym;
    uint16_t name_offset, method_offset;
    const char* builtin_classes[] = {
        "System",
        "BufferedWriter",
        "BufferedReader",
        "FileOutputStream",
        "FileInputStream",
        "OutputStream",
        "InputStream",
        "Date",
        "Socket",
        NULL
    };
    int i;
    
    for (i = 0; builtin_classes[i] != NULL; i++) {
        /* Add class name to string pool */
        name_offset = semantic_add_string(analyzer, builtin_classes[i]);
        if (name_offset == 0xFFFF) {
            return -1;
        }
        
        /* Create class symbol */
        memset(&class_sym, 0, sizeof(Symbol));
        class_sym.kind = SYM_CLASS;
        class_sym.name_offset = name_offset;
        class_sym.type.kind = TYPE_CLASS;
        class_sym.type.class_name = name_offset;
        class_sym.data.class_data.member_count = 0;
        
        /* Add to symbol table */
        if (symtable_add_symbol(analyzer->symtable, &class_sym) == 0xFFFF) {
            return -1;
        }
        
        /* Add common methods for I/O classes */
        if (strcmp(builtin_classes[i], "BufferedWriter") == 0) {
            /* Add write(String) method for BufferedWriter */
            uint16_t string_class_offset, param_offset;
            Symbol param_sym;
            
            method_offset = semantic_add_string(analyzer, "write");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_VOID;
            method_sym.data.method_data.param_count = 1;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 0;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameter symbol for write(String str) */
            param_offset = semantic_add_string(analyzer, "str");
            if (param_offset == 0xFFFF) {
                return -1;
            }
            
            string_class_offset = semantic_add_string(analyzer, "String");
            if (string_class_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&param_sym, 0, sizeof(Symbol));
            param_sym.kind = SYM_PARAM;
            param_sym.name_offset = param_offset;
            param_sym.type.kind = TYPE_CLASS;
            param_sym.type.class_name = string_class_offset;
            param_sym.data.param_data.index = 0;
            
            if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                return -1;
            }
        }
        
        /* Add writeLine(String) method for BufferedWriter */
        if (strcmp(builtin_classes[i], "BufferedWriter") == 0) {
            uint16_t string_class_offset, param_offset;
            Symbol param_sym;
            
            method_offset = semantic_add_string(analyzer, "writeLine");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_VOID;
            method_sym.data.method_data.param_count = 1;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 0;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameter symbol for writeLine(String str) */
            param_offset = semantic_add_string(analyzer, "str");
            if (param_offset == 0xFFFF) {
                return -1;
            }
            
            string_class_offset = semantic_add_string(analyzer, "String");
            if (string_class_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&param_sym, 0, sizeof(Symbol));
            param_sym.kind = SYM_PARAM;
            param_sym.name_offset = param_offset;
            param_sym.type.kind = TYPE_CLASS;
            param_sym.type.class_name = string_class_offset;
            param_sym.data.param_data.index = 0;
            
            if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                return -1;
            }
        }
        
        if (strcmp(builtin_classes[i], "FileOutputStream") == 0) {
            /* Add write(int) method for FileOutputStream */
            Symbol param_sym;
            uint16_t param_offset;
            
            method_offset = semantic_add_string(analyzer, "write");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_VOID;
            method_sym.data.method_data.param_count = 1;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 0;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameter symbol for write(int b) */
            param_offset = semantic_add_string(analyzer, "b");
            if (param_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&param_sym, 0, sizeof(Symbol));
            param_sym.kind = SYM_PARAM;
            param_sym.name_offset = param_offset;
            param_sym.type.kind = TYPE_INT;
            param_sym.data.param_data.index = 0;
            
            if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                return -1;
            }
        }
        
        if (strcmp(builtin_classes[i], "BufferedWriter") == 0) {
            /* Add newLine() method for BufferedWriter */
            method_offset = semantic_add_string(analyzer, "newLine");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_VOID;
            method_sym.data.method_data.param_count = 0;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 0;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
        }
        
        if (strcmp(builtin_classes[i], "BufferedWriter") == 0 ||
            strcmp(builtin_classes[i], "BufferedReader") == 0 ||
            strcmp(builtin_classes[i], "FileOutputStream") == 0 ||
            strcmp(builtin_classes[i], "FileInputStream") == 0) {
            /* Add close() method */
            method_offset = semantic_add_string(analyzer, "close");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_VOID;
            method_sym.data.method_data.param_count = 0;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 0;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
        }
        
        if (strcmp(builtin_classes[i], "BufferedReader") == 0 ||
            strcmp(builtin_classes[i], "FileInputStream") == 0) {
            /* Add read() method */
            method_offset = semantic_add_string(analyzer, "read");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_INT;
            method_sym.data.method_data.param_count = 0;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 0;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add readLine() method for BufferedReader */
            if (strcmp(builtin_classes[i], "BufferedReader") == 0) {
                uint16_t string_class_offset;
                
                method_offset = semantic_add_string(analyzer, "readLine");
                if (method_offset == 0xFFFF) {
                    return -1;
                }
                
                /* Get String class name offset */
                string_class_offset = semantic_add_string(analyzer, "String");
                if (string_class_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&method_sym, 0, sizeof(Symbol));
                method_sym.kind = SYM_METHOD;
                method_sym.name_offset = method_offset;
                method_sym.type.kind = TYPE_CLASS;
                method_sym.type.class_name = string_class_offset;
                method_sym.data.method_data.param_count = 0;
                method_sym.data.method_data.local_count = 0;
                method_sym.data.method_data.is_static = 0;
                method_sym.data.method_data.is_public = 1;
                
                if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                    return -1;
                }
            }
        }
        
        /* Add Socket class methods */
        if (strcmp(builtin_classes[i], "Socket") == 0) {
            /* Add init() static method */
            method_offset = semantic_add_string(analyzer, "init");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_VOID;
            method_sym.data.method_data.param_count = 0;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 1;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add create(String, int) static method - returns int (socket handle) */
            method_offset = semantic_add_string(analyzer, "create");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_INT;
            method_sym.data.method_data.param_count = 2;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 1;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameters for create(String host, int port) */
            {
                Symbol param_sym;
                uint16_t param_offset, string_class_offset;
                
                /* Parameter 0: String host */
                param_offset = semantic_add_string(analyzer, "host");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                string_class_offset = semantic_add_string(analyzer, "String");
                if (string_class_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_CLASS;
                param_sym.type.class_name = string_class_offset;
                param_sym.data.param_data.index = 0;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
                
                /* Parameter 1: int port */
                param_offset = semantic_add_string(analyzer, "port");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_INT;
                param_sym.data.param_data.index = 1;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
            }
            
            /* Add close(Socket) static method */
            method_offset = semantic_add_string(analyzer, "close");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_VOID;
            method_sym.data.method_data.param_count = 1;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 1;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameter for close(int sock) */
            {
                Symbol param_sym;
                uint16_t param_offset;
                
                param_offset = semantic_add_string(analyzer, "sock");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_INT;
                param_sym.data.param_data.index = 0;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
            }
            
            /* Add send(Socket, String) static method - returns int */
            method_offset = semantic_add_string(analyzer, "send");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_INT;
            method_sym.data.method_data.param_count = 2;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 1;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameters for send(int sock, String data) */
            {
                Symbol param_sym;
                uint16_t param_offset, string_class_offset;
                
                /* Parameter 0: int sock */
                param_offset = semantic_add_string(analyzer, "sock");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_INT;
                param_sym.data.param_data.index = 0;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
                
                /* Parameter 1: String data */
                param_offset = semantic_add_string(analyzer, "data");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                string_class_offset = semantic_add_string(analyzer, "String");
                if (string_class_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_CLASS;
                param_sym.type.class_name = string_class_offset;
                param_sym.data.param_data.index = 1;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
            }
            
            /* Add recv(Socket, int) static method - returns String */
            method_offset = semantic_add_string(analyzer, "recv");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_CLASS;
            method_sym.type.class_name = semantic_add_string(analyzer, "String");
            method_sym.data.method_data.param_count = 2;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 1;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameters for recv(int sock, int maxlen) */
            {
                Symbol param_sym;
                uint16_t param_offset;
                
                /* Parameter 0: int sock */
                param_offset = semantic_add_string(analyzer, "sock");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_INT;
                param_sym.data.param_data.index = 0;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
                
                /* Parameter 1: int maxlen */
                param_offset = semantic_add_string(analyzer, "maxlen");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_INT;
                param_sym.data.param_data.index = 1;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
            }
            
            /* Add isConnected(Socket) static method - returns int (boolean) */
            method_offset = semantic_add_string(analyzer, "isConnected");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_INT;
            method_sym.data.method_data.param_count = 1;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 1;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameter for isConnected(int sock) */
            {
                Symbol param_sym;
                uint16_t param_offset;
                
                param_offset = semantic_add_string(analyzer, "sock");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_INT;
                param_sym.data.param_data.index = 0;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
            }
        }
        
        /* Add System class methods */
        if (strcmp(builtin_classes[i], "System") == 0) {
            /* Add println(String) static method */
            method_offset = semantic_add_string(analyzer, "println");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_VOID;
            method_sym.data.method_data.param_count = 1;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 1;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameter for println(String message) */
            {
                Symbol param_sym;
                uint16_t param_offset, string_class_offset;
                
                param_offset = semantic_add_string(analyzer, "message");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                string_class_offset = semantic_add_string(analyzer, "String");
                if (string_class_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_CLASS;
                param_sym.type.class_name = string_class_offset;
                param_sym.data.param_data.index = 0;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
            }
            
            /* Add print(String) static method */
            method_offset = semantic_add_string(analyzer, "print");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_VOID;
            method_sym.data.method_data.param_count = 1;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 1;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameter for print(String message) */
            {
                Symbol param_sym;
                uint16_t param_offset, string_class_offset;
                
                param_offset = semantic_add_string(analyzer, "message");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                string_class_offset = semantic_add_string(analyzer, "String");
                if (string_class_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_CLASS;
                param_sym.type.class_name = string_class_offset;
                param_sym.data.param_data.index = 0;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
            }
            
            /* Add printInt(int) static method */
            method_offset = semantic_add_string(analyzer, "printInt");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_VOID;
            method_sym.data.method_data.param_count = 1;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 1;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameter for printInt(int value) */
            {
                Symbol param_sym;
                uint16_t param_offset;
                
                param_offset = semantic_add_string(analyzer, "value");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_INT;
                param_sym.data.param_data.index = 0;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
            }
            
            /* Add printLong(long) static method */
            method_offset = semantic_add_string(analyzer, "printLong");
            if (method_offset == 0xFFFF) {
                return -1;
            }
            
            memset(&method_sym, 0, sizeof(Symbol));
            method_sym.kind = SYM_METHOD;
            method_sym.name_offset = method_offset;
            method_sym.type.kind = TYPE_VOID;
            method_sym.data.method_data.param_count = 1;
            method_sym.data.method_data.local_count = 0;
            method_sym.data.method_data.is_static = 1;
            method_sym.data.method_data.is_public = 1;
            
            if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
                return -1;
            }
            
            /* Add parameter for printLong(long value) */
            {
                Symbol param_sym;
                uint16_t param_offset;
                
                param_offset = semantic_add_string(analyzer, "value");
                if (param_offset == 0xFFFF) {
                    return -1;
                }
                
                memset(&param_sym, 0, sizeof(Symbol));
                param_sym.kind = SYM_PARAM;
                param_sym.name_offset = param_offset;
                param_sym.type.kind = TYPE_LONG;
                param_sym.data.param_data.index = 0;
                
                if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
                    return -1;
                }
            }
        }
    }
    
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
    
    /* AST file layout:
     *   [uint16 total_nodes][uint16 pool_size][string_pool][AST nodes...]
     *
     * The old offset math used analyzer->pool_size directly. That becomes wrong
     * after semantic_add_string() grows the in-memory pool during analysis
     * (for example when adding "String"), causing later node reads to seek into
     * the wrong location and produce impossible line/column values like 2304/13312.
     *
     * read_ast_header()/load_string_pool() leave the file position exactly at the
     * first AST node, so anchor all node reads to that stable on-disk offset.
     */
    file_pos = ftell(analyzer->ast_file);
    if (file_pos < 0) {
        return NULL;
    }
    if (analyzer->node_count == 0) {
        analyzer->node_count = (uint16_t)file_pos;
    }
    
    file_pos = (long)analyzer->node_count + (long)(node_index - 1) * (long)sizeof(ASTNode);
    fseek(analyzer->ast_file, file_pos, SEEK_SET);
    
    if (fread(&analyzer->nodes[0], sizeof(ASTNode), 1, analyzer->ast_file) != 1) {
        return NULL;
    }
    
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

static int current_scope_has_local_name(SemanticAnalyzer* analyzer, const char* name, SymbolKind kind) {
    uint16_t i;
    uint16_t scope_start;
    
    if (!analyzer || !analyzer->symtable || !name) {
        return 0;
    }
    
    scope_start = analyzer->symtable->scope_stack[analyzer->symtable->scope_level];
    
    for (i = analyzer->symtable->symbol_count; i > scope_start; i--) {
        Symbol* sym = &analyzer->symtable->symbols[i - 1];
        const char* sym_name;
        
        if (sym->scope_level != analyzer->symtable->scope_level) {
            continue;
        }
        if (sym->kind != kind) {
            continue;
        }
        
        sym_name = symtable_get_string(analyzer->symtable, sym->name_offset);
        if (sym_name && strcmp(sym_name, name) == 0) {
            return 1;
        }
    }
    
    return 0;
}

static int is_string_type(SemanticAnalyzer* analyzer, TypeInfo type) {
    const char* class_name;
    
    if (!analyzer || type.kind != TYPE_CLASS) {
        return 0;
    }
    
    class_name = semantic_get_string(analyzer, type.class_name);
    if (!class_name && analyzer->symtable) {
        class_name = symtable_get_string(analyzer->symtable, type.class_name);
    }
    return class_name && strcmp(class_name, "String") == 0;
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
    uint16_t class_idx;
    
    if (!analyzer) {
        return -1;
    }
    
    /* Get root node (parser writes NODE_PROGRAM last) */
    root = semantic_get_node(analyzer, analyzer->total_nodes);
    if (!root || root->type != NODE_PROGRAM) {
        semantic_error(analyzer, 0, 0, "Invalid AST: root node is not NODE_PROGRAM");
        return -1;
    }
    
    /* Save class index before next semantic_get_node overwrites root buffer */
    class_idx = root->data.program.class_node;
    
    /* Get class node */
    class_node = semantic_get_node(analyzer, class_idx);
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
    uint16_t class_name_off;
    uint16_t class_member_count;
    uint16_t class_first_member;
    
    if (!analyzer || !class_node) {
        return -1;
    }
    
    /* Get class name */
    class_name_off = class_node->data.class_decl.name;
    class_member_count = class_node->data.class_decl.member_count;
    class_first_member = class_node->data.class_decl.first_member;
    class_name = semantic_get_string(analyzer, class_name_off);
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
    class_sym.name_offset = class_name_off;
    class_sym.type.kind = TYPE_CLASS;
    class_sym.type.class_name = class_name_off;
    class_sym.data.class_data.member_count = class_member_count;
    
    /* Add class to symbol table */
    if (symtable_add_symbol(analyzer->symtable, &class_sym) == 0xFFFF) {
        semantic_error_node(analyzer, class_node, "Failed to add class symbol");
        return -1;
    }
    
    analyzer->current_class = symtable_lookup(analyzer->symtable, class_name);
    
    /* Enter class scope */
    symtable_enter_scope(analyzer->symtable);
    
    /* Collect member symbols */
    member_idx = class_first_member;
    member_count = 0;
    
    while (member_idx != 0 && member_count < class_member_count) {
        uint16_t next_member_idx;
        uint16_t member_type;
        member_node = semantic_get_node(analyzer, member_idx);
        if (!member_node) {
            break;
        }
        
        /* Save fields before any function calls that might invalidate the pointer */
        member_type = member_node->type;
        next_member_idx = member_node->next_sibling;
        
        if (member_type == NODE_METHOD) {
            if (collect_method_symbols(analyzer, member_node) != 0) {
                return -1;
            }
        } else if (member_type == NODE_FIELD) {
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
    uint16_t method_name_off;
    TypeInfo method_return_type;
    uint16_t method_param_count;
    uint16_t method_is_static;
    uint16_t method_is_public;
    uint16_t param_idx;
    ASTNode* param_node;
    uint16_t param_count;
    uint16_t method_line;
    uint16_t method_column;
    
    if (!analyzer || !method_node) {
        return -1;
    }
    
    method_name_off = method_node->data.method.name;
    method_return_type = method_node->data.method.return_type;
    method_param_count = method_node->data.method.param_count;
    method_is_static = method_node->data.method.is_static;
    method_is_public = method_node->data.method.is_public;
    param_idx = method_node->data.method.first_param;
    method_line = method_node->line;
    method_column = method_node->column;
    
    /* Get method name */
    method_name = semantic_get_string(analyzer, method_name_off);
    if (!method_name) {
        semantic_error(analyzer, method_line, method_column, "Invalid method name");
        return -1;
    }
    /* Check for duplicate method */
    if (symtable_exists_in_current_scope(analyzer->symtable, method_name)) {
        semantic_error(analyzer, method_line, method_column, "Duplicate method declaration");
        return -1;
    }
    
    /* Create method symbol */
    memset(&method_sym, 0, sizeof(Symbol));
    method_sym.kind = SYM_METHOD;
    method_sym.name_offset = method_name_off;
    method_sym.type = method_return_type;
    method_sym.data.method_data.param_count = method_param_count;
    method_sym.data.method_data.is_static = method_is_static;
    method_sym.data.method_data.is_public = method_is_public;
    
    /* Add method to symbol table */
    if (symtable_add_symbol(analyzer->symtable, &method_sym) == 0xFFFF) {
        semantic_error(analyzer, method_line, method_column, "Failed to add method symbol");
        return -1;
    }
    
    /* Do not enter/exit a separate parameter scope during declaration collection.
     * Because exited symbols are preserved for codegen, reusing the same numeric
     * scope level for later methods makes `symtable_exists_in_current_scope()`
     * see parameters from previous methods as duplicates.
     *
     * Keep parameters at the class scope level during Pass1, and let Pass2
     * create the active method scope used for identifier resolution.
     */
    param_count = 0;
    
    while (param_idx != 0 && param_count < method_param_count) {
        Symbol param_sym;
        const char* param_name;
        uint16_t next_param_idx;
        uint16_t param_name_off;
        TypeInfo param_type;
        uint16_t param_line;
        uint16_t param_column;
        
        param_node = semantic_get_node(analyzer, param_idx);
        if (!param_node || param_node->type != NODE_PARAM) {
            break;
        }
        
        next_param_idx = param_node->next_sibling;
        param_name_off = param_node->data.param.name;
        param_type = param_node->data.param.type;
        param_line = param_node->line;
        param_column = param_node->column;
        
        param_name = semantic_get_string(analyzer, param_name_off);
        if (!param_name) {
            semantic_error(analyzer, param_line, param_column, "Invalid parameter name");
            return -1;
        }
        
        /* Only compare against parameters already collected for this method.
         * Symbols from previous methods remain preserved in the same class scope,
         * so scanning the whole current scope will produce false duplicates.
         * At this point, only the most recently added contiguous SYM_PARAM entries
         * belong to the current method.
         */
        {
            uint16_t i = analyzer->symtable->symbol_count;
            while (i > 0) {
                Symbol* existing = &analyzer->symtable->symbols[i - 1];
                const char* existing_name;
                
                if (existing->scope_level != analyzer->symtable->scope_level) {
                    break;
                }
                if (existing->kind != SYM_PARAM) {
                    break;
                }
                
                existing_name = symtable_get_string(analyzer->symtable, existing->name_offset);
                if (existing_name && strcmp(existing_name, param_name) == 0) {
                    semantic_error(analyzer, param_line, param_column, "Duplicate parameter name");
                    return -1;
                }
                i--;
            }
        }
        
        memset(&param_sym, 0, sizeof(Symbol));
        param_sym.kind = SYM_PARAM;
        param_sym.name_offset = param_name_off;
        param_sym.type = param_type;
        param_sym.data.param_data.index = param_count;
        
        if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
            semantic_error(analyzer, param_line, param_column, "Failed to add parameter symbol");
            return -1;
        }
        
        param_idx = next_param_idx;
        param_count++;
    }
    
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
    uint16_t class_idx;
    uint16_t member_idx;
    ASTNode* member_node;
    uint16_t member_count;
    uint16_t expected_member_count;
    
    if (!analyzer) {
        return -1;
    }
    
    /* Get root node and save class index before next node read overwrites buffer */
    root = semantic_get_node(analyzer, analyzer->total_nodes);
    if (!root) {
        return -1;
    }
    class_idx = root->data.program.class_node;
    
    class_node = semantic_get_node(analyzer, class_idx);
    if (!class_node) {
        return -1;
    }
    
    /* Save member traversal data before later semantic_get_node calls overwrite class_node */
    member_idx = class_node->data.class_decl.first_member;
    expected_member_count = class_node->data.class_decl.member_count;
    
    /* Enter class scope */
    symtable_enter_scope(analyzer->symtable);
    
    /* Check each method body */
    member_count = 0;
    
    while (member_idx != 0 && member_count < expected_member_count) {
        uint16_t next_member_idx;
        member_node = semantic_get_node(analyzer, member_idx);
        if (!member_node) {
            break;
        }
        
        /* Save next_sibling before any function calls */
        next_member_idx = member_node->next_sibling;
        
        {
            uint16_t member_type = member_node->type;
            
            if (member_type == NODE_METHOD) {
                if (check_method_body(analyzer, member_node) != 0) {
                    /* Continue checking other methods */
                }
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
    uint16_t method_name_off;
    TypeInfo return_type;
    uint16_t body_idx;
    uint16_t method_param_count;
    uint16_t param_idx;
    ASTNode* param_node;
    uint16_t param_count;
    uint16_t method_line;
    uint16_t method_column;
    
    if (!analyzer || !method_node) {
        return -1;
    }
    
    /* Save method data before semantic_get_node calls overwrite method_node buffer */
    method_name_off = method_node->data.method.name;
    return_type = method_node->data.method.return_type;
    body_idx = method_node->data.method.body;
    method_param_count = method_node->data.method.param_count;
    param_idx = method_node->data.method.first_param;
    method_line = method_node->line;
    method_column = method_node->column;
    
    /* Get method symbol */
    method_name = semantic_get_string(analyzer, method_name_off);
    analyzer->current_method = symtable_lookup(analyzer->symtable, method_name);
    analyzer->expected_return = return_type;
    analyzer->has_return = 0;
    analyzer->next_local_index = 0;
    
    /* Enter method scope */
    symtable_enter_scope(analyzer->symtable);
    
    /* Re-add parameters to scope only when not already present.
     * Pass1 keeps parameter symbols for later codegen, so blindly re-adding
     * them in Pass2 can trigger duplicate-parameter failures depending on
     * scope reuse/preservation details.
     */
    param_count = 0;
    
    while (param_idx != 0 && param_count < method_param_count) {
        Symbol param_sym;
        uint16_t next_param_idx;
        const char* param_name;
        uint16_t param_name_off;
        TypeInfo param_type;
        uint16_t param_line;
        uint16_t param_column;
        
        param_node = semantic_get_node(analyzer, param_idx);
        if (!param_node) {
            break;
        }
        
        next_param_idx = param_node->next_sibling;
        param_name_off = param_node->data.param.name;
        param_type = param_node->data.param.type;
        param_line = param_node->line;
        param_column = param_node->column;
        
        param_name = semantic_get_string(analyzer, param_name_off);
        if (!param_name) {
            semantic_error(analyzer, param_line, param_column, "Invalid parameter name");
            break;
        }
        
        /* Always add active parameter symbols for the current method scope.
         * Pass1 preserved parameter metadata at class scope for codegen, but
         * those are not visible inside the nested method/body scopes used in
         * Pass2. Skipping this re-add caused:
         *   - method arguments to resolve against stale metadata
         *   - loop/body references like n/str to become undefined
         *   - String return/compound-assignment checks to fail downstream
         */
        memset(&param_sym, 0, sizeof(Symbol));
        param_sym.kind = SYM_PARAM;
        param_sym.name_offset = param_name_off;
        param_sym.type = param_type;
        param_sym.data.param_data.index = param_count;
        
        if (symtable_add_symbol(analyzer->symtable, &param_sym) == 0xFFFF) {
            semantic_error(analyzer, param_line, param_column, "Failed to add parameter symbol");
            break;
        }
        
        param_idx = next_param_idx;
        param_count++;
    }
    
    /* Check method body */
    body_node = semantic_get_node(analyzer, body_idx);
    if (body_node) {
        check_block(analyzer, body_node);
    }
    
    /* Check return statement requirement */
    if (!is_void_type(analyzer->expected_return) && !analyzer->has_return) {
        semantic_error(analyzer, method_line, method_column, "Missing return statement in non-void method");
    }
    
    /* Exit method scope */
    symtable_exit_scope(analyzer->symtable);
    
    analyzer->current_method = NULL;
    
    return 0;
}

/* Check statement */
int check_statement(SemanticAnalyzer* analyzer, ASTNode* stmt_node, uint16_t stmt_idx) {
    uint16_t stmt_type;
    uint16_t expr_idx;
    
    if (!analyzer || !stmt_node) {
        return -1;
    }
    
    /* CRITICAL: semantic_get_node() uses a shared buffer. Save dispatch data
     * before calling helpers that recursively load other nodes.
     */
    stmt_type = stmt_node->type;
    expr_idx = 0;
    if (stmt_type == NODE_EXPR_STMT) {
        expr_idx = stmt_node->data.expr_stmt.expr;
    }
    
    switch (stmt_type) {
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
            ASTNode* expr_node;
            
            expr_node = semantic_get_node(analyzer, expr_idx);
            if (expr_node) {
                return check_expression(analyzer, expr_node, &expr_type);
            }
            return 0;
        }
        
        case NODE_TRY:
            /* TODO: Implement try-catch-finally semantic check */
            /* For now, just accept it without checking */
            return 0;
        
        case NODE_THROW:
            /* TODO: Implement throw semantic check */
            /* For now, just accept it without checking */
            return 0;
        
        case NODE_SWITCH:
            return check_switch_stmt(analyzer, stmt_node);
        
        case NODE_BREAK:
            /* Check if break is inside switch or loop */
            if (!analyzer->in_switch && !analyzer->in_loop) {
                semantic_error_node(analyzer, stmt_node, "break statement not in switch or loop");
                return -1;
            }
            return 0;
        
        case NODE_CONTINUE:
            /* Check if continue is inside loop (not switch) */
            if (!analyzer->in_loop) {
                semantic_error_node(analyzer, stmt_node, "continue statement not in loop");
                return -1;
            }
            return 0;
        
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
    uint16_t var_name_off;
    uint16_t err_line;
    uint16_t err_col;
    
    if (!analyzer || !var_node) {
        return -1;
    }
    
    /* Save all fields needed after recursive reads because semantic_get_node()
     * reuses a shared AST buffer.
     */
    var_type = var_node->data.var_decl.type;
    init_expr_index = var_node->data.var_decl.init_expr;
    var_name_off = var_node->data.var_decl.name;
    err_line = var_node->line;
    err_col = var_node->column;
    
    /* Get variable name */
    var_name = semantic_get_string(analyzer, var_name_off);
    if (!var_name) {
        semantic_error(analyzer, err_line, err_col, "Invalid variable name");
        return -1;
    }
    
    /* Check for duplicate local variable in current active scope only */
    if (current_scope_has_local_name(analyzer, var_name, SYM_LOCAL)) {
        semantic_error(analyzer, err_line, err_col, "Duplicate variable declaration");
        return -1;
    }
    
    /* Check initializer if present */
    if (init_expr_index != 0) {
        init_expr = semantic_get_node(analyzer, init_expr_index);
        if (init_expr) {
            if (check_expression(analyzer, init_expr, &init_type) == 0) {
                /* Check type compatibility */
                if (!types_compatible(var_type, init_type)) {
                    if (!(is_string_type(analyzer, var_type) && is_string_type(analyzer, init_type))) {
                        semantic_error(analyzer, err_line, err_col, "Type mismatch in variable initialization");
                    }
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
        semantic_error(analyzer, err_line, err_col, "String pool full");
        return -1;
    }
    var_sym.type = var_type;  /* Use saved copy */
    var_sym.data.local_data.index = analyzer->next_local_index;
    
    /* Long and float type variables occupy 2 slots */
    if (var_type.kind == TYPE_LONG || var_type.kind == TYPE_FLOAT) {
        analyzer->next_local_index += 2;
    } else {
        analyzer->next_local_index++;
    }
    
    /* Add variable to symbol table */
    if (symtable_add_symbol(analyzer->symtable, &var_sym) == 0xFFFF) {
        semantic_error(analyzer, err_line, err_col, "Failed to add variable symbol");
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
    
    /* Check body - set loop context */
    body_node = semantic_get_node(analyzer, body_idx);
    if (body_node) {
        int saved_in_loop = analyzer->in_loop;
        analyzer->in_loop = 1;
        check_statement(analyzer, body_node, body_idx);
        analyzer->in_loop = saved_in_loop;
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
    
    /* The for-loop init declaration must remain visible to condition, update,
     * and body, so analyze the whole loop inside a dedicated scope.
     */
    symtable_enter_scope(analyzer->symtable);
    
    /* Check init (optional) */
    if (init_idx != 0) {
        init_node = semantic_get_node(analyzer, init_idx);
        if (init_node) {
            if (init_node->type == NODE_VAR_DECL) {
                check_var_decl(analyzer, init_node);
            } else {
                check_expression(analyzer, init_node, &init_type);
            }
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
    
    /* Check body - set loop context */
    body_node = semantic_get_node(analyzer, body_idx);
    if (body_node) {
        int saved_in_loop = analyzer->in_loop;
        analyzer->in_loop = 1;
        check_statement(analyzer, body_node, body_idx);
        analyzer->in_loop = saved_in_loop;
    }
    
    symtable_exit_scope(analyzer->symtable);
    
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
    uint16_t expr_type;
    uint16_t next_sibling_idx;
    
    if (!analyzer || !expr_node || !result_type) {
        return -1;
    }
    
    /* CRITICAL: semantic_get_node() reuses a single shared buffer.
     * Save dispatch-critical fields before any recursive reads.
     */
    expr_type = expr_node->type;
    next_sibling_idx = expr_node->next_sibling;
    
    switch (expr_type) {
        case NODE_LITERAL_INT:
            result_type->kind = TYPE_INT;
            result_type->class_name = 0;
            return 0;
        
        case NODE_LITERAL_LONG:
            result_type->kind = TYPE_LONG;
            result_type->class_name = 0;
            return 0;
        
        case NODE_LITERAL_FLOAT:
            result_type->kind = TYPE_FLOAT;
            result_type->class_name = 0;
            return 0;
        
        case NODE_LITERAL_BOOL:
            result_type->kind = TYPE_BOOLEAN;
            result_type->class_name = 0;
            return 0;
        
        case NODE_LITERAL_STRING:
            result_type->kind = TYPE_CLASS;
            result_type->class_name = semantic_add_string(analyzer, "String");
            return 0;
        
        case NODE_THIS: {
            /* 'this' keyword - only valid in instance methods (explicit this support) */
            if (!analyzer->current_method || analyzer->current_method->data.method_data.is_static) {
                semantic_error_node(analyzer, expr_node, "'this' cannot be used in static context");
                return -1;
            }
            
            /* Return the type of the current class */
            if (!analyzer->current_class) {
                semantic_error_node(analyzer, expr_node, "'this' used outside of class context");
                return -1;
            }
            
            result_type->kind = TYPE_CLASS;
            result_type->class_name = analyzer->current_class->name_offset;
            return 0;
        }
        
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
        
        case NODE_CALL:
            return check_call(analyzer, expr_node, result_type);
        
        case NODE_NEW: {
            uint16_t class_name_value;
            const char* class_name;
            Symbol* class_sym;
            
            /* Save class_name value before any semantic_get_node calls */
            class_name_value = expr_node->data.new_expr.class_name;
            
            /* Initialize to avoid warnings */
            class_name = NULL;
            class_sym = NULL;
            
            /* Check if it's an array (class_name is TYPE_INT, TYPE_LONG, TYPE_FLOAT, or TYPE_BOOLEAN) */
            if (class_name_value == TYPE_INT || class_name_value == TYPE_LONG ||
                class_name_value == TYPE_FLOAT || class_name_value == TYPE_BOOLEAN) {
                /* Array creation: new int[size], new long[size], new float[size], or new boolean[size] */
                ASTNode* size_node;
                TypeInfo size_type;
                
                size_node = semantic_get_node(analyzer, next_sibling_idx);
                if (!size_node || check_expression(analyzer, size_node, &size_type) != 0) {
                    return -1;
                }
                if (!is_numeric_type(size_type)) {
                    semantic_error_node(analyzer, expr_node, "Array size must be integer");
                    return -1;
                }
                
                result_type->kind = TYPE_ARRAY;
                result_type->class_name = class_name_value;  /* Store element type */
                return 0;
            }
            
            /* Object creation: new ClassName() */
            class_name = semantic_get_string(analyzer, class_name_value);
            if (!class_name) {
                semantic_error_node(analyzer, expr_node, "Invalid class name in new expression");
                return -1;
            }
            
            /* Lookup class symbol */
            class_sym = symtable_lookup(analyzer->symtable, class_name);
            if (!class_sym || class_sym->kind != SYM_CLASS) {
                semantic_error_node(analyzer, expr_node, "Undefined class in new expression");
                return -1;
            }
            
            /* Return class type */
            result_type->kind = TYPE_CLASS;
            result_type->class_name = class_name_value;
            return 0;
        }
        
        case NODE_ARRAY_ACCESS: {
            ASTNode* array_node;
            ASTNode* index_node;
            TypeInfo array_type;
            TypeInfo index_type;
            uint16_t array_idx;
            uint16_t index_idx;
            
            array_idx = expr_node->data.array_access.array;
            index_idx = expr_node->data.array_access.index;
            
            array_node = semantic_get_node(analyzer, array_idx);
            if (!array_node || check_expression(analyzer, array_node, &array_type) != 0) {
                return -1;
            }
            if (array_type.kind != TYPE_ARRAY) {
                semantic_error_node(analyzer, expr_node, "Array access requires array type");
                return -1;
            }
            
            index_node = semantic_get_node(analyzer, index_idx);
            if (!index_node || check_expression(analyzer, index_node, &index_type) != 0) {
                return -1;
            }
            if (!is_numeric_type(index_type)) {
                semantic_error_node(analyzer, expr_node, "Array index must be integer");
                return -1;
            }
            
            /* Return element type from array's class_name field */
            if (array_type.class_name == TYPE_LONG) {
                result_type->kind = TYPE_LONG;
            } else if (array_type.class_name == TYPE_FLOAT) {
                result_type->kind = TYPE_FLOAT;
            } else if (array_type.class_name == TYPE_BOOLEAN) {
                result_type->kind = TYPE_BOOLEAN;
            } else {
                result_type->kind = TYPE_INT;
            }
            result_type->class_name = 0;
            return 0;
        }
        
        case NODE_FIELD_ACCESS: {
            ASTNode* object_node;
            TypeInfo object_type;
            const char* field_name;
            const char* class_name;
            Symbol* class_sym;
            Symbol* field_sym;
            uint16_t object_idx;
            uint16_t field_name_off;
            uint16_t i;
            
            object_idx = expr_node->data.field_access.object;
            field_name_off = expr_node->data.field_access.field_name;
            
            object_node = semantic_get_node(analyzer, object_idx);
            if (!object_node || check_expression(analyzer, object_node, &object_type) != 0) {
                return -1;
            }
            
            field_name = semantic_get_string(analyzer, field_name_off);
            if (!field_name) {
                semantic_error_node(analyzer, expr_node, "Invalid field name");
                return -1;
            }
            
            /* Array.length special case */
            if (object_type.kind == TYPE_ARRAY && strcmp(field_name, "length") == 0) {
                result_type->kind = TYPE_INT;
                result_type->class_name = 0;
                return 0;
            }
            
            /* Object field access */
            if (object_type.kind == TYPE_CLASS) {
                class_name = semantic_get_string(analyzer, object_type.class_name);
                if (!class_name) {
                    class_name = symtable_get_string(analyzer->symtable, object_type.class_name);
                }
                if (!class_name) {
                    semantic_error_node(analyzer, expr_node, "Invalid class name in field access");
                    return -1;
                }
                
                /* Lookup class symbol */
                class_sym = symtable_lookup(analyzer->symtable, class_name);
                if (!class_sym || class_sym->kind != SYM_CLASS) {
                    semantic_error_node(analyzer, expr_node, "Undefined class in field access");
                    return -1;
                }
                
                /* Find field in class scope */
                field_sym = NULL;
                for (i = 0; i < analyzer->symtable->symbol_count; i++) {
                    Symbol* sym = &analyzer->symtable->symbols[i];
                    const char* sym_name;
                    
                    if (sym->kind != SYM_FIELD) {
                        continue;
                    }
                    
                    sym_name = symtable_get_string(analyzer->symtable, sym->name_offset);
                    if (sym_name && strcmp(sym_name, field_name) == 0) {
                        field_sym = sym;
                        break;
                    }
                }
                
                if (!field_sym) {
                    semantic_error_node(analyzer, expr_node, "Undefined field");
                    return -1;
                }
                
                *result_type = field_sym->type;
                return 0;
            }
            
            semantic_error_node(analyzer, expr_node, "Field access requires object or array");
            return -1;
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
    uint16_t err_line;
    uint16_t err_col;
    
    if (!analyzer || !binop_node || !result_type) {
        return -1;
    }
    
    /* CRITICAL: Save indices/operator/source location BEFORE any semantic_get_node calls */
    left_idx = binop_node->data.binary_op.left;
    right_idx = binop_node->data.binary_op.right;
    op = binop_node->data.binary_op.op;
    err_line = binop_node->line;
    err_col = binop_node->column;
    
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
    
    /* String concatenation: String + String, String + int, int + String */
    if (op == BINOP_ADD) {
        /* String + String */
        if (is_string_type(analyzer, left_type) && is_string_type(analyzer, right_type)) {
            result_type->kind = TYPE_CLASS;
            result_type->class_name = semantic_add_string(analyzer, "String");
            return 0;
        }
        
        /* String + int */
        if (is_string_type(analyzer, left_type) && is_numeric_type(right_type)) {
            result_type->kind = TYPE_CLASS;
            result_type->class_name = semantic_add_string(analyzer, "String");
            return 0;
        }
        
        /* int + String */
        if (is_numeric_type(left_type) && is_string_type(analyzer, right_type)) {
            result_type->kind = TYPE_CLASS;
            result_type->class_name = semantic_add_string(analyzer, "String");
            return 0;
        }
    }
    
    /* String comparison with null (represented as 0): String == 0, String != 0 */
    if (op == BINOP_EQ || op == BINOP_NE) {
        /* String == 0 or String != 0 */
        if (is_string_type(analyzer, left_type) && is_numeric_type(right_type)) {
            result_type->kind = TYPE_BOOLEAN;
            result_type->class_name = 0;
            return 0;
        }
        
        /* 0 == String or 0 != String */
        if (is_numeric_type(left_type) && is_string_type(analyzer, right_type)) {
            result_type->kind = TYPE_BOOLEAN;
            result_type->class_name = 0;
            return 0;
        }
    }
    
    /* Get result type */
    if (get_binary_op_result_type(op, left_type, right_type, result_type) != 0) {
        semantic_error(analyzer, err_line, err_col, "Invalid operand types for binary operation");
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
    uint16_t assign_op;
    uint16_t target_node_type;
    
    if (!analyzer || !assign_node || !result_type) {
        return -1;
    }
    
    /* CRITICAL: Save indices/operator BEFORE any semantic_get_node calls */
    assign_op = assign_node->data.assign.op;
    target_idx = assign_node->data.assign.target;
    value_idx = assign_node->data.assign.value;
    
    /* Check target */
    target_node = semantic_get_node(analyzer, target_idx);
    if (!target_node || check_expression(analyzer, target_node, &target_type) != 0) {
        return -1;
    }
    target_node_type = target_node->type;
    
    /* Check value */
    value_node = semantic_get_node(analyzer, value_idx);
    if (!value_node || check_expression(analyzer, value_node, &value_type) != 0) {
        return -1;
    }
    
    if (assign_op == 0) {
        /* Simple assignment */
        if (!types_compatible(target_type, value_type)) {
            semantic_error_node(analyzer, assign_node, "Type mismatch in assignment");
            return -1;
        }
        
        *result_type = target_type;
        return 0;
    }
    
    if (!(assign_op == 1 || assign_op == 2)) {
        semantic_error_node(analyzer, assign_node, "Unsupported assignment operator");
        return -1;
    }
    
    if (!(target_node_type == NODE_IDENTIFIER || target_node_type == NODE_ARRAY_ACCESS)) {
        semantic_error_node(analyzer, assign_node, "Compound assignment requires variable or array element target");
        return -1;
    }
    
    if (!types_compatible(target_type, value_type)) {
        semantic_error_node(analyzer, assign_node, "Type mismatch in compound assignment");
        return -1;
    }
    
    if (target_type.kind == TYPE_INT) {
        *result_type = target_type;
        return 0;
    }
    
    if (assign_op == 1 && is_string_type(analyzer, target_type) && is_string_type(analyzer, value_type)) {
        *result_type = target_type;
        return 0;
    }
    
    semantic_error_node(analyzer, assign_node, "Compound assignment currently supports int -=/+= and String +=");
    return -1;
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
    
    /* Built-in class names used by Phase 1 runtime support */
    if (strcmp(id_name, "System") == 0 || strcmp(id_name, "String") == 0) {
        result_type->kind = TYPE_CLASS;
        result_type->class_name = semantic_add_string(analyzer, id_name);
        return 0;
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

/* Check method call */
int check_call(SemanticAnalyzer* analyzer, ASTNode* call_node, TypeInfo* result_type) {
    ASTNode* object_node;
    ASTNode* arg_node;
    ASTNode* recv_object_node;
    uint16_t object_idx;
    uint16_t first_arg_idx;
    uint16_t method_name_off;
    uint16_t arg_count;
    const char* method_name;
    
    if (!analyzer || !call_node || !result_type) {
        return -1;
    }
    
    object_idx = call_node->data.call.object;
    first_arg_idx = call_node->data.call.first_arg;
    method_name_off = call_node->data.call.method_name;
    arg_count = call_node->data.call.arg_count;
    method_name = semantic_get_string(analyzer, method_name_off);
    
    /* Special-case built-in System.out.println(...) and System.out.print(...) */
    if (object_idx != 0 && method_name &&
        (strcmp(method_name, "println") == 0 || strcmp(method_name, "print") == 0)) {
        uint16_t field_name_off;
        uint16_t recv_object_idx;
        const char* field_name;
        
        object_node = semantic_get_node(analyzer, object_idx);
        if (!object_node) {
            return -1;
        }
        
        if (object_node->type == NODE_FIELD_ACCESS) {
            field_name_off = object_node->data.field_access.field_name;
            recv_object_idx = object_node->data.field_access.object;
            field_name = semantic_get_string(analyzer, field_name_off);
            recv_object_node = semantic_get_node(analyzer, recv_object_idx);
            
            if (field_name && strcmp(field_name, "out") == 0 &&
                recv_object_node && recv_object_node->type == NODE_IDENTIFIER) {
                uint16_t recv_name_off = recv_object_node->data.identifier.name;
                const char* recv_name = semantic_get_string(analyzer, recv_name_off);
                uint16_t checked_args = 0;
                uint16_t current_arg_idx = first_arg_idx;
                
                if (recv_name && strcmp(recv_name, "System") == 0) {
                    while (current_arg_idx != 0 && checked_args < arg_count) {
                        TypeInfo arg_type;
                        const char* arg_class_name = NULL;
                        uint16_t next_arg_idx;
                        
                        arg_node = semantic_get_node(analyzer, current_arg_idx);
                        if (!arg_node || check_expression(analyzer, arg_node, &arg_type) != 0) {
                            return -1;
                        }
                        
                        if (arg_type.kind == TYPE_CLASS && arg_type.class_name < analyzer->pool_size) {
                            arg_class_name = semantic_get_string(analyzer, arg_type.class_name);
                        }
                        
                        if (!(is_numeric_type(arg_type) ||
                              (arg_class_name && strcmp(arg_class_name, "String") == 0))) {
                            semantic_error_node(analyzer, call_node, "println supports only int or String in Phase 1");
                            return -1;
                        }
                        
                        next_arg_idx = arg_node->next_sibling;
                        current_arg_idx = next_arg_idx;
                        checked_args++;
                    }
                    
                    result_type->kind = TYPE_VOID;
                    result_type->class_name = 0;
                    return 0;
                }
            }
        }
    }
    
    /* Special-case Math static methods */
    if (object_idx != 0 && method_name) {
        object_node = semantic_get_node(analyzer, object_idx);
        if (object_node && object_node->type == NODE_IDENTIFIER) {
            uint16_t class_name_off = object_node->data.identifier.name;
            const char* class_name = semantic_get_string(analyzer, class_name_off);
            
            if (class_name && strcmp(class_name, "Math") == 0) {
                /* Math.abs(float), Math.sqrt(float), Math.sin(float), Math.cos(float), Math.tan(float), Math.exp(float), Math.log(float) */
                if (strcmp(method_name, "abs") == 0 || strcmp(method_name, "sqrt") == 0 ||
                    strcmp(method_name, "sin") == 0 || strcmp(method_name, "cos") == 0 ||
                    strcmp(method_name, "tan") == 0 || strcmp(method_name, "exp") == 0 ||
                    strcmp(method_name, "log") == 0) {
                    if (arg_count != 1) {
                        semantic_error_node(analyzer, call_node, "Math method takes exactly 1 argument");
                        return -1;
                    }
                    /* Check argument type */
                    if (first_arg_idx != 0) {
                        TypeInfo arg_type;
                        arg_node = semantic_get_node(analyzer, first_arg_idx);
                        if (!arg_node || check_expression(analyzer, arg_node, &arg_type) != 0) {
                            return -1;
                        }
                        if (arg_type.kind != TYPE_FLOAT) {
                            semantic_error_node(analyzer, call_node, "Math method requires float argument");
                            return -1;
                        }
                    }
                    result_type->kind = TYPE_FLOAT;
                    result_type->class_name = 0;
                    return 0;
                }
                /* Math.min(float, float), Math.max(float, float), Math.pow(float, float) */
                else if (strcmp(method_name, "min") == 0 || strcmp(method_name, "max") == 0 ||
                         strcmp(method_name, "pow") == 0) {
                    if (arg_count != 2) {
                        semantic_error_node(analyzer, call_node, "Math method takes exactly 2 arguments");
                        return -1;
                    }
                    /* Check argument types */
                    if (first_arg_idx != 0) {
                        TypeInfo arg_type;
                        uint16_t current_arg_idx = first_arg_idx;
                        int checked_args = 0;
                        
                        while (current_arg_idx != 0 && checked_args < 2) {
                            arg_node = semantic_get_node(analyzer, current_arg_idx);
                            if (!arg_node || check_expression(analyzer, arg_node, &arg_type) != 0) {
                                return -1;
                            }
                            if (arg_type.kind != TYPE_FLOAT) {
                                semantic_error_node(analyzer, call_node, "Math method requires float arguments");
                                return -1;
                            }
                            current_arg_idx = arg_node->next_sibling;
                            checked_args++;
                        }
                    }
                    result_type->kind = TYPE_FLOAT;
                    result_type->class_name = 0;
                    return 0;
                }
            }
        }
    }
    
    /* Special-case Phase 1 String native instance methods */
    if (object_idx != 0 && method_name &&
        (strcmp(method_name, "length") == 0 ||
         strcmp(method_name, "toUpperCase") == 0 ||
         strcmp(method_name, "toLowerCase") == 0 ||
         strcmp(method_name, "startsWith") == 0 ||
         strcmp(method_name, "endsWith") == 0 ||
         strcmp(method_name, "equals") == 0 ||
         strcmp(method_name, "compareTo") == 0 ||
         strcmp(method_name, "indexOf") == 0 ||
         strcmp(method_name, "lastIndexOf") == 0 ||
         strcmp(method_name, "substr") == 0)) {
        TypeInfo object_type;
        uint16_t string_name_off;
        int is_comparison_method;
        int is_equals_method;
        int is_compareto_method;
        int is_index_method;
        int is_substr_method;
        
        is_comparison_method = (strcmp(method_name, "startsWith") == 0 ||
                                strcmp(method_name, "endsWith") == 0);
        is_equals_method = (strcmp(method_name, "equals") == 0);
        is_compareto_method = (strcmp(method_name, "compareTo") == 0);
        is_index_method = (strcmp(method_name, "indexOf") == 0 ||
                           strcmp(method_name, "lastIndexOf") == 0);
        is_substr_method = (strcmp(method_name, "substr") == 0);
        
        if (strcmp(method_name, "length") == 0 ||
            strcmp(method_name, "toUpperCase") == 0 ||
            strcmp(method_name, "toLowerCase") == 0) {
            if (arg_count != 0) {
                if (strcmp(method_name, "length") == 0) {
                    semantic_error_node(analyzer, call_node, "length() takes no arguments");
                } else {
                    semantic_error_node(analyzer, call_node, "String case conversion takes no arguments");
                }
                return -1;
            }
        } else if (is_comparison_method || is_equals_method || is_compareto_method) {
            if (arg_count != 1) {
                if (is_equals_method) {
                    semantic_error_node(analyzer, call_node, "equals() requires 1 argument");
                } else if (is_compareto_method) {
                    semantic_error_node(analyzer, call_node, "compareTo() requires 1 argument");
                } else {
                    semantic_error_node(analyzer, call_node, "startsWith/endsWith requires 1 argument");
                }
                return -1;
            }
        } else if (is_index_method) {
            if (arg_count != 1 && arg_count != 2) {
                semantic_error_node(analyzer, call_node, "indexOf/lastIndexOf requires 1 or 2 arguments");
                return -1;
            }
        } else if (is_substr_method) {
            if (arg_count != 1 && arg_count != 2) {
                semantic_error_node(analyzer, call_node, "substr requires 1 or 2 arguments");
                return -1;
            }
        }
        
        object_node = semantic_get_node(analyzer, object_idx);
        if (!object_node || check_expression(analyzer, object_node, &object_type) != 0) {
            return -1;
        }
        
        if (object_type.kind != TYPE_CLASS) {
            semantic_error_node(analyzer, call_node, "String method requires String receiver");
            return -1;
        }
        
        /* Compare class name string directly instead of offsets
         * This handles cases where "String" appears at different offsets in the string pool */
        {
            const char* class_name_str = semantic_get_string(analyzer, object_type.class_name);
            if (!class_name_str || strcmp(class_name_str, "String") != 0) {
                semantic_error_node(analyzer, call_node, "String method requires String receiver");
                return -1;
            }
        }
        
        /* Get the canonical "String" offset for return type */
        string_name_off = semantic_add_string(analyzer, "String");
        
        /* equals() returns boolean */
        if (is_equals_method) {
            result_type->kind = TYPE_BOOLEAN;
            result_type->class_name = 0;
        } else if (strcmp(method_name, "length") == 0 || is_comparison_method || is_compareto_method || is_index_method) {
            result_type->kind = TYPE_INT;
            result_type->class_name = 0;
        } else {
            /* toUpperCase, toLowerCase, substr return String */
            result_type->kind = TYPE_CLASS;
            result_type->class_name = string_name_off;
        }
        return 0;
    }
    
    /* Special-case Date instance methods */
    if (method_name && object_idx != 0 &&
        (strcmp(method_name, "getTime") == 0 ||
         strcmp(method_name, "setTime") == 0 ||
         strcmp(method_name, "getFullYear") == 0 ||
         strcmp(method_name, "getMonth") == 0 ||
         strcmp(method_name, "getDate") == 0 ||
         strcmp(method_name, "getHours") == 0 ||
         strcmp(method_name, "getMinutes") == 0 ||
         strcmp(method_name, "getSeconds") == 0)) {
        
        TypeInfo object_type;
        ASTNode* object_node;
        const char* class_name;
        
        /* Check object expression */
        object_node = semantic_get_node(analyzer, object_idx);
        if (!object_node || check_expression(analyzer, object_node, &object_type) != 0) {
            return -1;
        }
        
        /* Object must be of Date class type */
        if (object_type.kind != TYPE_CLASS) {
            semantic_error_node(analyzer, call_node, "Date method requires Date receiver");
            return -1;
        }
        
        /* Get class name string */
        class_name = semantic_get_string(analyzer, object_type.class_name);
        if (!class_name) {
            class_name = symtable_get_string(analyzer->symtable, object_type.class_name);
        }
        
        /* Verify it's a Date object */
        if (!class_name || strcmp(class_name, "Date") != 0) {
            semantic_error_node(analyzer, call_node, "Date method requires Date receiver");
            return -1;
        }
        
        /* Validate argument counts and set return types */
        if (strcmp(method_name, "getTime") == 0) {
            /* long getTime() - returns timestamp as long (milliseconds) */
            if (arg_count != 0) {
                semantic_error_node(analyzer, call_node, "getTime() takes no arguments");
                return -1;
            }
            result_type->kind = TYPE_LONG;
            result_type->class_name = 0;
        } else if (strcmp(method_name, "setTime") == 0) {
            /* void setTime(long timestamp) */
            if (arg_count != 1) {
                semantic_error_node(analyzer, call_node, "setTime() requires 1 argument");
                return -1;
            }
            result_type->kind = TYPE_VOID;
            result_type->class_name = 0;
        } else if (strcmp(method_name, "getFullYear") == 0 ||
                   strcmp(method_name, "getMonth") == 0 ||
                   strcmp(method_name, "getDate") == 0 ||
                   strcmp(method_name, "getHours") == 0 ||
                   strcmp(method_name, "getMinutes") == 0 ||
                   strcmp(method_name, "getSeconds") == 0) {
            /* int getXXX() - all getters return int */
            if (arg_count != 0) {
                semantic_error_node(analyzer, call_node, "Date getter methods take no arguments");
                return -1;
            }
            result_type->kind = TYPE_INT;
            result_type->class_name = 0;
        }
        
        return 0;
    }
    
    /* Special-case File static methods */
    if (method_name &&
        (strcmp(method_name, "open") == 0 ||
         strcmp(method_name, "readLine") == 0 ||
         strcmp(method_name, "writeLine") == 0 ||
         strcmp(method_name, "close") == 0)) {
        
        /* Check if this is File.method() call */
        if (object_idx != 0) {
            ASTNode* obj_node = semantic_get_node(analyzer, object_idx);
            if (obj_node && obj_node->type == NODE_IDENTIFIER) {
                const char* obj_name = semantic_get_string(analyzer, obj_node->data.identifier.name);
                if (obj_name && strcmp(obj_name, "File") == 0) {
                    /* This is a File.method() call */
                    if (strcmp(method_name, "open") == 0) {
                        /* File.open(String filename) or File.open(String filename, String mode) */
                        if (arg_count != 1 && arg_count != 2) {
                            semantic_error_node(analyzer, call_node, "File.open requires 1 or 2 arguments");
                            return -1;
                        }
                        /* Returns void */
                        result_type->kind = TYPE_VOID;
                        result_type->class_name = 0;
                    } else if (strcmp(method_name, "readLine") == 0) {
                        /* File.readLine() returns String */
                        if (arg_count != 0) {
                            semantic_error_node(analyzer, call_node, "File.readLine requires 0 arguments");
                            return -1;
                        }
                        /* Returns String */
                        result_type->kind = TYPE_CLASS;
                        result_type->class_name = semantic_add_string(analyzer, "String");
                    } else if (strcmp(method_name, "writeLine") == 0) {
                        /* File.writeLine(String text) */
                        if (arg_count != 1) {
                            semantic_error_node(analyzer, call_node, "File.writeLine requires 1 argument");
                            return -1;
                        }
                        /* Returns void */
                        result_type->kind = TYPE_VOID;
                        result_type->class_name = 0;
                    } else if (strcmp(method_name, "close") == 0) {
                        /* File.close() */
                        if (arg_count != 0) {
                            semantic_error_node(analyzer, call_node, "File.close requires 0 arguments");
                            return -1;
                        }
                        /* Returns void */
                        result_type->kind = TYPE_VOID;
                        result_type->class_name = 0;
                    }
                    return 0;
                }
            }
        }
    }
    
    /* General instance method call: object.method(...) */
    if (object_idx != 0) {
        TypeInfo object_type;
        const char* class_name;
        Symbol* class_sym;
        Symbol* method_sym = NULL;
        uint16_t checked_args;
        uint16_t current_arg_idx;
        uint16_t param_index;
        uint16_t i;
        int method_symbol_index = -1;
        
        /* Check object expression */
        object_node = semantic_get_node(analyzer, object_idx);
        if (!object_node || check_expression(analyzer, object_node, &object_type) != 0) {
            return -1;
        }
        
        /* Object must be of class type */
        if (object_type.kind != TYPE_CLASS) {
            semantic_error_node(analyzer, call_node, "Method call requires object");
            return -1;
        }
        
        /* Get class name */
        class_name = semantic_get_string(analyzer, object_type.class_name);
        if (!class_name) {
            class_name = symtable_get_string(analyzer->symtable, object_type.class_name);
        }
        if (!class_name) {
            semantic_error_node(analyzer, call_node, "Invalid class name in method call");
            return -1;
        }
        
        /* Lookup class symbol */
        class_sym = symtable_lookup(analyzer->symtable, class_name);
        if (!class_sym || class_sym->kind != SYM_CLASS) {
            semantic_error_node(analyzer, call_node, "Undefined class in method call");
            return -1;
        }
        
        /* Find method in class scope - match by name, arg count, and arg types */
        for (i = 0; i < analyzer->symtable->symbol_count; i++) {
            Symbol* sym = &analyzer->symtable->symbols[i];
            const char* sym_name;
            uint16_t j;
            
            if (sym->kind != SYM_METHOD) {
                continue;
            }
            
            sym_name = symtable_get_string(analyzer->symtable, sym->name_offset);
            if (!sym_name || strcmp(sym_name, method_name) != 0) {
                continue;
            }
            
            if (sym->data.method_data.param_count != arg_count) {
                continue;
            }
            
            /* Quick type check for first argument if present */
            if (arg_count > 0 && first_arg_idx != 0) {
                TypeInfo first_arg_type;
                Symbol* first_param_sym = NULL;
                ASTNode* first_arg_node = semantic_get_node(analyzer, first_arg_idx);
                
                if (first_arg_node && check_expression(analyzer, first_arg_node, &first_arg_type) == 0) {
                    /* Find first parameter */
                    for (j = (uint16_t)(i + 1); j < analyzer->symtable->symbol_count; j++) {
                        Symbol* psym = &analyzer->symtable->symbols[j];
                        if (psym->kind == SYM_METHOD) break;
                        if (psym->kind == SYM_PARAM && psym->data.param_data.index == 0) {
                            first_param_sym = psym;
                            break;
                        }
                    }
                    
                    if (first_param_sym && !types_compatible(first_param_sym->type, first_arg_type)) {
                        continue; /* Type mismatch, try next method */
                    }
                }
            }
            
            /* Found matching method */
            method_sym = sym;
            method_symbol_index = (int)i;
            break;
        }
        
        if (!method_sym) {
            semantic_error_node(analyzer, call_node, "Undefined method");
            return -1;
        }
        
        /* Check argument types */
        checked_args = 0;
        current_arg_idx = first_arg_idx;
        param_index = 0;
        
        while (current_arg_idx != 0 && checked_args < arg_count) {
            TypeInfo arg_type;
            Symbol* param_sym = NULL;
            uint16_t next_arg_idx;
            
            arg_node = semantic_get_node(analyzer, current_arg_idx);
            if (!arg_node || check_expression(analyzer, arg_node, &arg_type) != 0) {
                return -1;
            }
            
            /* Find parameter symbol */
            if (method_symbol_index >= 0) {
                for (i = (uint16_t)(method_symbol_index + 1); i < analyzer->symtable->symbol_count; i++) {
                    Symbol* sym = &analyzer->symtable->symbols[i];
                    if (sym->kind == SYM_METHOD) {
                        break;
                    }
                    if (sym->kind != SYM_PARAM) {
                        continue;
                    }
                    if (sym->data.param_data.index == param_index) {
                        param_sym = sym;
                        break;
                    }
                }
            }
            
            if (!param_sym) {
                semantic_error_node(analyzer, call_node, "Internal error: method parameter metadata not found");
                return -1;
            }
            
            if (!types_compatible(param_sym->type, arg_type)) {
                semantic_error_node(analyzer, call_node, "Argument type mismatch in method call");
                return -1;
            }
            
            next_arg_idx = arg_node->next_sibling;
            current_arg_idx = next_arg_idx;
            checked_args++;
            param_index++;
        }
        
        *result_type = method_sym->type;
        return 0;
    }
    
    /* Static method call: ClassName.method(...) or method(...) */
    {
        Symbol* method_sym = NULL;
        uint16_t checked_args;
        uint16_t current_arg_idx;
        uint16_t param_index;
        uint16_t i;
        int method_symbol_index = -1;
        
        if (!method_name) {
            semantic_error_node(analyzer, call_node, "Invalid method name");
            return -1;
        }
        
        for (i = analyzer->symtable->symbol_count; i > 0; i--) {
            Symbol* sym = &analyzer->symtable->symbols[i - 1];
            const char* sym_name = symtable_get_string(analyzer->symtable, sym->name_offset);
            if (sym->kind == SYM_METHOD &&
                sym_name &&
                strcmp(sym_name, method_name) == 0) {
                method_sym = sym;
                method_symbol_index = (int)(i - 1);
                break;
            }
        }
        
        if (!method_sym) {
            semantic_error_node(analyzer, call_node, "Undefined method");
            return -1;
        }
        
        if (!method_sym->data.method_data.is_static) {
            semantic_error_node(analyzer, call_node, "Only static methods are supported");
            return -1;
        }
        
        if (method_sym->data.method_data.param_count != arg_count) {
            semantic_error_node(analyzer, call_node, "Argument count mismatch in method call");
            return -1;
        }
        
        checked_args = 0;
        current_arg_idx = first_arg_idx;
        param_index = 0;
        
        while (current_arg_idx != 0 && checked_args < arg_count) {
            TypeInfo arg_type;
            Symbol* param_sym = NULL;
            uint16_t next_arg_idx;
            
            arg_node = semantic_get_node(analyzer, current_arg_idx);
            if (!arg_node || check_expression(analyzer, arg_node, &arg_type) != 0) {
                return -1;
            }
            
            if (method_symbol_index >= 0) {
                for (i = (uint16_t)(method_symbol_index + 1); i < analyzer->symtable->symbol_count; i++) {
                    Symbol* sym = &analyzer->symtable->symbols[i];
                    if (sym->kind == SYM_METHOD) {
                        break;
                    }
                    if (sym->kind != SYM_PARAM) {
                        continue;
                    }
                    if (sym->data.param_data.index == param_index) {
                        param_sym = sym;
                        break;
                    }
                }
            }
            
            if (!param_sym) {
                semantic_error_node(analyzer, call_node, "Internal error: method parameter metadata not found");
                return -1;
            }
            
            if (!types_compatible(param_sym->type, arg_type)) {
                semantic_error_node(analyzer, call_node, "Argument type mismatch in method call");
                return -1;
            }
            
            next_arg_idx = arg_node->next_sibling;
            current_arg_idx = next_arg_idx;
            checked_args++;
            param_index++;
        }
        
        *result_type = method_sym->type;
        return 0;
    }
}

/* Type checking helpers */

int types_compatible(TypeInfo t1, TypeInfo t2) {
    if (t1.kind != t2.kind) {
        return 0;
    }
    
    if (t1.kind == TYPE_CLASS) {
        if (t1.class_name == t2.class_name) {
            return 1;
        }
        
        /* Phase 1/2 currently only supports nominal String reference values.
         * Different passes may preserve different pool offsets for "String",
         * so treat any TYPE_CLASS pair as compatible for now.
         */
        return 1;
    }
    
    return 1;
}

int is_boolean_type(TypeInfo type) {
    return type.kind == TYPE_BOOLEAN;
}

int is_numeric_type(TypeInfo type) {
    return type.kind == TYPE_INT || type.kind == TYPE_LONG || type.kind == TYPE_FLOAT;
}

int is_void_type(TypeInfo type) {
    return type.kind == TYPE_VOID;
}

int get_binary_op_result_type(BinaryOp op, TypeInfo left_type, TypeInfo right_type, TypeInfo* result_type) {
    if (!result_type) {
        return -1;
    }
    
    /* Arithmetic operations: numeric op numeric -> result type
     * If either operand is long, result is long; otherwise int
     * String concatenation is handled in check_binary_op() because nominal
     * String detection needs analyzer access to resolve string-pool names.
     */
    if (op == BINOP_ADD || op == BINOP_SUB || op == BINOP_MUL || op == BINOP_DIV || op == BINOP_MOD) {
        if (is_numeric_type(left_type) && is_numeric_type(right_type)) {
            /* Type promotion: float > long > int */
            if (left_type.kind == TYPE_FLOAT || right_type.kind == TYPE_FLOAT) {
                result_type->kind = TYPE_FLOAT;
            } else if (left_type.kind == TYPE_LONG || right_type.kind == TYPE_LONG) {
                result_type->kind = TYPE_LONG;
            } else {
                result_type->kind = TYPE_INT;
            }
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
    
    /* Negation: -numeric -> same type */
    if (op == UNOP_NEG) {
        if (is_numeric_type(operand_type)) {
            result_type->kind = operand_type.kind;  /* Preserve int or long */
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




/* Check if expression is a compile-time constant */
static int is_constant_expression(ASTNode* expr_node) {
    if (!expr_node) {
        return 0;
    }
    
    /* Accept integer and string literals */
    return expr_node->type == NODE_LITERAL_INT ||
           expr_node->type == NODE_LITERAL_LONG ||
           expr_node->type == NODE_LITERAL_STRING;
}

/* Check case label */
static int check_case_label(SemanticAnalyzer* analyzer, ASTNode* case_node, TypeInfo switch_type) {
    ASTNode* value_node;
    ASTNode* stmt_node;
    TypeInfo value_type;
    uint16_t value_idx;
    uint16_t stmt_idx;
    
    if (!case_node || case_node->type != NODE_CASE) {
        return -1;
    }
    
    /* Get case value expression */
    value_idx = case_node->data.case_label.value;
    value_node = semantic_get_node(analyzer, value_idx);
    if (!value_node) {
        semantic_error_node(analyzer, case_node, "Invalid case value");
        return -1;
    }
    
    /* Check if value is a constant expression */
    if (!is_constant_expression(value_node)) {
        semantic_error_node(analyzer, case_node, "Case value must be a constant expression");
        return -1;
    }
    
    /* Check value expression type */
    if (check_expression(analyzer, value_node, &value_type) != 0) {
        return -1;
    }
    
    /* Check type compatibility with switch expression */
    if (value_type.kind != switch_type.kind) {
        semantic_error_node(analyzer, case_node, "Case value type does not match switch expression type");
        return -1;
    }
    
    /* Check case statement (already in switch context) */
    stmt_idx = case_node->data.case_label.stmt;
    if (stmt_idx != 0) {
        stmt_node = semantic_get_node(analyzer, stmt_idx);
        if (stmt_node) {
            if (check_statement(analyzer, stmt_node, stmt_idx) != 0) {
                return -1;
            }
        }
    }
    
    return 0;
}

/* Check switch statement */
static int check_switch_stmt(SemanticAnalyzer* analyzer, ASTNode* switch_node) {
    ASTNode* expr_node;
    ASTNode* case_node;
    ASTNode* default_node;
    TypeInfo expr_type;
    uint16_t expr_idx;
    uint16_t case_idx;
    uint16_t default_idx;
    int old_in_switch;
    
    if (!switch_node || switch_node->type != NODE_SWITCH) {
        return -1;
    }
    
    /* Get switch expression */
    expr_idx = switch_node->data.switch_stmt.expr;
    expr_node = semantic_get_node(analyzer, expr_idx);
    if (!expr_node) {
        semantic_error_node(analyzer, switch_node, "Invalid switch expression");
        return -1;
    }
    
    /* Check switch expression type */
    if (check_expression(analyzer, expr_node, &expr_type) != 0) {
        return -1;
    }
    
    /* Allow int, long, and String types */
    if (expr_type.kind != TYPE_INT && expr_type.kind != TYPE_LONG && expr_type.kind != TYPE_CLASS) {
        semantic_error_node(analyzer, switch_node, "Switch expression must be int, long, or String type");
        return -1;
    }
    
    /* If TYPE_CLASS, verify it's String */
    if (expr_type.kind == TYPE_CLASS) {
        const char* class_name = semantic_get_string(analyzer, expr_type.class_name);
        if (!class_name || strcmp(class_name, "String") != 0) {
            semantic_error_node(analyzer, switch_node, "Switch expression must be int, long, or String type");
            return -1;
        }
    }
    
    /* Set switch context */
    old_in_switch = analyzer->in_switch;
    analyzer->in_switch = 1;
    
    /* Check all case labels */
    case_idx = switch_node->data.switch_stmt.first_case;
    while (case_idx != 0) {
        case_node = semantic_get_node(analyzer, case_idx);
        if (!case_node) {
            analyzer->in_switch = old_in_switch;
            return -1;
        }
        
        if (check_case_label(analyzer, case_node, expr_type) != 0) {
            analyzer->in_switch = old_in_switch;
            return -1;
        }
        
        /* Move to next case */
        case_idx = case_node->data.case_label.next_case;
    }
    
    /* Check default statement if present */
    if (switch_node->data.switch_stmt.has_default) {
        default_idx = switch_node->data.switch_stmt.default_stmt;
        if (default_idx != 0) {
            default_node = semantic_get_node(analyzer, default_idx);
            if (default_node) {
                if (check_statement(analyzer, default_node, default_idx) != 0) {
                    analyzer->in_switch = old_in_switch;
                    return -1;
                }
            }
        }
    }
    
    /* Restore switch context */
    analyzer->in_switch = old_in_switch;
    
    return 0;
}
