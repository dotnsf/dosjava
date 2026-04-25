#include "symtable.h"
#include <string.h>
#include <stdlib.h>

/**
 * DOS Java Compiler - Symbol Table Implementation
 * 
 * Implements symbol table operations for semantic analysis.
 * Optimized for 16-bit DOS environment.
 */

/* Initialize symbol table */
int symtable_init(SymbolTable* table) {
    if (!table) {
        return -1;
    }
    
    /* Clear all fields */
    memset(table, 0, sizeof(SymbolTable));
    
    /* Initialize scope stack with global scope */
    table->scope_stack[0] = 0;
    table->scope_level = 0;
    table->symbol_count = 0;
    table->pool_size = 0;
    
    return 0;
}

/* Cleanup symbol table */
void symtable_cleanup(SymbolTable* table) {
    if (!table) {
        return;
    }
    
    /* Nothing to free - all data is embedded in the structure */
    memset(table, 0, sizeof(SymbolTable));
}

/* Enter new scope */
int symtable_enter_scope(SymbolTable* table) {
    if (!table) {
        return -1;
    }
    
    /* Check scope depth limit */
    if (table->scope_level >= 15) {
        return -1;  /* Max 16 levels (0-15) */
    }
    
    /* Push current symbol count as scope start */
    table->scope_level++;
    table->scope_stack[table->scope_level] = table->symbol_count;
    
    return 0;
}

/* Exit current scope */
int symtable_exit_scope(SymbolTable* table) {
    uint16_t scope_start;
    
    if (!table) {
        return -1;
    }
    
    /* Cannot exit global scope */
    if (table->scope_level == 0) {
        return -1;
    }
    
    /* NOTE: We do NOT remove symbols when exiting scope because we need
     * to preserve them for code generation phase. The symbol table is
     * written to a file and read back during code generation. */
    scope_start = table->scope_stack[table->scope_level];
    /* table->symbol_count = scope_start; */ /* DISABLED: Keep symbols for codegen */
    
    /* Pop scope */
    table->scope_level--;

    return 0;
}

/* Add symbol to table */
uint16_t symtable_add_symbol(SymbolTable* table, const Symbol* sym) {
    uint16_t index;
    
    if (!table || !sym) {
        return 0xFFFF;
    }
    
    /* Check symbol table capacity */
    if (table->symbol_count >= 256) {
        return 0xFFFF;
    }
    
    /* Copy symbol */
    index = table->symbol_count;
    memcpy(&table->symbols[index], sym, sizeof(Symbol));
    
    /* Set scope level */
    table->symbols[index].scope_level = table->scope_level;
    
    table->symbol_count++;
    
    return index;
}

/* Lookup symbol by name (searches from current scope to global) */
Symbol* symtable_lookup(SymbolTable* table, const char* name) {
    uint16_t i;
    const char* sym_name;
    
    if (!table || !name) {
        return NULL;
    }
    
    /* Search from most recent to oldest */
    for (i = table->symbol_count; i > 0; i--) {
        sym_name = symtable_get_string(table, table->symbols[i - 1].name_offset);
        if (sym_name && strcmp(sym_name, name) == 0) {
            return &table->symbols[i - 1];
        }
    }
    
    return NULL;
}

/* Lookup symbol in specific scope only */
Symbol* symtable_lookup_in_scope(SymbolTable* table, const char* name, uint16_t scope) {
    uint16_t i;
    uint16_t scope_start;
    uint16_t scope_end;
    const char* sym_name;
    
    if (!table || !name || scope > table->scope_level) {
        return NULL;
    }
    
    /* Determine scope boundaries */
    scope_start = table->scope_stack[scope];
    if (scope < table->scope_level) {
        scope_end = table->scope_stack[scope + 1];
    } else {
        scope_end = table->symbol_count;
    }
    
    /* Search within scope */
    for (i = scope_start; i < scope_end; i++) {
        if (table->symbols[i].scope_level == scope) {
            sym_name = symtable_get_string(table, table->symbols[i].name_offset);
            if (sym_name && strcmp(sym_name, name) == 0) {
                return &table->symbols[i];
            }
        }
    }
    
    return NULL;
}

/* Check if symbol exists in current scope */
int symtable_exists_in_current_scope(SymbolTable* table, const char* name) {
    return symtable_lookup_in_scope(table, name, table->scope_level) != NULL;
}

/* Add string to string pool */
uint16_t symtable_add_string(SymbolTable* table, const char* str) {
    uint16_t len;
    uint16_t offset;
    uint16_t i;
    const char* pool_str;
    
    if (!table || !str) {
        return 0xFFFF;
    }
    
    len = strlen(str);
    
    /* Check if string already exists in pool */
    for (i = 0; i < table->pool_size; ) {
        pool_str = &table->string_pool[i];
        if (strcmp(pool_str, str) == 0) {
            return i;  /* Return existing offset */
        }
        i += strlen(pool_str) + 1;  /* Skip to next string */
    }
    
    /* Check pool capacity */
    if (table->pool_size + len + 1 > 2048) {
        return 0xFFFF;
    }
    
    /* Add new string */
    offset = table->pool_size;
    strcpy(&table->string_pool[offset], str);
    table->pool_size += len + 1;
    
    return offset;
}

