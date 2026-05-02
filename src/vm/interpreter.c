#include "interpreter.h"
#include "memory.h"
#include "../format/opcodes.h"
#include "../runtime/system.h"
#include <stdio.h>
#include <string.h>

/* ===== Stack Helper Functions ===== */

/**
 * Push value onto shared stack
 */
static inline int stack_push_shared(ExecutionContext* ctx, uint16_t value) {
    if (ctx->stack_pointer >= SHARED_STACK_SIZE) {
        return -1;  /* Stack overflow */
    }
    ctx->shared_stack[ctx->stack_pointer++] = value;
    return 0;
}

/**
 * Pop value from shared stack
 */
static inline uint16_t stack_pop_shared(ExecutionContext* ctx) {
    if (ctx->stack_pointer == 0) {
        return 0;  /* Stack underflow */
    }
    return ctx->shared_stack[--ctx->stack_pointer];
}

/**
 * Peek at top of shared stack
 */
static inline uint16_t stack_peek_shared(ExecutionContext* ctx) {
    if (ctx->stack_pointer == 0) {
        return 0;
    }
    return ctx->shared_stack[ctx->stack_pointer - 1];
}

/**
 * Duplicate top of shared stack
 */
static inline int stack_dup_shared(ExecutionContext* ctx) {
    if (ctx->stack_pointer == 0 || ctx->stack_pointer >= SHARED_STACK_SIZE) {
        return -1;
    }
    ctx->shared_stack[ctx->stack_pointer] = ctx->shared_stack[ctx->stack_pointer - 1];
    ctx->stack_pointer++;
    return 0;
}

/* ===== Local Variable Helper Functions ===== */

/**
 * Get current frame pointer
 */
static inline uint16_t get_frame_pointer(ExecutionContext* ctx) {
    if (ctx->call_depth == 0) {
        return 0;
    }
    return ctx->call_frames[ctx->call_depth - 1].frame_pointer;
}

/**
 * Get current local base
 */
static inline uint16_t get_local_base(ExecutionContext* ctx) {
    if (ctx->call_depth == 0) {
        return 0;
    }
    return ctx->call_frames[ctx->call_depth - 1].local_base;
}

/**
 * Load local variable
 */
static inline uint16_t load_local(ExecutionContext* ctx, uint8_t index) {
    uint16_t base = get_local_base(ctx);
    if (base + index >= SHARED_LOCALS_SIZE) {
        return 0;  /* Out of bounds */
    }
    return ctx->shared_locals[base + index];
}

/**
 * Store local variable
 */
static inline void store_local(ExecutionContext* ctx, uint8_t index, uint16_t value) {
    uint16_t base = get_local_base(ctx);
    if (base + index < SHARED_LOCALS_SIZE) {
        ctx->shared_locals[base + index] = value;
    }
}

/* ===== Context Management ===== */

/**
 * Initialize execution context
 */
int interpreter_init_context(ExecutionContext* ctx, DJCFile* djc_file, DJCMethod* method) {
    uint8_t* code;
    
    if (ctx == NULL || djc_file == NULL || method == NULL) {
        return -1;
    }
    
    /* Get method code */
    code = djc_get_method_code(djc_file, method);
    if (code == NULL) {
        return -1;
    }
    
    /* Initialize context */
    memset(ctx, 0, sizeof(ExecutionContext));
    
    ctx->pc = code;
    ctx->code_start = code;
    ctx->code_length = method->code_length;
    ctx->djc_file = djc_file;
    ctx->running = 1;
    
    /* Initialize shared stack */
    ctx->stack_pointer = 0;
    
    /* Initialize shared locals */
    ctx->local_pointer = 0;
    
    /* Allocate space for main method locals */
    if (method->max_locals > 0) {
        if (method->max_locals > SHARED_LOCALS_SIZE) {
            return -1;  /* Too many locals */
        }
        ctx->local_pointer = method->max_locals;
        /* Initialize locals to 0 */
        memset(ctx->shared_locals, 0, method->max_locals * sizeof(uint16_t));
    }
    
    /* Initialize call frames */
    ctx->call_depth = 0;
    
    return 0;
}

/**
 * Free execution context resources
 */
void interpreter_free_context(ExecutionContext* ctx) {
    if (ctx == NULL) {
        return;
    }
    
    /* Nothing to free - all memory is static */
    ctx->running = 0;
}

