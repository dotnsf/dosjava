#include "interpreter.h"
#include "memory.h"
#include "../format/opcodes.h"
#include "../runtime/system.h"
#include <stdio.h>
#include <string.h>

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
    
    /* Allocate operand stack */
    ctx->stack = (Stack*)memory_alloc(sizeof(Stack));
    if (ctx->stack == NULL) {
        return -1;
    }
    
    if (stack_init(ctx->stack, method->max_stack) != 0) {
        memory_free(ctx->stack);
        return -1;
    }
    
    /* Allocate local variables */
    ctx->local_count = method->max_locals;
    if (ctx->local_count > 0) {
        ctx->locals = (uint16_t*)memory_alloc(sizeof(uint16_t) * ctx->local_count);
        if (ctx->locals == NULL) {
            stack_free(ctx->stack);
            memory_free(ctx->stack);
            return -1;
        }
        
        /* Initialize locals to 0 */
        memset(ctx->locals, 0, sizeof(uint16_t) * ctx->local_count);
    }
    
    return 0;
}

/**
 * Free execution context resources
 */
void interpreter_free_context(ExecutionContext* ctx) {
    if (ctx == NULL) {
        return;
    }
    
    if (ctx->stack != NULL) {
        stack_free(ctx->stack);
        memory_free(ctx->stack);
        ctx->stack = NULL;
    }
    
    if (ctx->locals != NULL) {
        memory_free(ctx->locals);
        ctx->locals = NULL;
    }
}

/**
 * Get local variable
 */
uint16_t interpreter_get_local(ExecutionContext* ctx, uint16_t index) {
    if (ctx == NULL || ctx->locals == NULL || index >= ctx->local_count) {
        return 0;
    }
    
    return ctx->locals[index];
}

/**
 * Set local variable
 */