/* Get string from string pool */
const char* symtable_get_string(SymbolTable* table, uint16_t offset) {
    if (!table || offset >= table->pool_size) {
        return NULL;
    }
    
    return &table->string_pool[offset];
}

/* Write symbol table to file */
int symtable_write(SymbolTable* table, const char* filename) {
    FILE* file;
    uint16_t i;
    
    if (!table || !filename) {
        return -1;
    }
    
    
    for (i = 0; i < table->symbol_count; i++) {
        const char* name = &table->string_pool[table->symbols[i].name_offset];
        printf("  [%u] kind=%d, name='%s'\n", i, table->symbols[i].kind, name);
    }

    file = fopen(filename, "wb");
    if (!file) {
        return -1;
    }
    
    /* Write header */
    fwrite(&table->symbol_count, sizeof(uint16_t), 1, file);
    fwrite(&table->pool_size, sizeof(uint16_t), 1, file);
    
    /* Write symbols */
    if (table->symbol_count > 0) {
        fwrite(table->symbols, sizeof(Symbol), table->symbol_count, file);
    }
    
    /* Write string pool */
    if (table->pool_size > 0) {
        fwrite(table->string_pool, 1, table->pool_size, file);
    }
    
    fclose(file);
    return 0;
}

/* Read symbol table from file */
int symtable_read(SymbolTable* table, const char* filename) {
    FILE* file;
    uint16_t i;
    
    if (!table || !filename) {
        return -1;
    }
    
    file = fopen(filename, "rb");
    if (!file) {
        
        return -1;
    }
    
    /* Read header */
    if (fread(&table->symbol_count, sizeof(uint16_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    if (fread(&table->pool_size, sizeof(uint16_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    
    
    /* Validate counts */
    if (table->symbol_count > 256 || table->pool_size > 2048) {
        fclose(file);
        return -1;
    }
    
    /* Read symbols */
    if (table->symbol_count > 0) {
        if (fread(table->symbols, sizeof(Symbol), table->symbol_count, file) != table->symbol_count) {
            fclose(file);
            return -1;
        }
    }
    
    /* Read string pool */
    if (table->pool_size > 0) {
        if (fread(table->string_pool, 1, table->pool_size, file) != table->pool_size) {
            fclose(file);
            return -1;
        }
    }
    
    /* Initialize scope stack */
    table->scope_stack[0] = 0;
    table->scope_level = 0;
    
    /* Debug: Print loaded symbols */
    
    for (i = 0; i < table->symbol_count; i++) {
        const char* name = &table->string_pool[table->symbols[i].name_offset];
        printf("  [%u] kind=%d, name='%s'\n", i, table->symbols[i].kind, name);
    }
    
    fclose(file);
    return 0;
}

/* Get symbol kind name */
const char* symbol_kind_name(SymbolKind kind) {
    switch (kind) {
        case SYM_CLASS:  return "class";
        case SYM_METHOD: return "method";
        case SYM_FIELD:  return "field";
        case SYM_LOCAL:  return "local";
        case SYM_PARAM:  return "param";
        default:         return "unknown";
    }
}

/* Get type kind name (internal helper) */
static const char* get_type_name(TypeKind kind) {
    switch (kind) {
        case TYPE_VOID:    return "void";
        case TYPE_INT:     return "int";
        case TYPE_BOOLEAN: return "boolean";
        case TYPE_CLASS:   return "class";
        default:           return "unknown";
    }
}

/* Print symbol table */
void symtable_print(SymbolTable* table) {
    uint16_t i;
    const char* name;
    const Symbol* sym;
    
    if (!table) {
        return;
    }
    
    printf("Symbol Table (%u symbols, scope level %u)\n", 
           table->symbol_count, table->scope_level);
    printf("----------------------------------------\n");
    
    for (i = 0; i < table->symbol_count; i++) {
        sym = &table->symbols[i];
        name = symtable_get_string(table, sym->name_offset);
        
        printf("[%3u] %-10s %-8s %-8s scope=%u",
               i, name ? name : "(null)",
               symbol_kind_name(sym->kind),
               get_type_name(sym->type.kind),
               sym->scope_level);
        
        /* Print kind-specific data */
        switch (sym->kind) {
            case SYM_CLASS:
                printf(" members=%u", sym->data.class_data.member_count);
                break;
            case SYM_METHOD:
                printf(" params=%u locals=%u %s%s",
                       sym->data.method_data.param_count,
                       sym->data.method_data.local_count,
                       sym->data.method_data.is_static ? "static " : "",
                       sym->data.method_data.is_public ? "public" : "private");
                break;
            case SYM_FIELD:
                printf(" %s%s",
                       sym->data.field_data.is_static ? "static " : "",
                       sym->data.field_data.is_public ? "public" : "private");
                break;
            case SYM_LOCAL:
                printf(" index=%u", sym->data.local_data.index);
                break;
            case SYM_PARAM:
                printf(" index=%u", sym->data.param_data.index);
                break;
        }
        
        printf("\n");
    }
    
    printf("----------------------------------------\n");
    printf("String pool: %u bytes used\n", table->pool_size);
}

// Made with Bob