/**
 * Get local variable (legacy interface)
 */
uint16_t interpreter_get_local(ExecutionContext* ctx, uint16_t index) {
    if (ctx == NULL) {
        return 0;
    }
    return load_local(ctx, (uint8_t)index);
}

/**
 * Set local variable (legacy interface)
 */
void interpreter_set_local(ExecutionContext* ctx, uint16_t index, uint16_t value) {
    if (ctx == NULL) {
        return;
    }
    store_local(ctx, (uint8_t)index, value);
}

/**
 * Read 8-bit operand from code
 */
uint8_t interpreter_read_u8(ExecutionContext* ctx) {
    uint8_t value;
    
    if (ctx == NULL || ctx->pc == NULL) {
        return 0;
    }
    
    value = *ctx->pc;
    ctx->pc++;
    
    return value;
}

/**
 * Read 16-bit operand from code (little-endian)
 */
uint16_t interpreter_read_u16(ExecutionContext* ctx) {
    uint16_t value;
    
    if (ctx == NULL || ctx->pc == NULL) {
        return 0;
    }
    
    value = (uint16_t)ctx->pc[0] | ((uint16_t)ctx->pc[1] << 8);
    ctx->pc += 2;
    
    return value;
}

/**
 * Execute a single instruction
 */
int interpreter_step(ExecutionContext* ctx) {
    uint8_t opcode;
    uint16_t value1, value2, result;
    uint8_t index8;
    uint16_t index16;
    int16_t offset;
    
    if (ctx == NULL || !ctx->running) {
        return -1;
    }
    
    /* Check PC bounds - allow one byte beyond for compiler bug workaround */
    /* Allow reading up to length+1 to handle compiler's off-by-one error */
    if (ctx->pc < ctx->code_start ||
        ctx->pc > ctx->code_start + ctx->code_length + 1) {
        return -1;
    }
    
    /* Fetch opcode */
    opcode = interpreter_read_u8(ctx);
    
    /* Decode and execute */
    switch (opcode) {
        case OP_NOP:
            /* No operation */
            break;
        
        case OP_PUSH_CONST: {
            /* Push constant from pool */
            const char* str_value;
            
            index16 = interpreter_read_u16(ctx);
            
            /* Get UTF8 string from constant pool */
            str_value = djc_get_utf8(ctx->djc_file, index16);
            if (str_value == NULL) {
                printf("ERROR: Invalid constant index: %u\n", index16);
                return -1;
            }
            
            /* For now, push the constant index itself */
            /* In Phase 4, we'll create proper string objects */
            if (stack_push_shared(ctx, index16) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_PUSH_INT:
            /* Push immediate integer */
            value1 = interpreter_read_u16(ctx);
            if (stack_push_shared(ctx, value1) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_POP:
            /* Pop and discard */
            
            if (ctx->stack_pointer == 0) {
                printf("ERROR: OP_POP on empty stack!\n");
                return -1;
            }
            stack_pop_shared(ctx);
            
            break;
        
        case OP_DUP:
            /* Duplicate top of stack */
            if (stack_dup_shared(ctx) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_LOAD_LOCAL:
            /* Load local variable */
            index8 = interpreter_read_u8(ctx);
            value1 = load_local(ctx, index8);
            if (stack_push_shared(ctx, value1) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_STORE_LOCAL:
            /* Store to local variable */
            index8 = interpreter_read_u8(ctx);
            value1 = stack_pop_shared(ctx);
            store_local(ctx, index8, value1);
            break;
        
        case OP_LOAD_0:
            value1 = load_local(ctx, 0);
            if (stack_push_shared(ctx, value1) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_LOAD_1:
            value1 = load_local(ctx, 1);
            if (stack_push_shared(ctx, value1) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_LOAD_2:
            value1 = load_local(ctx, 2);
            if (stack_push_shared(ctx, value1) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_STORE_0:
            value1 = stack_pop_shared(ctx);
            store_local(ctx, 0, value1);
            break;
        
        case OP_STORE_1:
            value1 = stack_pop_shared(ctx);
            store_local(ctx, 1, value1);
            break;
        
        case OP_STORE_2:
            value1 = stack_pop_shared(ctx);
            store_local(ctx, 2, value1);
            break;
        
        case OP_ADD:
            /* Integer addition */
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            result = (uint16_t)((int16_t)value1 + (int16_t)value2);
            if (stack_push_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_SUB:
            /* Integer subtraction */
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            result = (uint16_t)((int16_t)value1 - (int16_t)value2);
            if (stack_push_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_MUL:
            /* Integer multiplication */
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            result = (uint16_t)((int16_t)value1 * (int16_t)value2);
            if (stack_push_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_DIV:
            /* Integer division */
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if (value2 == 0) {
                printf("ERROR: Division by zero\n");
                return -1;
            }
            result = (uint16_t)((int16_t)value1 / (int16_t)value2);
            if (stack_push_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_MOD:
            /* Integer modulo */
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if (value2 == 0) {
                printf("ERROR: Modulo by zero\n");
                return -1;
            }
            result = (uint16_t)((int16_t)value1 % (int16_t)value2);
            if (stack_push_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_NEG:
            /* Negate */
            value1 = stack_pop_shared(ctx);
            result = (uint16_t)(-(int16_t)value1);
            if (stack_push_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_INC_LOCAL:
            /* Increment local variable */
            index8 = interpreter_read_u8(ctx);
            value1 = (uint16_t)((int8_t)interpreter_read_u8(ctx));
            value2 = load_local(ctx, index8);
            result = value2 + value1;
            store_local(ctx, index8, result);
            break;
        
        case OP_CMP_EQ:
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if (stack_push_shared(ctx, (value1 == value2) ? 1 : 0) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_CMP_NE:
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if (stack_push_shared(ctx, (value1 != value2) ? 1 : 0) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_CMP_LT:
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            result = ((int16_t)value1 < (int16_t)value2) ? 1 : 0;
            if (stack_push_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_CMP_LE:
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if (stack_push_shared(ctx, ((int16_t)value1 <= (int16_t)value2) ? 1 : 0) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_CMP_GT:
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if (stack_push_shared(ctx, ((int16_t)value1 > (int16_t)value2) ? 1 : 0) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_CMP_GE:
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if (stack_push_shared(ctx, ((int16_t)value1 >= (int16_t)value2) ? 1 : 0) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        
        case OP_GOTO:
            /* Unconditional jump - offset is RELATIVE to current PC */
            offset = (int16_t)interpreter_read_u16(ctx);
            /* offset is relative to current PC */
            ctx->pc += offset;
            break;
        
        case OP_IF_TRUE:
            /* Jump if true - offset is RELATIVE to current PC */
            offset = (int16_t)interpreter_read_u16(ctx);
            value1 = stack_pop_shared(ctx);
            if (value1 != 0) {
                ctx->pc += offset;
            }
            break;
        
        case OP_IF_FALSE:
            /* Jump if false - offset is RELATIVE to current PC */
            offset = (int16_t)interpreter_read_u16(ctx);
            value1 = stack_pop_shared(ctx);
            if (value1 == 0) {
                ctx->pc += offset;
            }
            break;
        
        case OP_IF_EQ:
            /* Jump if equal */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if (value1 == value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_NE:
            /* Jump if not equal */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if (value1 != value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_LT:
            /* Jump if less than */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if ((int16_t)value1 < (int16_t)value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_LE:
            /* Jump if less or equal */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if ((int16_t)value1 <= (int16_t)value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_GT:
            /* Jump if greater than */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if ((int16_t)value1 > (int16_t)value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_GE:
            /* Jump if greater or equal */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop_shared(ctx);
            value1 = stack_pop_shared(ctx);
            if ((int16_t)value1 >= (int16_t)value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_PRINT_INT:
            /* Debug: print integer */
            value1 = stack_pop_shared(ctx);
            system_print_int((int16_t)value1);
            break;
        
        case OP_PRINT_CHAR:
            /* Debug: print character */
            value1 = stack_pop_shared(ctx);
            system_print_char((char)value1);
            break;
        
        case 0x0D:
        case 0x0E:
        case 0x0F:
            /* Return void (legacy opcodes for compatibility) */
            ctx->running = 0;
            return 1;
        
        case OP_INVOKE_STATIC: {
            uint16_t method_index;
            DJCMethod* method;
            CallFrame* frame;
            uint8_t* method_code;
            const char* method_name;
            
            /* Read method index (2 bytes, little-endian) */
            method_index = interpreter_read_u16(ctx);
            
            
            
            /* Look up method */
            method = djc_find_method(ctx->djc_file, method_index);
            if (method == NULL) {
                printf("ERROR: Method not found (index: %d)\n", method_index);
                return -1;
            }
            
            /* Get method name */
            method_name = djc_get_utf8(ctx->djc_file, method->name_index);
            
            
            /* Check if method is native */
            if (method->flags & METHOD_NATIVE) {
                /* Handle native methods - don't increment call depth */
                
                if (method_name) {
                    /* Check for System.out.println */
                    if (strcmp(method_name, "println") == 0) {
                        /* Pop value from stack */
                        uint16_t value;
                        const char* str;
                        const char* descriptor;
                        
                        value = stack_pop_shared(ctx);
                        
                        /* Get method descriptor to determine parameter type */
                        descriptor = djc_get_utf8(ctx->djc_file, method->descriptor_index);
                        
                        /* Check descriptor for parameter type:
                         * (Ljava/lang/String;)V - String parameter
                         * (I)V - int parameter
                         */
                        if (descriptor && strstr(descriptor, "(I)") != NULL) {
                            /* Integer parameter - print as integer */
                            system_println_int((int16_t)value);
                        } else if (descriptor && strstr(descriptor, "(Ljava/lang/String;)") != NULL) {
                            /* String parameter - value is constant pool index */
                            str = NULL;
                            if (value < ctx->djc_file->header.constant_pool_count) {
                                if (ctx->djc_file->constants[value].tag == CONST_UTF8) {
                                    str = ctx->djc_file->constants[value].data.utf8_data;
                                }
                            }
                            if (str) {
                                system_println_cstr(str);
                            } else {
                                printf("ERROR: Invalid string constant index: %d\n", value);
                                return -1;
                            }
                        } else {
                            /* Fallback: try to detect if it's a string constant */
                            str = NULL;
                            if (value < ctx->djc_file->header.constant_pool_count) {
                                if (ctx->djc_file->constants[value].tag == CONST_UTF8) {
                                    str = ctx->djc_file->constants[value].data.utf8_data;
                                }
                            }
                            if (str) {
                                system_println_cstr(str);
                            } else {
                                system_println_int((int16_t)value);
                            }
                        }
                        break;
                    }
                }
                
                printf("ERROR: Unsupported native method: %s\n",
                       method_name ? method_name : "???");
                return -1;
            }
            
            /* Check call depth for non-native methods */
            
            if (ctx->call_depth >= MAX_CALL_DEPTH) {
                printf("ERROR: Call stack overflow (max depth: %d)\n", MAX_CALL_DEPTH);
                return -1;
            }
            
            /* Get method code */
            method_code = djc_get_method_code(ctx->djc_file, method);
            if (method_code == NULL) {
                printf("ERROR: Failed to get method code\n");
                return -1;
            }
            
            /* Check if we have enough space for locals */
            if (ctx->local_pointer + method->max_locals > SHARED_LOCALS_SIZE) {
                printf("ERROR: Not enough space for local variables\n");
                return -1;
            }
            
            /* Save current state to call frame */
            frame = &ctx->call_frames[ctx->call_depth];
            frame->return_pc = ctx->pc;
            frame->return_code_start = ctx->code_start;
            frame->return_code_length = ctx->code_length;
            frame->frame_pointer = ctx->stack_pointer;
            frame->local_base = ctx->local_pointer;
            frame->local_count = method->max_locals;
            
            /* Increment call depth */
            ctx->call_depth++;
            
            /* Allocate space for new method's locals */
            ctx->local_pointer += method->max_locals;
            
            /* Initialize new locals to 0 */
            if (method->max_locals > 0) {
                memset(&ctx->shared_locals[frame->local_base], 0,
                       method->max_locals * sizeof(uint16_t));
            }
            
            /* TODO: Handle method parameters (pop from stack, store to locals) */
            /* For now, we assume no parameters */
            
            /* Set PC to method code */
            ctx->pc = method_code;
            ctx->code_start = method_code;
            ctx->code_length = method->code_length;
            
            break;
        }
        
        case OP_RETURN: {
            CallFrame* frame;
            
            /* Check if this is main method return */
            if (ctx->call_depth == 0) {
                ctx->running = 0;
                return 1;
            }
            
            /* Get call frame */
            ctx->call_depth--;
            frame = &ctx->call_frames[ctx->call_depth];
            
            /* Restore stack pointer (discard current frame's stack) */
            ctx->stack_pointer = frame->frame_pointer;
            
            /* Restore local pointer (free current frame's locals) */
            ctx->local_pointer = frame->local_base;
            
            /* Restore PC and code context */
            ctx->pc = frame->return_pc;
            ctx->code_start = frame->return_code_start;
            ctx->code_length = frame->return_code_length;
            
            /* Note: code_start and code_length are not restored */
            /* This is OK because PC is absolute */
            
            break;
        }
        
        case OP_RETURN_VALUE: {
            CallFrame* frame;
            uint16_t return_value;
            
            /* Pop return value */
            return_value = stack_pop_shared(ctx);
            
            /* Check if this is main method return */
            if (ctx->call_depth == 0) {
                ctx->running = 0;
                return 1;
            }
            
            /* Get call frame */
            ctx->call_depth--;
            frame = &ctx->call_frames[ctx->call_depth];
            
            /* Restore stack pointer */
            ctx->stack_pointer = frame->frame_pointer;
            
            /* Push return value onto caller's stack */
            if (stack_push_shared(ctx, return_value) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            
            /* Restore local pointer */
            ctx->local_pointer = frame->local_base;
            
            /* Restore PC */
            ctx->pc = frame->return_pc;
            
            break;
        }
        
        case OP_HALT:
            /* Halt execution */
            ctx->running = 0;
            return 1;
        
        default:
            printf("ERROR: Unknown opcode: 0x%02X at PC offset %u\n", opcode,
                   (uint16_t)(ctx->pc - ctx->code_start - 1));
            return -1;
    }
    
    
    return 0;
}

/**
 * Execute a method
 */
int interpreter_execute(ExecutionContext* ctx) {
    int result;
    
    if (ctx == NULL) {
        return -1;
    }
    
    /* Execute until return or error */
    while (ctx->running) {
        uint16_t pc_offset = (uint16_t)(ctx->pc - ctx->code_start);
        uint8_t next_opcode = (ctx->pc < ctx->code_start + ctx->code_length) ? *ctx->pc : 0xFF;
        
        
        
        result = interpreter_step(ctx);
        
        
        
        if (result != 0) {
            
            return result;
        }
    }
    
    
    return 0;
}

/**
 * Print execution context state (for debugging)
 */
void interpreter_print_state(ExecutionContext* ctx) {
    uint16_t i;
    uint16_t offset;
    uint16_t local_base;
    uint8_t local_count;
    
    if (ctx == NULL) {
        printf("Context: NULL\n");
        return;
    }
    
    printf("Execution Context:\n");
    printf("  Running: %s\n", ctx->running ? "yes" : "no");
    printf("  Call Depth: %u / %u\n", ctx->call_depth, MAX_CALL_DEPTH);
    
    offset = (uint16_t)(ctx->pc - ctx->code_start);
    printf("  PC: %u / %u\n", offset, ctx->code_length);
    
    /* Print current frame's locals */
    local_base = get_local_base(ctx);
    if (ctx->call_depth > 0) {
        local_count = ctx->call_frames[ctx->call_depth - 1].local_count;
    } else {
        local_count = ctx->local_pointer;
    }
    
    printf("  Locals (base=%u, count=%u):\n", local_base, local_count);
    for (i = 0; i < local_count && i < 10; i++) {
        printf("    [%u] = %u\n", i, ctx->shared_locals[local_base + i]);
    }
    if (local_count > 10) {
        printf("    ... (%u more)\n", local_count - 10);
    }
    
    /* Print shared stack */
    printf("  Stack (pointer=%u / %u):\n", ctx->stack_pointer, SHARED_STACK_SIZE);
    for (i = 0; i < ctx->stack_pointer && i < 10; i++) {
        printf("    [%u] = %u\n", i, ctx->shared_stack[i]);
    }
    if (ctx->stack_pointer > 10) {
        printf("    ... (%u more)\n", ctx->stack_pointer - 10);
    }
}