void interpreter_set_local(ExecutionContext* ctx, uint16_t index, uint16_t value) {
    if (ctx == NULL || ctx->locals == NULL || index >= ctx->local_count) {
        return;
    }
    
    ctx->locals[index] = value;
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
        
        case OP_PUSH_CONST:
            /* Push constant from pool */
            index16 = interpreter_read_u16(ctx);
            /* TODO: Get constant from pool and push */
            stack_push(ctx->stack, index16);
            break;
        
        case OP_PUSH_INT:
            /* Push immediate integer */
            value1 = interpreter_read_u16(ctx);
            stack_push(ctx->stack, value1);
            break;
        
        case OP_POP:
            /* Pop and discard */
            stack_pop(ctx->stack);
            break;
        
        case OP_DUP:
            /* Duplicate top of stack */
            stack_dup(ctx->stack);
            break;
        
        case OP_LOAD_LOCAL:
            /* Load local variable */
            index8 = interpreter_read_u8(ctx);
            value1 = interpreter_get_local(ctx, index8);
            stack_push(ctx->stack, value1);
            break;
        
        case OP_STORE_LOCAL:
            /* Store to local variable */
            index8 = interpreter_read_u8(ctx);
            value1 = stack_pop(ctx->stack);
            interpreter_set_local(ctx, index8, value1);
            break;
        
        case OP_LOAD_0:
            value1 = interpreter_get_local(ctx, 0);
            stack_push(ctx->stack, value1);
            break;
        
        case OP_LOAD_1:
            value1 = interpreter_get_local(ctx, 1);
            stack_push(ctx->stack, value1);
            break;
        
        case OP_LOAD_2:
            value1 = interpreter_get_local(ctx, 2);
            stack_push(ctx->stack, value1);
            break;
        
        case OP_STORE_0:
            value1 = stack_pop(ctx->stack);
            interpreter_set_local(ctx, 0, value1);
            break;
        
        case OP_STORE_1:
            value1 = stack_pop(ctx->stack);
            interpreter_set_local(ctx, 1, value1);
            break;
        
        case OP_STORE_2:
            value1 = stack_pop(ctx->stack);
            interpreter_set_local(ctx, 2, value1);
            break;
        
        case OP_ADD:
            /* Integer addition */
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            result = value1 + value2;
            stack_push(ctx->stack, result);
            break;
        
        case OP_SUB:
            /* Integer subtraction */
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            result = value1 - value2;
            stack_push(ctx->stack, result);
            break;
        
        case OP_MUL:
            /* Integer multiplication */
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            result = value1 * value2;
            stack_push(ctx->stack, result);
            break;
        
        case OP_DIV:
            /* Integer division */
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            if (value2 == 0) {
                printf("ERROR: Division by zero\n");
                return -1;
            }
            result = value1 / value2;
            stack_push(ctx->stack, result);
            break;
        
        case OP_MOD:
            /* Integer modulo */
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            if (value2 == 0) {
                printf("ERROR: Modulo by zero\n");
                return -1;
            }
            result = value1 % value2;
            stack_push(ctx->stack, result);
            break;
        
        case OP_NEG:
            /* Negate */
            value1 = stack_pop(ctx->stack);
            result = (uint16_t)(-(int16_t)value1);
            stack_push(ctx->stack, result);
            break;
        
        case OP_INC_LOCAL:
            /* Increment local variable */
            index8 = interpreter_read_u8(ctx);
            value1 = (uint16_t)((int8_t)interpreter_read_u8(ctx));
            value2 = interpreter_get_local(ctx, index8);
            result = value2 + value1;
            interpreter_set_local(ctx, index8, result);
            break;
        
        case OP_CMP_EQ:
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            stack_push(ctx->stack, (value1 == value2) ? 1 : 0);
            break;
        
        case OP_CMP_NE:
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            stack_push(ctx->stack, (value1 != value2) ? 1 : 0);
            break;
        
        case OP_CMP_LT:
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            stack_push(ctx->stack, ((int16_t)value1 < (int16_t)value2) ? 1 : 0);
            break;
        
        case OP_CMP_LE:
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            stack_push(ctx->stack, ((int16_t)value1 <= (int16_t)value2) ? 1 : 0);
            break;
        
        case OP_CMP_GT:
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            stack_push(ctx->stack, ((int16_t)value1 > (int16_t)value2) ? 1 : 0);
            break;
        
        case OP_CMP_GE:
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            stack_push(ctx->stack, ((int16_t)value1 >= (int16_t)value2) ? 1 : 0);
            break;
        
        case OP_GOTO:
            /* Unconditional jump */
            offset = (int16_t)interpreter_read_u16(ctx);
            ctx->pc = ctx->code_start + offset;
            break;
        
        case OP_IF_TRUE:
            /* Jump if true */
            offset = (int16_t)interpreter_read_u16(ctx);
            value1 = stack_pop(ctx->stack);
            if (value1 != 0) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_FALSE:
            /* Jump if false */
            offset = (int16_t)interpreter_read_u16(ctx);
            value1 = stack_pop(ctx->stack);
            if (value1 == 0) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_EQ:
            /* Jump if equal */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            if (value1 == value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_NE:
            /* Jump if not equal */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            if (value1 != value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_LT:
            /* Jump if less than */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            if ((int16_t)value1 < (int16_t)value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_LE:
            /* Jump if less or equal */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            if ((int16_t)value1 <= (int16_t)value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_GT:
            /* Jump if greater than */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            if ((int16_t)value1 > (int16_t)value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_IF_GE:
            /* Jump if greater or equal */
            offset = (int16_t)interpreter_read_u16(ctx);
            value2 = stack_pop(ctx->stack);
            value1 = stack_pop(ctx->stack);
            if ((int16_t)value1 >= (int16_t)value2) {
                ctx->pc = ctx->code_start + offset;
            }
            break;
        
        case OP_PRINT_INT:
            /* Debug: print integer */
            value1 = stack_pop(ctx->stack);
            system_print_int((int16_t)value1);
            break;
        
        case OP_PRINT_CHAR:
            /* Debug: print character */
            value1 = stack_pop(ctx->stack);
            system_print_char((char)value1);
            break;
        
        case 0x0D:
        case 0x0E:
        case 0x0F:
            /* Return void (legacy opcodes for compatibility) */
            ctx->running = 0;
            return 1;
        
        case OP_RETURN:
            /* Return void */
            ctx->running = 0;
            return 1;
        
        case OP_RETURN_VALUE:
            /* Return with value */
            ctx->running = 0;
            return 1;
        
        case OP_HALT:
            /* Halt execution */
            ctx->running = 0;
            return 1;
        
        default:
            printf("ERROR: Unknown opcode: 0x%02X\n", opcode);
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
    
    if (ctx == NULL) {
        printf("Context: NULL\n");
        return;
    }
    
    printf("Execution Context:\n");
    printf("  Running: %s\n", ctx->running ? "yes" : "no");
    
    offset = (uint16_t)(ctx->pc - ctx->code_start);
    printf("  PC: %u / %u\n", offset, ctx->code_length);
    
    printf("  Locals (%u):\n", ctx->local_count);
    for (i = 0; i < ctx->local_count && i < 10; i++) {
        printf("    [%u] = %u\n", i, ctx->locals[i]);
    }
    if (ctx->local_count > 10) {
        printf("    ... (%u more)\n", ctx->local_count - 10);
    }
    
    printf("  Stack:\n");
    stack_print(ctx->stack);
}

// Made with Bob
