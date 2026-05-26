#include "interpreter.h"
#include "memory.h"
#include "native.h"
#include "../format/opcodes.h"
#include "../runtime/system.h"
#include "../runtime/fileinputstream.h"
#include "../runtime/fileoutputstream.h"
#include "../runtime/bufferedreader.h"
#include "../runtime/bufferedwriter.h"
#include "../runtime/date.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Global debug flag (set by djvm.c or default to 0) */
/* Declared in djvm.c */
extern int g_debug_mode;

/* Global file handle for File I/O operations */
static FILE* g_file_handle = NULL;

static uint8_t descriptor_param_count(const char* descriptor) {
    uint8_t count = 0;
    const char* p;
    
    if (!descriptor || descriptor[0] != '(') {
        return 0xFF;
    }
    
    p = descriptor + 1;
    while (*p && *p != ')') {
        if (*p == 'I') {
            count++;
            p++;
        } else if (*p == 'L') {
            count++;
            while (*p && *p != ';') {
                p++;
            }
            if (*p == ';') {
                p++;
            } else {
                return 0xFF;
            }
        } else {
            return 0xFF;
        }
    }
    
    if (*p != ')') {
        return 0xFF;
    }
    
    return count;
}

static char descriptor_return_type(const char* descriptor) {
    const char* p;
    
    if (!descriptor) {
        return '\0';
    }
    
    p = strchr(descriptor, ')');
    if (!p || p[1] == '\0') {
        return '\0';
    }
    
    return p[1];
}

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

/* ===== Long Type Stack Helper Functions ===== */

/**
 * Push 32-bit long value onto stack as two 16-bit words
 * Stack layout: [... | high | low | ...] (low is top)
 *
 * @param ctx Execution context
 * @param value 32-bit long value
 * @return 0 on success, -1 on stack overflow
 */
static inline int stack_push_long(ExecutionContext* ctx, uint32_t value) {
    uint16_t high;
    uint16_t low;
    
    high = (uint16_t)(value >> 16);
    low = (uint16_t)(value & 0xFFFF);
    
    /* Push high word first, then low word */
    if (stack_push_shared(ctx, high) != 0) {
        return -1;  /* Stack overflow */
    }
    if (stack_push_shared(ctx, low) != 0) {
        /* Rollback: pop the high word we just pushed */
        ctx->stack_pointer--;
        return -1;  /* Stack overflow */
    }
    return 0;
}

/**
 * Pop 32-bit long value from stack (two 16-bit words)
 * Stack layout: [... | high | low | ...] (low is top)
 *
 * @param ctx Execution context
 * @return 32-bit long value
 */
static inline uint32_t stack_pop_long(ExecutionContext* ctx) {
    uint16_t low;
    uint16_t high;
    
    low = stack_pop_shared(ctx);   /* Pop low word first */
    high = stack_pop_shared(ctx);  /* Then pop high word */
    return ((uint32_t)high << 16) | low;
}

/**
 * Peek at 32-bit long value on top of stack (without popping)
 * Stack layout: [... | high | low | ...] (low is top)
 *
 * @param ctx Execution context
 * @return 32-bit long value
 */
static inline uint32_t stack_peek_long(ExecutionContext* ctx) {
    uint16_t low;
    uint16_t high;
    
    if (ctx->stack_pointer < 2) {
        return 0;  /* Stack underflow */
    }
    low = ctx->shared_stack[ctx->stack_pointer - 1];
    high = ctx->shared_stack[ctx->stack_pointer - 2];
    return ((uint32_t)high << 16) | low;
}

/* ===== Float Type Stack Helper Functions ===== */

/**
 * Push float value onto shared stack as two 16-bit words
 * Float is stored as IEEE 754 32-bit: [high 16 bits] [low 16 bits]
 * Stack layout: [... | high | low | ...] (low is top)
 *
 * @param ctx Execution context
 * @param value Float value
 * @return 0 on success, -1 on stack overflow
 */
int stack_push_float_shared(ExecutionContext* ctx, float value) {
    uint32_t bits;
    uint16_t high, low;
    
    /* Convert float to 32-bit representation */
    memcpy(&bits, &value, sizeof(float));
    
    /* Split into high and low 16-bit words */
    high = (uint16_t)(bits >> 16);
    low = (uint16_t)(bits & 0xFFFF);
    
    /* Push high word first, then low word */
    if (stack_push_shared(ctx, high) != 0) {
        return -1;  /* Stack overflow */
    }
    if (stack_push_shared(ctx, low) != 0) {
        /* Rollback: pop the high word we just pushed */
        ctx->stack_pointer--;
        return -1;  /* Stack overflow */
    }
    return 0;
}

/**
 * Pop float value from shared stack (two 16-bit words)
 * Float is stored as IEEE 754 32-bit: [high 16 bits] [low 16 bits]
 * Stack layout: [... | high | low | ...] (low is top)
 *
 * @param ctx Execution context
 * @return Float value
 */
float stack_pop_float_shared(ExecutionContext* ctx) {
    uint16_t low, high;
    uint32_t bits;
    float value;
    
    /* Pop low word first, then high word */
    low = stack_pop_shared(ctx);
    high = stack_pop_shared(ctx);
    
    /* Combine into 32-bit value */
    bits = ((uint32_t)high << 16) | low;
    
    /* Convert to float */
    memcpy(&value, &bits, sizeof(float));
    
    return value;
}

/**
 * Peek at float value on top of shared stack (without popping)
 * Float is stored as IEEE 754 32-bit: [high 16 bits] [low 16 bits]
 * Stack layout: [... | high | low | ...] (low is top)
 *
 * @param ctx Execution context
 * @param offset Offset in float units (0 = top, 1 = second, etc.)
 * @return Float value
 */
float stack_peek_float_shared(ExecutionContext* ctx, uint16_t offset) {
    uint16_t low, high;
    uint32_t bits;
    float value;
    uint16_t word_offset;
    
    /* Calculate word offset (each float is 2 words) */
    word_offset = offset * 2;
    
    /* Check bounds */
    if (ctx->stack_pointer < word_offset + 2) {
        return 0.0f;  /* Stack underflow */
    }
    
    /* Read low and high words without modifying SP */
    low = ctx->shared_stack[ctx->stack_pointer - 1 - word_offset];
    high = ctx->shared_stack[ctx->stack_pointer - 2 - word_offset];
    
    /* Combine into 32-bit value */
    bits = ((uint32_t)high << 16) | low;
    
    /* Convert to float */
    memcpy(&value, &bits, sizeof(float));
    
    return value;
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
    if (ctx->call_depth <= 1) {
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

/**
 * Load 32-bit long from local variables (2 consecutive slots)
 * Locals layout: [idx] = high word, [idx+1] = low word
 *
 * @param ctx Execution context
 * @param index Starting index (uses idx and idx+1)
 * @return 32-bit long value
 */
static inline uint32_t load_local_long(ExecutionContext* ctx, uint8_t index) {
    uint16_t base;
    uint16_t high;
    uint16_t low;
    uint32_t result;
    
    base = get_local_base(ctx);
    if (base + index + 1 >= SHARED_LOCALS_SIZE) {
        return 0;  /* Out of bounds */
    }
    high = ctx->shared_locals[base + index];
    low = ctx->shared_locals[base + index + 1];
    result = ((uint32_t)high << 16) | low;
    
    return result;
}

/**
 * Store 32-bit long to local variables (2 consecutive slots)
 * Locals layout: [idx] = high word, [idx+1] = low word
 *
 * @param ctx Execution context
 * @param index Starting index (uses idx and idx+1)
 * @param value 32-bit long value
 */
static inline void store_local_long(ExecutionContext* ctx, uint8_t index, uint32_t value) {
    uint16_t base;
    uint16_t high;
    uint16_t low;
    
    base = get_local_base(ctx);
    high = (uint16_t)(value >> 16);
    low = (uint16_t)(value & 0xFFFF);
    
    if (base + index + 1 < SHARED_LOCALS_SIZE) {
        ctx->shared_locals[base + index] = high;      /* High word */
        ctx->shared_locals[base + index + 1] = low;   /* Low word */
    }
}

/* ===== Exception Helper Functions ===== */

/**
 * Throw a runtime exception
 * If in try block, jump to catch block
 * Otherwise, print error and terminate
 * 
 * @param ctx Execution context
 * @param message Error message
 * @return 0 if exception handled (jumped to catch), -1 if should terminate
 */
static int throw_runtime_exception(ExecutionContext* ctx, const char* message) {
    if (ctx->in_try_block && ctx->catch_pc) {
        /* Jump to catch block */
        ctx->pc = ctx->catch_pc;
        ctx->in_try_block = 0;
        return 0;  /* Exception handled */
    } else {
        /* No try block - print error and terminate */
        printf("ERROR: %s\n", message);
        return -1;  /* Should terminate */
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
    
    /* Initialize exception handling state */
    ctx->catch_pc = NULL;
    ctx->in_try_block = 0;
    
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
    ctx->call_depth = 1;
    ctx->call_frames[0].return_pc = NULL;
    ctx->call_frames[0].return_code_start = ctx->code_start;
    ctx->call_frames[0].return_code_length = ctx->code_length;
    ctx->call_frames[0].frame_pointer = 0;
    ctx->call_frames[0].local_base = 0;
    ctx->call_frames[0].local_count = method->max_locals;
    
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
                /* Try to throw exception */
                if (throw_runtime_exception(ctx, "Division by zero") != 0) {
                    return -1;  /* Not in try block - terminate */
                }
                break;  /* Exception handled - jumped to catch block */
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
                /* Try to throw exception */
                if (throw_runtime_exception(ctx, "Modulo by zero") != 0) {
                    return -1;  /* Not in try block - terminate */
                }
                break;  /* Exception handled - jumped to catch block */
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
        
        case OP_NEW_ARRAY: {
            uint8_t elem_type;
            uint16_t size;
            uint16_t total_size;
            uint16_t* array_data;
            uint16_t array_handle;
            
            elem_type = interpreter_read_u8(ctx);
            size = stack_pop_shared(ctx);
            (void)elem_type;
            
            if ((int16_t)size < 0) {
                printf("ERROR: Negative array size\n");
                return -1;
            }
            
            if (size > 0x7FFF / sizeof(uint16_t) - 1) {
                printf("ERROR: Array too large\n");
                return -1;
            }
            
            total_size = (uint16_t)((size + 1) * sizeof(uint16_t));
            array_data = (uint16_t*)memory_alloc(total_size);
            if (array_data == NULL) {
                printf("ERROR: Out of memory allocating array\n");
                return -1;
            }
            
            memset(array_data, 0, total_size);
            array_data[0] = size;
            
            /* Allocate handle for array pointer */
            array_handle = memory_alloc_array_handle(array_data);
            if (array_handle == 0) {
                printf("ERROR: Out of array handles\n");
                memory_free(array_data);
                return -1;
            }
            
            if (stack_push_shared(ctx, array_handle) != 0) {
                printf("ERROR: Stack overflow\n");
                memory_free_array_handle(array_handle);
                memory_free(array_data);
                return -1;
            }
            break;
        }
        
        case OP_ARRAY_LENGTH: {
            uint16_t array_handle;
            uint16_t* array_data;
            
            array_handle = stack_pop_shared(ctx);
            array_data = (uint16_t*)memory_get_array_ptr(array_handle);
            if (array_data == NULL) {
                printf("ERROR: Null array reference (ARRAY_LENGTH)\n");
                return -1;
            }
            
            if (stack_push_shared(ctx, array_data[0]) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_ARRAY_LOAD: {
            uint16_t array_handle;
            uint16_t index;
            uint16_t* array_data;
            
            index = stack_pop_shared(ctx);
            array_handle = stack_pop_shared(ctx);
            array_data = (uint16_t*)memory_get_array_ptr(array_handle);
            if (array_data == NULL) {
                printf("ERROR: Null array reference (ARRAY_LOAD idx=%u)\n", index);
                return -1;
            }
            if (index >= array_data[0]) {
                printf("ERROR: Array index out of bounds\n");
                return -1;
            }
            
            if (stack_push_shared(ctx, array_data[index + 1]) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_ARRAY_STORE: {
            uint16_t array_handle;
            uint16_t value;
            uint16_t index;
            uint16_t* array_data;
            
            value = stack_pop_shared(ctx);
            index = stack_pop_shared(ctx);
            array_handle = stack_pop_shared(ctx);
            array_data = (uint16_t*)memory_get_array_ptr(array_handle);
            if (array_data == NULL) {
                printf("ERROR: Null array reference (ARRAY_STORE idx=%u val=%u)\n", index, value);
                return -1;
            }
            if (index >= array_data[0]) {
                printf("ERROR: Array index out of bounds (idx=%u len=%u val=%u)\n",
                    index, array_data[0], value);
                return -1;
            }
            
            array_data[index + 1] = value;
            if (stack_push_shared(ctx, value) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
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
            uint8_t arg_count;
            DJCMethod* method;
            CallFrame* frame;
            uint8_t* method_code;
            const char* method_name;
            
            /* Read method index (2 bytes, little-endian) and argument count */
            method_index = interpreter_read_u16(ctx);
            arg_count = interpreter_read_u8(ctx);
            
            
            /* Look up method */
            method = djc_find_method(ctx->djc_file, method_index);
            if (method == NULL) {
                printf("ERROR: Method not found (index: %d)\n", method_index);
                return -1;
            }
            
            /* Get method name */
            method_name = djc_get_utf8(ctx->djc_file, method->name_index);
            
            {
                const char* descriptor = djc_get_utf8(ctx->djc_file, method->descriptor_index);
                uint8_t expected_arg_count = descriptor_param_count(descriptor);
                if (descriptor && expected_arg_count != 0xFF && expected_arg_count != arg_count) {
                    printf("ERROR: Method descriptor argument mismatch for %s\n",
                           method_name ? method_name : "???");
                    printf("  Descriptor: %s\n", descriptor ? descriptor : "NULL");
                    printf("  Expected args: %d, Actual args: %d\n", expected_arg_count, arg_count);
                    return -1;
                }
            }
            
            /* Check if method is native */
            if (method->flags & METHOD_NATIVE) {
                /* Handle native methods using native method mechanism */
                const char* descriptor;
                NativeMethodDescriptor* native_method;
                uint16_t args[8];  /* Support up to 8 arguments */
                uint16_t result[2];  /* Support up to 2-word return values (for float) */
                uint8_t i;
                int invoke_result;
                
                /* Get method descriptor */
                descriptor = djc_get_utf8(ctx->djc_file, method->descriptor_index);
                
                /* Find native method in registry */
                native_method = native_find(NULL, method_name, descriptor);
                
                if (native_method != NULL) {
                    /* Removed special handling for println(int) with 2 words on stack
                     * This was causing issues when float variables were used before println(int)
                     * Long values should use println(long) with descriptor (J)V instead
                     */
                    
                    
                    /* Pop arguments from stack (in reverse order) */
                    if (arg_count > 8) {
                        printf("ERROR: Too many arguments for native method: %u\n", arg_count);
                        return -1;
                    }
                    
                    for (i = 0; i < arg_count; i++) {
                        args[arg_count - 1 - i] = stack_pop_shared(ctx);
                    }
                    
                    /* Invoke native method */
                    result[0] = 0;
                    result[1] = 0;
                    invoke_result = native_invoke(ctx, native_method, args, arg_count, result);
                    
                    if (invoke_result != 0) {
                        printf("ERROR: Native method %s failed\n", method_name ? method_name : "???");
                        return -1;
                    }
                    
                    /* Push result if method returns a value */
                    if (native_method->return_type != NATIVE_RETURN_VOID) {
                        if (native_method->return_type == NATIVE_RETURN_FLOAT) {
                            /* Float return value: 2 words [high, low] */
                            if (stack_push_shared(ctx, result[0]) != 0) {
                                printf("ERROR: Stack overflow\n");
                                return -1;
                            }
                            if (stack_push_shared(ctx, result[1]) != 0) {
                                printf("ERROR: Stack overflow\n");
                                return -1;
                            }
                        } else {
                            /* Single word return value */
                            if (stack_push_shared(ctx, result[0]) != 0) {
                                printf("ERROR: Stack overflow\n");
                                return -1;
                            }
                        }
                    }
                    
                    break;
                }
                
                /* Fallback: Handle legacy native methods not yet migrated to native mechanism */
                if (method_name) {
                    /* Check for System.out.println and System.out.print */
                    if (strcmp(method_name, "println") == 0 || strcmp(method_name, "print") == 0) {
                        /* Pop value from stack */
                        uint16_t value;
                        const char* str;
                        const char* descriptor;
                        int is_println;
                        
                        is_println = (strcmp(method_name, "println") == 0);
                        
                        /* Get method descriptor to determine parameter type */
                        descriptor = djc_get_utf8(ctx->djc_file, method->descriptor_index);
                        
                        /* Check descriptor for parameter type:
                         * (Ljava/lang/String;)V - String parameter
                         * (I)V - int parameter
                         * (J)V - long parameter
                         */
                        if (descriptor && strcmp(descriptor, "(J)V") == 0) {
                            /* Long parameter - pop 2 words [low, high] */
                            uint16_t low, high;
                            uint32_t long_value;
                            
                            low = stack_pop_shared(ctx);
                            high = stack_pop_shared(ctx);
                            long_value = ((uint32_t)high << 16) | low;
                            
                            if (is_println) {
                                printf("%ld\n", (long)long_value);
                            } else {
                                printf("%ld", (long)long_value);
                            }
                        } else if (descriptor && strcmp(descriptor, "(Ljava/lang/String;)V") == 0) {
                            value = stack_pop_shared(ctx);
                            /* String parameter - value is direct UTF8 constant index */
                            str = NULL;
                            if (value < ctx->djc_file->header.constant_pool_count) {
                                if (ctx->djc_file->constants[value].tag == CONST_UTF8) {
                                    str = ctx->djc_file->constants[value].data.utf8_data;
                                }
                            }
                            if (str) {
                                if (is_println) {
                                    system_println_cstr(str);
                                } else {
                                    printf("%s", str);
                                }
                            } else {
                                printf("ERROR: Invalid string constant index: %d\n", value);
                                return -1;
                            }
                        } else {
                            /* Native print/println(int) and any non-string fallback */
                            value = stack_pop_shared(ctx);
                            if (is_println) {
                                system_println_int((int16_t)value);
                            } else {
                                printf("%d", (int16_t)value);
                            }
                        }
                        break;
                    } else if (strcmp(method_name, "length") == 0) {
                        uint16_t value;
                        const char* str;
                        uint16_t len;
                        
                        if (arg_count != 1) {
                            printf("ERROR: length expects 1 argument, got %u\n", arg_count);
                            return -1;
                        }
                        
                        value = stack_pop_shared(ctx);
                        str = NULL;
                        
                        /* Phase 1 String values are stored as direct UTF8 constant indices,
                         * identical to println(String) handling.
                         */
                        if (value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[value].tag == CONST_UTF8) {
                                str = ctx->djc_file->constants[value].data.utf8_data;
                            }
                        }
                        
                        if (!str) {
                            printf("ERROR: Invalid string constant index for length: %d\n", value);
                            return -1;
                        }
                        
                        len = (uint16_t)strlen(str);
                        if (stack_push_shared(ctx, len) != 0) {
                            printf("ERROR: Stack overflow\n");
                            return -1;
                        }
                        break;
                    } else if (strcmp(method_name, "toUpperCase") == 0 ||
                               strcmp(method_name, "toLowerCase") == 0) {
                        uint16_t value;
                        const char* src_str;
                        char conv_buf[256];
                        uint16_t len;
                        uint16_t i;
                        uint16_t const_idx;
                        int to_upper;
                        
                        if (arg_count != 1) {
                            printf("ERROR: %s expects 1 argument, got %u\n", method_name, arg_count);
                            return -1;
                        }
                        
                        value = stack_pop_shared(ctx);
                        src_str = NULL;
                        
                        if (value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[value].tag == CONST_UTF8) {
                                src_str = ctx->djc_file->constants[value].data.utf8_data;
                            }
                        }
                        
                        if (!src_str) {
                            printf("ERROR: Invalid string constant index for %s: %d\n", method_name, value);
                            return -1;
                        }
                        
                        len = (uint16_t)strlen(src_str);
                        if (len >= sizeof(conv_buf)) {
                            printf("ERROR: String too long for %s\n", method_name);
                            return -1;
                        }
                        
                        to_upper = (strcmp(method_name, "toUpperCase") == 0);
                        for (i = 0; i < len; i++) {
                            char ch = src_str[i];
                            if (to_upper) {
                                if (ch >= 'a' && ch <= 'z') {
                                    ch = (char)(ch - ('a' - 'A'));
                                }
                            } else {
                                if (ch >= 'A' && ch <= 'Z') {
                                    ch = (char)(ch + ('a' - 'A'));
                                }
                            }
                            conv_buf[i] = ch;
                        }
                        conv_buf[len] = '\0';
                        
                        const_idx = ctx->djc_file->header.constant_pool_count;
                        if (const_idx >= DJC_MAX_CONSTANTS) {
                            printf("ERROR: Constant pool full during %s\n", method_name);
                            return -1;
                        }
                        
                        ctx->djc_file->constants[const_idx].tag = CONST_UTF8;
                        ctx->djc_file->constants[const_idx].length = len;
                        ctx->djc_file->constants[const_idx].data.utf8_data = (char*)memory_alloc(len + 1);
                        if (ctx->djc_file->constants[const_idx].data.utf8_data == NULL) {
                            printf("ERROR: Out of memory during %s\n", method_name);
                            return -1;
                        }
                        strcpy(ctx->djc_file->constants[const_idx].data.utf8_data, conv_buf);
                        ctx->djc_file->header.constant_pool_count++;
                        
                        if (stack_push_shared(ctx, const_idx) != 0) {
                            printf("ERROR: Stack overflow\n");
                            return -1;
                        }
                        break;
                    } else if (strcmp(method_name, "concat") == 0) {
                        uint16_t right_value;
                        uint16_t left_value;
                        const char* left_str;
                        const char* right_str;
                        char concat_buf[256];
                        uint16_t left_len;
                        uint16_t right_len;
                        uint16_t total_len;
                        uint16_t i;
                        uint16_t j;
                        uint16_t const_idx;
                        
                        if (arg_count != 2) {
                            printf("ERROR: concat expects 2 arguments, got %u\n", arg_count);
                            return -1;
                        }
                        
                        right_value = stack_pop_shared(ctx);
                        left_value = stack_pop_shared(ctx);
                        
                        if (g_debug_mode) {
                            printf("[DEBUG] concat: left_value=%u, right_value=%u\n", left_value, right_value);
                            printf("[DEBUG] concat: constant_pool_count=%u\n", ctx->djc_file->header.constant_pool_count);
                        }
                        
                        left_str = NULL;
                        right_str = NULL;
                        
                        if (left_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[left_value].tag == CONST_UTF8) {
                                left_str = ctx->djc_file->constants[left_value].data.utf8_data;
                            }
                        }
                        if (right_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[right_value].tag == CONST_UTF8) {
                                right_str = ctx->djc_file->constants[right_value].data.utf8_data;
                            }
                        }
                        
                        if (g_debug_mode) {
                            printf("[DEBUG] concat: left_str=%s, right_str=%s\n",
                                   left_str ? left_str : "NULL",
                                   right_str ? right_str : "NULL");
                        }
                        
                        if (!left_str || !right_str) {
                            printf("ERROR: Invalid string constant index for concat: %d, %d\n",
                                   left_value, right_value);
                            if (left_value < ctx->djc_file->header.constant_pool_count) {
                                printf("  left constant tag: %d\n", ctx->djc_file->constants[left_value].tag);
                            }
                            if (right_value < ctx->djc_file->header.constant_pool_count) {
                                printf("  right constant tag: %d\n", ctx->djc_file->constants[right_value].tag);
                            }
                            return -1;
                        }
                        
                        
                        left_len = (uint16_t)strlen(left_str);
                        right_len = (uint16_t)strlen(right_str);
                        total_len = left_len + right_len;
                        if (total_len >= sizeof(concat_buf)) {
                            printf("ERROR: Concatenated string too long\n");
                            return -1;
                        }
                        
                        for (i = 0; i < left_len; i++) {
                            concat_buf[i] = left_str[i];
                        }
                        for (j = 0; j < right_len; j++) {
                            concat_buf[left_len + j] = right_str[j];
                        }
                        concat_buf[total_len] = '\0';
                        
                        const_idx = ctx->djc_file->header.constant_pool_count;
                        if (const_idx >= DJC_MAX_CONSTANTS) {
                            printf("ERROR: Constant pool full during concat\n");
                            return -1;
                        }
                        
                        ctx->djc_file->constants[const_idx].tag = CONST_UTF8;
                        ctx->djc_file->constants[const_idx].length = total_len;
                        ctx->djc_file->constants[const_idx].data.utf8_data = (char*)memory_alloc(total_len + 1);
                        if (ctx->djc_file->constants[const_idx].data.utf8_data == NULL) {
                            printf("ERROR: Out of memory during concat\n");
                            return -1;
                        }
                        strcpy(ctx->djc_file->constants[const_idx].data.utf8_data, concat_buf);
                        ctx->djc_file->header.constant_pool_count++;
                        
                        if (stack_push_shared(ctx, const_idx) != 0) {
                            printf("ERROR: Stack overflow\n");
                            return -1;
                        }
                        break;
                    } else if (strcmp(method_name, "startsWith") == 0 ||
                               strcmp(method_name, "endsWith") == 0) {
                        uint16_t cmp_value;
                        uint16_t str_value;
                        const char* str;
                        const char* cmp_str;
                        uint16_t str_len;
                        uint16_t cmp_len;
                        int result;
                        int is_starts;
                        
                        if (arg_count != 2) {
                            printf("ERROR: %s expects 2 arguments, got %u\n", method_name, arg_count);
                            return -1;
                        }
                        
                        /* Pop comparison string and target string from stack */
                        cmp_value = stack_pop_shared(ctx);
                        str_value = stack_pop_shared(ctx);
                        str = NULL;
                        cmp_str = NULL;
                        
                        /* Get target string from constant pool */
                        if (str_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
                                str = ctx->djc_file->constants[str_value].data.utf8_data;
                            }
                        }
                        
                        /* Get comparison string from constant pool */
                        if (cmp_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[cmp_value].tag == CONST_UTF8) {
                                cmp_str = ctx->djc_file->constants[cmp_value].data.utf8_data;
                            }
                        }
                        
                        if (!str || !cmp_str) {
                            printf("ERROR: Invalid string constant index for %s: %d, %d\n",
                                   method_name, str_value, cmp_value);
                            return -1;
                        }
                        
                        str_len = (uint16_t)strlen(str);
                        cmp_len = (uint16_t)strlen(cmp_str);
                        
                        is_starts = (strcmp(method_name, "startsWith") == 0);
                        result = 0;
                        
                        if (is_starts) {
                            /* startsWith: check if str begins with cmp_str */
                            if (cmp_len <= str_len) {
                                if (strncmp(str, cmp_str, cmp_len) == 0) {
                                    result = 1;
                                }
                            }
                        } else {
                            /* endsWith: check if str ends with cmp_str */
                            if (cmp_len <= str_len) {
                                if (strcmp(str + (str_len - cmp_len), cmp_str) == 0) {
                                    result = 1;
                                }
                            }
                        }
                        
                        /* Push result (0 or 1) to stack */
                        if (stack_push_shared(ctx, (uint16_t)result) != 0) {
                            printf("ERROR: Stack overflow\n");
                            return -1;
                        }
                        break;
                    } else if (strcmp(method_name, "equals") == 0) {
                        uint16_t arg_value;
                        uint16_t str_value;
                        const char* str;
                        const char* arg_str;
                        int result;
                        
                        if (arg_count != 2) {
                            printf("ERROR: equals expects 2 arguments, got %u\n", arg_count);
                            return -1;
                        }
                        
                        /* Pop argument and receiver from stack */
                        arg_value = stack_pop_shared(ctx);
                        str_value = stack_pop_shared(ctx);
                        
                        str = NULL;
                        arg_str = NULL;
                        
                        /* Get receiver string from constant pool */
                        if (str_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
                                str = ctx->djc_file->constants[str_value].data.utf8_data;
                            }
                        }
                        
                        /* Get argument string from constant pool */
                        if (arg_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[arg_value].tag == CONST_UTF8) {
                                arg_str = ctx->djc_file->constants[arg_value].data.utf8_data;
                            }
                        }
                        
                        if (!str || !arg_str) {
                            printf("ERROR: Invalid string constant for equals: %d, %d\n",
                                   str_value, arg_value);
                            return -1;
                        }
                        
                        /* Compare strings (case-sensitive) */
                        result = (strcmp(str, arg_str) == 0) ? 1 : 0;
                        
                        /* Push result (0 or 1) to stack */
                        if (stack_push_shared(ctx, (uint16_t)result) != 0) {
                            printf("ERROR: Stack overflow\n");
                            return -1;
                        }
                        break;
                    } else if (strcmp(method_name, "compareTo") == 0) {
                        uint16_t arg_value;
                        uint16_t str_value;
                        const char* str;
                        const char* arg_str;
                        int result;
                        
                        if (arg_count != 2) {
                            printf("ERROR: compareTo expects 2 arguments, got %u\n", arg_count);
                            return -1;
                        }
                        
                        /* Pop argument and receiver from stack */
                        arg_value = stack_pop_shared(ctx);
                        str_value = stack_pop_shared(ctx);
                        
                        str = NULL;
                        arg_str = NULL;
                        
                        /* Get receiver string from constant pool */
                        if (str_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
                                str = ctx->djc_file->constants[str_value].data.utf8_data;
                            }
                        }
                        
                        /* Get argument string from constant pool */
                        if (arg_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[arg_value].tag == CONST_UTF8) {
                                arg_str = ctx->djc_file->constants[arg_value].data.utf8_data;
                            }
                        }
                        
                        if (!str || !arg_str) {
                            printf("ERROR: Invalid string constant for compareTo: %d, %d\n",
                                   str_value, arg_value);
                            return -1;
                        }
                        
                        /* Compare strings lexicographically (case-sensitive) */
                        result = strcmp(str, arg_str);
                        
                        /* Push result to stack */
                        if (stack_push_shared(ctx, (uint16_t)result) != 0) {
                            printf("ERROR: Stack overflow\n");
                            return -1;
                        }
                        break;
                    } else if (strcmp(method_name, "indexOf") == 0) {
                        uint16_t search_value;
                        uint16_t str_value;
                        uint16_t from_index;
                        const char* str;
                        const char* search_str;
                        const char* found_pos;
                        uint16_t str_len;
                        int result;
                        int has_from_index;
                        
                        has_from_index = (arg_count == 3);
                        
                        if (arg_count != 2 && arg_count != 3) {
                            printf("ERROR: indexOf expects 2 or 3 arguments, got %u\n", arg_count);
                            return -1;
                        }
                        
                        /* Pop arguments from stack */
                        if (has_from_index) {
                            from_index = stack_pop_shared(ctx);
                        } else {
                            from_index = 0;
                        }
                        search_value = stack_pop_shared(ctx);
                        str_value = stack_pop_shared(ctx);
                        
                        str = NULL;
                        search_str = NULL;
                        
                        /* Get target string from constant pool */
                        if (str_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
                                str = ctx->djc_file->constants[str_value].data.utf8_data;
                            }
                        }
                        
                        /* Get search string from constant pool */
                        if (search_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[search_value].tag == CONST_UTF8) {
                                search_str = ctx->djc_file->constants[search_value].data.utf8_data;
                            }
                        }
                        
                        if (!str || !search_str) {
                            printf("ERROR: Invalid string constant index for indexOf: %d, %d\n",
                                   str_value, search_value);
                            return -1;
                        }
                        
                        str_len = (uint16_t)strlen(str);
                        
                        /* Validate from_index */
                        if (from_index > str_len) {
                            from_index = str_len;
                        }
                        
                        /* Search for substring starting from from_index */
                        result = -1;
                        found_pos = strstr(str + from_index, search_str);
                        if (found_pos != NULL) {
                            result = (int)(found_pos - str);
                        }
                        
                        /* Push result to stack */
                        if (stack_push_shared(ctx, (uint16_t)result) != 0) {
                            printf("ERROR: Stack overflow\n");
                            return -1;
                        }
                        break;
                    } else if (strcmp(method_name, "lastIndexOf") == 0) {
                        uint16_t search_value;
                        uint16_t str_value;
                        uint16_t from_index;
                        const char* str;
                        const char* search_str;
                        const char* found_pos;
                        const char* last_found;
                        uint16_t str_len;
                        uint16_t search_len;
                        int result;
                        int has_from_index;
                        const char* search_start;
                        
                        has_from_index = (arg_count == 3);
                        
                        if (arg_count != 2 && arg_count != 3) {
                            printf("ERROR: lastIndexOf expects 2 or 3 arguments, got %u\n", arg_count);
                            return -1;
                        }
                        
                        /* Pop arguments from stack */
                        if (has_from_index) {
                            from_index = stack_pop_shared(ctx);
                        } else {
                            from_index = 0;
                        }
                        search_value = stack_pop_shared(ctx);
                        str_value = stack_pop_shared(ctx);
                        
                        str = NULL;
                        search_str = NULL;
                        
                        /* Get target string from constant pool */
                        if (str_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
                                str = ctx->djc_file->constants[str_value].data.utf8_data;
                            }
                        }
                        
                        /* Get search string from constant pool */
                        if (search_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[search_value].tag == CONST_UTF8) {
                                search_str = ctx->djc_file->constants[search_value].data.utf8_data;
                            }
                        }
                        
                        if (!str || !search_str) {
                            printf("ERROR: Invalid string constant index for lastIndexOf: %d, %d\n",
                                   str_value, search_value);
                            return -1;
                        }
                        
                        str_len = (uint16_t)strlen(str);
                        search_len = (uint16_t)strlen(search_str);
                        
                        /* Determine search start position */
                        if (has_from_index) {
                            /* Search from fromIndex onwards */
                            if (from_index > str_len) {
                                from_index = str_len;
                            }
                            search_start = str + from_index;
                        } else {
                            /* Search entire string */
                            search_start = str;
                        }
                        
                        /* Find last occurrence by iterating through all occurrences */
                        result = -1;
                        last_found = NULL;
                        found_pos = search_start;
                        
                        while ((found_pos = strstr(found_pos, search_str)) != NULL) {
                            last_found = found_pos;
                            found_pos++; /* Move past this occurrence to find next */
                        }
                        
                        if (last_found != NULL) {
                            result = (int)(last_found - str);
                        }
                        
                        /* Push result to stack */
                        if (stack_push_shared(ctx, (uint16_t)result) != 0) {
                            printf("ERROR: Stack overflow\n");
                            return -1;
                        }
                        break;
                    } else if (strcmp(method_name, "substr") == 0) {
                        uint16_t str_value;
                        uint16_t from_index;
                        uint16_t end_index;
                        const char* src_str;
                        char substr_buf[256];
                        uint16_t str_len;
                        uint16_t substr_len;
                        uint16_t const_idx;
                        int has_end_index;
                        uint16_t i;
                        
                        has_end_index = (arg_count == 3);
                        
                        if (arg_count != 2 && arg_count != 3) {
                            printf("ERROR: substr expects 2 or 3 arguments, got %u\n", arg_count);
                            return -1;
                        }
                        
                        /* Pop arguments from stack */
                        if (has_end_index) {
                            end_index = stack_pop_shared(ctx);
                        } else {
                            end_index = 0xFFFF;  /* Use max value as sentinel */
                        }
                        from_index = stack_pop_shared(ctx);
                        str_value = stack_pop_shared(ctx);
                        
                        src_str = NULL;
                        
                        /* Get source string from constant pool */
                        if (str_value < ctx->djc_file->header.constant_pool_count) {
                            if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
                                src_str = ctx->djc_file->constants[str_value].data.utf8_data;
                            }
                        }
                        
                        if (!src_str) {
                            printf("ERROR: Invalid string constant index for substr: %d\n", str_value);
                            return -1;
                        }
                        
                        str_len = (uint16_t)strlen(src_str);
                        
                        /* Validate and adjust from_index */
                        if (from_index > str_len) {
                            from_index = str_len;
                        }
                        
                        /* Validate and adjust end_index */
                        if (has_end_index) {
                            if (end_index > str_len) {
                                end_index = str_len;
                            }
                            if (end_index < from_index) {
                                end_index = from_index;
                            }
                        } else {
                            end_index = str_len;
                        }
                        
                        /* Calculate substring length */
                        substr_len = end_index - from_index;
                        
                        if (substr_len >= sizeof(substr_buf)) {
                            printf("ERROR: Substring too long\n");
                            return -1;
                        }
                        
                        /* Extract substring */
                        for (i = 0; i < substr_len; i++) {
                            substr_buf[i] = src_str[from_index + i];
                        }
                        substr_buf[substr_len] = '\0';
                        
                        /* Add new string to constant pool */
                        const_idx = ctx->djc_file->header.constant_pool_count;
                        if (const_idx >= DJC_MAX_CONSTANTS) {
                            printf("ERROR: Constant pool full during substr\n");
                            return -1;
                        }
                        
                        ctx->djc_file->constants[const_idx].tag = CONST_UTF8;
                        ctx->djc_file->constants[const_idx].length = substr_len;
                        ctx->djc_file->constants[const_idx].data.utf8_data = (char*)memory_alloc(substr_len + 1);
                        if (ctx->djc_file->constants[const_idx].data.utf8_data == NULL) {
                            printf("ERROR: Out of memory during substr\n");
                            return -1;
                        }
                        strcpy(ctx->djc_file->constants[const_idx].data.utf8_data, substr_buf);
                        ctx->djc_file->header.constant_pool_count++;
                        
                        /* Push result to stack */
                        if (stack_push_shared(ctx, const_idx) != 0) {
                            printf("ERROR: Stack overflow\n");
                            return -1;
                        }
                        break;
                    } else if (strcmp(method_name, "open") == 0) {
                    /* File.open(String filename) or File.open(String filename, String mode) */
                    uint16_t filename_value;
                    uint16_t mode_value;
                    const char* filename;
                    const char* mode;
                    
                    if (arg_count != 1 && arg_count != 2) {
                        printf("ERROR: File.open expects 1 or 2 arguments, got %u\n", arg_count);
                        return -1;
                    }
                    
                    /* Pop arguments from stack (in reverse order) */
                    if (arg_count == 2) {
                        mode_value = stack_pop_shared(ctx);
                        filename_value = stack_pop_shared(ctx);
                    } else {
                        filename_value = stack_pop_shared(ctx);
                        mode_value = 0;  /* Default to read mode */
                    }
                    
                    filename = NULL;
                    mode = "r";  /* Default mode */
                    
                    /* Get filename from constant pool */
                    if (filename_value < ctx->djc_file->header.constant_pool_count) {
                        if (ctx->djc_file->constants[filename_value].tag == CONST_UTF8) {
                            filename = ctx->djc_file->constants[filename_value].data.utf8_data;
                        }
                    }
                    
                    if (!filename) {
                        printf("ERROR: Invalid filename constant index: %d\n", filename_value);
                        return -1;
                    }
                    
                    /* Get mode from constant pool if provided */
                    if (arg_count == 2 && mode_value < ctx->djc_file->header.constant_pool_count) {
                        if (ctx->djc_file->constants[mode_value].tag == CONST_UTF8) {
                            mode = ctx->djc_file->constants[mode_value].data.utf8_data;
                        }
                    }
                    
                    /* Validate mode */
                    if (strcmp(mode, "r") != 0 && strcmp(mode, "w") != 0 && strcmp(mode, "a") != 0) {
                        printf("ERROR: Invalid file mode: %s (must be 'r', 'w', or 'a')\n", mode);
                        return -1;
                    }
                    
                    /* Close previous file if open */
                    if (g_file_handle != NULL) {
                        fclose(g_file_handle);
                        g_file_handle = NULL;
                    }
                    
                    /* Open file with specified mode */
                    g_file_handle = fopen(filename, mode);
                    if (g_file_handle == NULL) {
                        /* Try to throw exception */
                        char error_msg[128];
                        sprintf(error_msg, "Cannot open file: %s", filename);
                        if (throw_runtime_exception(ctx, error_msg) != 0) {
                            return -1;  /* Not in try block - terminate */
                        }
                        break;  /* Exception handled - jumped to catch block */
                    }
                    break;
                } else if (strcmp(method_name, "readLine") == 0) {
                    /* File.readLine() returns String */
                    char line_buf[256];
                    uint16_t const_idx;
                    size_t len;
                    
                    if (arg_count != 0) {
                        printf("ERROR: File.readLine expects 0 arguments, got %u\n", arg_count);
                        return -1;
                    }
                    
                    /* Check if file is open */
                    if (g_file_handle == NULL) {
                        printf("ERROR: No file is open for reading\n");
                        return -1;
                    }
                    
                    /* Read line from file */
                    if (fgets(line_buf, sizeof(line_buf), g_file_handle) == NULL) {
                        /* EOF or error - return empty string */
                        line_buf[0] = '\0';
                    } else {
                        /* Remove trailing newline characters */
                        len = strlen(line_buf);
                        if (len > 0 && line_buf[len-1] == '\n') {
                            line_buf[len-1] = '\0';
                            len--;
                            if (len > 0 && line_buf[len-1] == '\r') {
                                line_buf[len-1] = '\0';
                            }
                        }
                    }
                    
                    /* Add string to constant pool */
                    const_idx = ctx->djc_file->header.constant_pool_count;
                    if (const_idx >= DJC_MAX_CONSTANTS) {
                        printf("ERROR: Constant pool full during readLine\n");
                        return -1;
                    }
                    
                    len = strlen(line_buf);
                    ctx->djc_file->constants[const_idx].tag = CONST_UTF8;
                    ctx->djc_file->constants[const_idx].length = (uint16_t)len;
                    ctx->djc_file->constants[const_idx].data.utf8_data = (char*)memory_alloc(len + 1);
                    if (ctx->djc_file->constants[const_idx].data.utf8_data == NULL) {
                        printf("ERROR: Out of memory during readLine\n");
                        return -1;
                    }
                    strcpy(ctx->djc_file->constants[const_idx].data.utf8_data, line_buf);
                    ctx->djc_file->header.constant_pool_count++;
                    
                    /* Push result to stack */
                    if (stack_push_shared(ctx, const_idx) != 0) {
                        printf("ERROR: Stack overflow\n");
                        return -1;
                    }
                    break;
                } else if (strcmp(method_name, "writeLine") == 0) {
                    /* File.writeLine(String text) */
                    uint16_t text_value;
                    const char* text;
                    
                    if (arg_count != 1) {
                        printf("ERROR: File.writeLine expects 1 argument, got %u\n", arg_count);
                        return -1;
                    }
                    
                    /* Check if file is open */
                    if (g_file_handle == NULL) {
                        printf("ERROR: No file is open for writing\n");
                        return -1;
                    }
                    
                    /* Pop text from stack */
                    text_value = stack_pop_shared(ctx);
                    text = NULL;
                    
                    /* Get text from constant pool */
                    if (text_value < ctx->djc_file->header.constant_pool_count) {
                        if (ctx->djc_file->constants[text_value].tag == CONST_UTF8) {
                            text = ctx->djc_file->constants[text_value].data.utf8_data;
                        }
                    }
                    
                    if (!text) {
                        printf("ERROR: Invalid text constant index: %d\n", text_value);
                        return -1;
                    }
                    
                    /* Write text with CR+LF line ending */
                    if (fprintf(g_file_handle, "%s\r\n", text) < 0) {
                        printf("ERROR: Failed to write to file\n");
                        return -1;
                    }
                    
                    /* Flush to ensure data is written */
                    fflush(g_file_handle);
                    break;
                } else if (strcmp(method_name, "close") == 0) {
                    /* File.close() */
                    
                    if (arg_count != 0) {
                        printf("ERROR: File.close expects 0 arguments, got %u\n", arg_count);
                        return -1;
                    }
                    
                    /* Close file if open */
                    if (g_file_handle != NULL) {
                        fclose(g_file_handle);
                        g_file_handle = NULL;
                    }
                    break;
                } else if (strcmp(method_name, "Integer.toString") == 0) {
                    /* Integer.toString(int) returns String */
                    int16_t int_value;
                    char int_buf[8];  /* Enough for 16-bit int (-32768 to 32767) */
                    uint16_t const_idx;
                    
                    if (arg_count != 1) {
                        printf("ERROR: Integer.toString expects 1 argument, got %u\n", arg_count);
                        return -1;
                    }
                    
                    /* Pop int value from stack */
                    int_value = (int16_t)stack_pop_shared(ctx);
                    
                    if (g_debug_mode) {
                        printf("[DEBUG] Integer.toString: converting %d to String\n", int_value);
                    }
                    
                    /* Convert int to string */
                    sprintf(int_buf, "%d", int_value);
                    
                    /* Add to constant pool */
                    const_idx = ctx->djc_file->header.constant_pool_count;
                    if (const_idx >= DJC_MAX_CONSTANTS) {
                        printf("ERROR: Constant pool full during Integer.toString\n");
                        return -1;
                    }
                    
                    ctx->djc_file->constants[const_idx].tag = CONST_UTF8;
                    ctx->djc_file->constants[const_idx].length = (uint16_t)strlen(int_buf);
                    ctx->djc_file->constants[const_idx].data.utf8_data = (char*)memory_alloc(strlen(int_buf) + 1);
                    if (ctx->djc_file->constants[const_idx].data.utf8_data == NULL) {
                        printf("ERROR: Out of memory during Integer.toString\n");
                        return -1;
                    }
                    strcpy(ctx->djc_file->constants[const_idx].data.utf8_data, int_buf);
                    ctx->djc_file->header.constant_pool_count++;
                    
                    if (g_debug_mode) {
                        printf("[DEBUG] Integer.toString: created constant at index %u: \"%s\"\n",
                               const_idx, int_buf);
                    }
                    
                    /* Push string constant index to stack */
                    if (stack_push_shared(ctx, const_idx) != 0) {
                        printf("ERROR: Stack overflow\n");
                        return -1;
                    }
                    break;
                } else if (strcmp(method_name, "Long.toString") == 0) {
                    /* Long.toString(long) returns String */
                    int32_t long_value;
                    char long_buf[16];  /* Enough for 32-bit long */
                    uint16_t const_idx;
                    uint16_t low, high;
                    
                    if (arg_count != 1) {
                        printf("ERROR: Long.toString expects 1 argument, got %u\n", arg_count);
                        return -1;
                    }
                    
                    /* Pop long value from stack (low word first, then high word) */
                    low = stack_pop_shared(ctx);
                    high = stack_pop_shared(ctx);
                    long_value = ((int32_t)high << 16) | low;
                    
                    if (g_debug_mode) {
                        printf("[DEBUG] Long.toString: converting %ld to String\n", (long)long_value);
                    }
                    
                    /* Convert long to string */
                    sprintf(long_buf, "%ld", (long)long_value);
                    
                    /* Add to constant pool */
                    const_idx = ctx->djc_file->header.constant_pool_count;
                    if (const_idx >= DJC_MAX_CONSTANTS) {
                        printf("ERROR: Constant pool full during Long.toString\n");
                        return -1;
                    }
                    
                    ctx->djc_file->constants[const_idx].tag = CONST_UTF8;
                    ctx->djc_file->constants[const_idx].length = (uint16_t)strlen(long_buf);
                    ctx->djc_file->constants[const_idx].data.utf8_data = (char*)memory_alloc(strlen(long_buf) + 1);
                    if (ctx->djc_file->constants[const_idx].data.utf8_data == NULL) {
                        printf("ERROR: Out of memory during Long.toString\n");
                        return -1;
                    }
                    strcpy(ctx->djc_file->constants[const_idx].data.utf8_data, long_buf);
                    ctx->djc_file->header.constant_pool_count++;
                    
                    if (g_debug_mode) {
                        printf("[DEBUG] Long.toString: created constant at index %u: \"%s\"\n",
                               const_idx, long_buf);
                    }
                    
                    /* Push string constant index to stack */
                    if (stack_push_shared(ctx, const_idx) != 0) {
                        printf("ERROR: Stack overflow\n");
                        return -1;
                    }
                    break;
                } else if (strcmp(method_name, "Float.toString") == 0) {
                    /* Float.toString(float) returns String */
                    float float_value;
                    char float_buf[16];
                    uint16_t const_idx;
                    uint16_t low, high;
                    uint32_t bits;
                    
                    if (arg_count != 1) {
                        printf("ERROR: Float.toString expects 1 argument, got %u\n", arg_count);
                        return -1;
                    }
                    
                    /* Pop float value from stack (low word first, then high word) */
                    low = stack_pop_shared(ctx);
                    high = stack_pop_shared(ctx);
                    bits = ((uint32_t)high << 16) | low;
                    memcpy(&float_value, &bits, sizeof(float));
                    
                    if (g_debug_mode) {
                        printf("[DEBUG] Float.toString: converting %.2f to String\n", float_value);
                    }
                    
                    /* Convert float to string */
                    sprintf(float_buf, "%.2f", float_value);
                    
                    /* Add to constant pool */
                    const_idx = ctx->djc_file->header.constant_pool_count;
                    if (const_idx >= DJC_MAX_CONSTANTS) {
                        printf("ERROR: Constant pool full during Float.toString\n");
                        return -1;
                    }
                    
                    ctx->djc_file->constants[const_idx].tag = CONST_UTF8;
                    ctx->djc_file->constants[const_idx].length = (uint16_t)strlen(float_buf);
                    ctx->djc_file->constants[const_idx].data.utf8_data = (char*)memory_alloc(strlen(float_buf) + 1);
                    if (ctx->djc_file->constants[const_idx].data.utf8_data == NULL) {
                        printf("ERROR: Out of memory during Float.toString\n");
                        return -1;
                    }
                    strcpy(ctx->djc_file->constants[const_idx].data.utf8_data, float_buf);
                    ctx->djc_file->header.constant_pool_count++;
                    
                    if (g_debug_mode) {
                        printf("[DEBUG] Float.toString: created constant at index %u: \"%s\"\n",
                               const_idx, float_buf);
                    }
                    
                    /* Push string constant index to stack */
                    if (stack_push_shared(ctx, const_idx) != 0) {
                        printf("ERROR: Stack overflow\n");
                        return -1;
                    }
                    break;
                }
                
                /* If we reach here, method_name was not recognized */
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
            
            /* Save current state to call frame.
             * The new callee frame owns locals starting at the current local_pointer.
             */
            frame = &ctx->call_frames[ctx->call_depth];
            frame->return_pc = ctx->pc;
            frame->return_code_start = ctx->code_start;
            frame->return_code_length = ctx->code_length;
            frame->frame_pointer = ctx->stack_pointer;
            frame->local_base = ctx->local_pointer;
            frame->local_count = method->max_locals;
            
            /* Increment call depth */
            ctx->call_depth++;
            
            /* The active frame base is derived from call_frames[call_depth - 1].
             * Publish callee base there before any local initialization or arg moves.
             */
            ctx->call_frames[ctx->call_depth - 1].local_base = frame->local_base;
            ctx->call_frames[ctx->call_depth - 1].local_count = frame->local_count;
            
            /* Initialize new locals to 0 */
            if (method->max_locals > 0) {
                memset(&ctx->shared_locals[frame->local_base], 0,
                       method->max_locals * sizeof(uint16_t));
            }
            
            /* Allocate space for new method's locals */
            ctx->local_pointer += method->max_locals;
            
            /* Move arguments from operand stack into callee locals.
             * Arguments are evaluated left-to-right and pushed in order,
             * so pop them in reverse into local slots [0..arg_count-1].
             */
            {
                uint8_t arg_index;
                
                if (arg_count > method->max_locals) {
                    arg_count = method->max_locals;
                }
                if (arg_count > ctx->stack_pointer) {
                    arg_count = (uint8_t)ctx->stack_pointer;
                }
                
                for (arg_index = 0; arg_index < arg_count; arg_index++) {
                    uint16_t arg_value = stack_pop_shared(ctx);
                    ctx->shared_locals[frame->local_base + arg_count - arg_index - 1] = arg_value;
                }
            }
            
            
            /* Set PC to method code */
            ctx->pc = method_code;
            ctx->code_start = method_code;
            ctx->code_length = method->code_length;
            
            break;
        }
        
        case OP_RETURN: {
            CallFrame* frame;
            
            /* Check if this is main method return */
            if (ctx->call_depth <= 1) {
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
            
            break;
        }
        
        case OP_RETURN_VALUE: {
            CallFrame* frame;
            uint16_t return_value;
            
            /* Pop return value */
            return_value = stack_pop_shared(ctx);
            
            /* Check if this is main method return */
            if (ctx->call_depth <= 1) {
                const char* descriptor = NULL;
                char return_type = '\0';
                uint16_t expected_value = 0;
                
                if (ctx->djc_file && ctx->djc_file->header.method_count > 0) {
                    uint16_t i;
                    for (i = 0; i < ctx->djc_file->header.method_count; i++) {
                        DJCMethod* candidate = &ctx->djc_file->methods[i];
                        uint8_t* candidate_code = djc_get_method_code(ctx->djc_file, candidate);
                        if (candidate_code == ctx->code_start) {
                            descriptor = djc_get_utf8(ctx->djc_file, candidate->descriptor_index);
                            break;
                        }
                    }
                }
                
                return_type = descriptor_return_type(descriptor);
                if (descriptor && return_type == 'V') {
                    printf("ERROR: Void method returned a value\n");
                    return -1;
                }
                if (descriptor && return_type == 'I') {
                    expected_value = return_value;
                    (void)expected_value;
                }
                
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
            
            /* Restore PC and code context */
            ctx->pc = frame->return_pc;
            ctx->code_start = frame->return_code_start;
            ctx->code_length = frame->return_code_length;
            
            break;
        }
        
        case OP_NEW: {
            /* Create new object [class_name_idx:2] [arg_count:1] */
            uint16_t class_name_idx;
            uint8_t arg_count;
            uint16_t object_handle;
            const char* class_name;
            uint16_t args[8];  /* Support up to 8 constructor arguments */
            uint8_t i;
            
            /* Read class name index from constant pool */
            class_name_idx = interpreter_read_u16(ctx);
            
            /* Read argument count */
            arg_count = interpreter_read_u8(ctx);
            
            /* Get class name from constant pool */
            class_name = djc_get_utf8(ctx->djc_file, class_name_idx);
            if (class_name == NULL) {
                printf("ERROR: Invalid class name index: %u\n", class_name_idx);
                return -1;
            }
            
            /* Pop constructor arguments from stack (in reverse order) */
            if (arg_count > 8) {
                printf("ERROR: Too many constructor arguments: %u\n", arg_count);
                return -1;
            }
            
            for (i = 0; i < arg_count; i++) {
                args[arg_count - 1 - i] = stack_pop_shared(ctx);
            }
            
            /* Handle special I/O classes with runtime support */
            if (strcmp(class_name, "FileOutputStream") == 0 && arg_count == 1) {
                /* FileOutputStream(String filename) */
                const char* filename = djc_get_utf8(ctx->djc_file, args[0]);
                FileOutputStream* fos;
                
                if (filename == NULL) {
                    printf("ERROR: Invalid filename for FileOutputStream\n");
                    return -1;
                }
                
                fos = fileoutputstream_new(filename);
                if (fos == NULL) {
                    printf("ERROR: Failed to create FileOutputStream\n");
                    return -1;
                }
                
                /* Allocate object handle */
                object_handle = memory_alloc_object_handle((void*)fos);
                if (object_handle == 0) {
                    printf("ERROR: Failed to allocate object handle for FileOutputStream\n");
                    return -1;
                }
            }
            else if (strcmp(class_name, "BufferedWriter") == 0 && arg_count == 1) {
                /* BufferedWriter(OutputStream os) */
                FileOutputStream* fos = (FileOutputStream*)memory_get_object_ptr(args[0]);
                BufferedWriter* bw;
                
                if (fos == NULL) {
                    printf("ERROR: Invalid FileOutputStream handle\n");
                    return -1;
                }
                
                bw = bufferedwriter_new(fos, 256);
                if (bw == NULL) {
                    printf("ERROR: Failed to create BufferedWriter\n");
                    return -1;
                }
                
                /* Allocate object handle */
                object_handle = memory_alloc_object_handle((void*)bw);
                if (object_handle == 0) {
                    printf("ERROR: Failed to allocate object handle for BufferedWriter\n");
                    return -1;
                }
            }
            else if (strcmp(class_name, "FileInputStream") == 0 && arg_count == 1) {
                /* FileInputStream(String filename) */
                const char* filename = djc_get_utf8(ctx->djc_file, args[0]);
                FileInputStream* fis;
                
                if (filename == NULL) {
                    printf("ERROR: Invalid filename for FileInputStream\n");
                    return -1;
                }
                
                fis = fileinputstream_new(filename);
                if (fis == NULL) {
                    printf("ERROR: Failed to create FileInputStream\n");
                    return -1;
                }
                
                /* Allocate object handle */
                object_handle = memory_alloc_object_handle((void*)fis);
                if (object_handle == 0) {
                    printf("ERROR: Failed to allocate object handle for FileInputStream\n");
                    return -1;
                }
            }
            else if (strcmp(class_name, "BufferedReader") == 0 && arg_count == 1) {
                /* BufferedReader(InputStream is) */
                FileInputStream* fis = (FileInputStream*)memory_get_object_ptr(args[0]);
                BufferedReader* br;
                
                if (fis == NULL) {
                    printf("ERROR: Invalid FileInputStream handle\n");
                    return -1;
                }
                
                br = bufferedreader_new(fis, 256);
                if (br == NULL) {
                    printf("ERROR: Failed to create BufferedReader\n");
                    return -1;
                }
                
                /* Allocate object handle */
                object_handle = memory_alloc_object_handle((void*)br);
                if (object_handle == 0) {
                    printf("ERROR: Failed to allocate object handle for BufferedReader\n");
                    return -1;
                }
            }
            else if (strcmp(class_name, "Date") == 0) {
                /* Date() or Date(long timestamp) */
                Date* date_obj;
                
                if (arg_count == 0) {
                    /* Date() - current time */
                    date_obj = date_new();
                } else if (arg_count == 1) {
                    /* Date(int timestamp) - int is passed as 1 word, treat as milliseconds */
                    uint32_t time_ms_low = (uint32_t)args[0];
                    date_obj = date_new_with_time(0, time_ms_low);
                } else if (arg_count == 2) {
                    /* Date(long timestamp) - long is passed as 2 words [high, low] */
                    /* Stack layout: args[0]=high, args[1]=low */
                    uint32_t time_ms_low;
                    uint16_t high = args[0];
                    uint16_t low = args[1];
                    time_ms_low = ((uint32_t)high << 16) | low;
                    date_obj = date_new_with_time(0, time_ms_low);
                } else {
                    printf("ERROR: Invalid argument count for Date constructor: %u\n", arg_count);
                    return -1;
                }
                
                if (date_obj == NULL) {
                    printf("ERROR: Failed to create Date object\n");
                    return -1;
                }
                
                /* Allocate object handle for Date object */
                object_handle = memory_alloc_object_handle((void*)date_obj);
                if (object_handle == 0) {
                    printf("ERROR: Failed to allocate object handle for Date\n");
                    return -1;
                }
            }
            else {
                /* Generic object creation (no constructor logic yet) */
                object_handle = class_name_idx + 1;
            }
            
            /* Push object handle onto stack */
            if (stack_push_shared(ctx, object_handle) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            
            break;
        }
        case OP_ACONST_NULL:
            /* Push null reference (0) */
            stack_push_shared(ctx, 0);
            break;
        
        
        case OP_GET_FIELD: {
            /* Read instance field [field_name_idx:2] */
            uint16_t field_name_idx;
            uint16_t object_handle;
            const char* field_name;
            uint16_t field_value;
            uint16_t storage_index;
            
            /* Read field name index */
            field_name_idx = interpreter_read_u16(ctx);
            
            /* Pop object reference from stack */
            object_handle = stack_pop_shared(ctx);
            
            /* Get field name from constant pool */
            field_name = djc_get_utf8(ctx->djc_file, field_name_idx);
            if (field_name == NULL) {
                printf("ERROR: Invalid field name index: %u\n", field_name_idx);
                return -1;
            }
            
            /* For Phase 1, use a simple field storage mechanism
             * Store fields in shared_locals using object_handle and field_name_idx
             * This is a simplified implementation for testing
             */
            /* Combine object_handle and field_name_idx to create unique storage location */
            storage_index = (object_handle * 10 + field_name_idx) % (SHARED_LOCALS_SIZE - 100);
            
            if (storage_index >= SHARED_LOCALS_SIZE) {
                printf("ERROR: Invalid storage index for field access: %u\n", storage_index);
                return -1;
            }
            
            field_value = ctx->shared_locals[storage_index];
            
            
            /* Push field value onto stack */
            if (stack_push_shared(ctx, field_value) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            
            if (g_debug_mode) {
                printf("DEBUG: OP_GET_FIELD '%s' from object %u = %u\n",
                       field_name, object_handle, field_value);
            }
            break;
        }
        
        case OP_PUT_FIELD: {
            /* Write instance field [field_name_idx:2] */
            uint16_t field_name_idx;
            uint16_t field_value;
            uint16_t object_handle;
            const char* field_name;
            uint16_t storage_index;
            
            /* Read field name index */
            field_name_idx = interpreter_read_u16(ctx);
            
            
            /* Pop value and object reference from stack
             * Codegen generates: object_ref, OP_DUP, value
             * Stack order: ... object_ref object_ref value (top)
             * We pop value, then one object_ref, leaving one object_ref as result
             */
            field_value = stack_pop_shared(ctx);
            object_handle = stack_pop_shared(ctx);
            
            /* Note: One object_ref remains on stack as assignment result */
            
            /* Get field name from constant pool */
            field_name = djc_get_utf8(ctx->djc_file, field_name_idx);
            if (field_name == NULL) {
                printf("ERROR: Invalid field name index: %u\n", field_name_idx);
                return -1;
            }
            
            /* For Phase 1, use simple field storage */
            /* Combine object_handle and field_name_idx to create unique storage location */
            storage_index = (object_handle * 10 + field_name_idx) % (SHARED_LOCALS_SIZE - 100);
            
            if (storage_index >= SHARED_LOCALS_SIZE) {
                printf("ERROR: Invalid storage index for field access: %u\n", storage_index);
                return -1;
            }
            
            ctx->shared_locals[storage_index] = field_value;
            
            
            if (g_debug_mode) {
                printf("DEBUG: OP_PUT_FIELD '%s' to object %u = %u\n",
                       field_name, object_handle, field_value);
            }
            break;
        }
        
        case OP_INVOKE_VIRTUAL: {
            /* Virtual method call [method_name_idx:2] */
            uint16_t method_name_idx;
            const char* method_name;
            DJCMethod* method;
            uint8_t* method_code;
            CallFrame* frame;
            uint8_t arg_count;
            uint16_t object_handle;
            
            /* Read method name index */
            method_name_idx = interpreter_read_u16(ctx);
            
            /* Get method name from constant pool */
            method_name = djc_get_utf8(ctx->djc_file, method_name_idx);
            if (method_name == NULL) {
                printf("ERROR: Invalid method name index: %u\n", method_name_idx);
                return -1;
            }
            
            /* Check for native I/O methods before looking up in DJC file */
            /* Peek at object handle (it's at stack top - arg_count - 1) */
            if (ctx->stack_pointer > 0) {
                /* For now, check method name to determine if it's a native method */
                if (strcmp(method_name, "write") == 0) {
                    /* Check if it's BufferedWriter.write(String) or FileOutputStream.write(int) */
                    uint16_t arg_value;
                    
                    /* Pop argument */
                    arg_value = stack_pop_shared(ctx);
                    
                    /* Pop object reference (handle) */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Try to determine object type by checking if arg is a string index */
                    if (arg_value < ctx->djc_file->header.constant_pool_count &&
                        ctx->djc_file->constants[arg_value].tag == CONST_UTF8) {
                        /* BufferedWriter.write(String) */
                        const char* str_value;
                        BufferedWriter* bw;
                        
                        str_value = ctx->djc_file->constants[arg_value].data.utf8_data;
                        if (str_value == NULL) {
                            printf("ERROR: Invalid string for write()\n");
                            return -1;
                        }
                        
                        /* Resolve handle to pointer */
                        bw = (BufferedWriter*)memory_get_object_ptr(object_handle);
                        if (bw == NULL) {
                            printf("ERROR: Invalid BufferedWriter handle in write()\n");
                            return -1;
                        }
                        
                        bufferedwriter_write_string(bw, str_value);
                    } else {
                        /* FileOutputStream.write(int) */
                        FileOutputStream* fos;
                        
                        /* Resolve handle to pointer */
                        fos = (FileOutputStream*)memory_get_object_ptr(object_handle);
                        if (fos == NULL) {
                            printf("ERROR: Invalid FileOutputStream handle in write()\n");
                            return -1;
                        }
                        
                        fileoutputstream_write(fos, (uint8_t)arg_value);
                    }
                    break;
                }
                else if (strcmp(method_name, "read") == 0) {
                    /* FileInputStream.read() - returns int */
                    FileInputStream* fis;
                    int byte_value;
                    
                    /* Pop object reference (handle) */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Resolve handle to pointer */
                    fis = (FileInputStream*)memory_get_object_ptr(object_handle);
                    if (fis == NULL) {
                        printf("ERROR: Invalid FileInputStream handle in read()\n");
                        return -1;
                    }
                    
                    /* Call native method */
                    byte_value = fileinputstream_read(fis);
                    
                    /* Push result (-1 for EOF, or 0-255 for byte value) */
                    if (byte_value == -1) {
                        stack_push_shared(ctx, (uint16_t)-1);  /* -1 as uint16_t is 0xFFFF */
                    } else {
                        stack_push_shared(ctx, (uint16_t)byte_value);
                    }
                    break;
                }
                else if (strcmp(method_name, "newLine") == 0) {
                    /* BufferedWriter.newLine() */
                    BufferedWriter* bw;
                    
                    /* Pop object reference (handle) */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Resolve handle to pointer */
                    bw = (BufferedWriter*)memory_get_object_ptr(object_handle);
                    if (bw == NULL) {
                        printf("ERROR: Invalid BufferedWriter handle in newLine()\n");
                        return -1;
                    }
                    
                    /* Call native method */
                    bufferedwriter_new_line(bw);
                    break;
                }
                else if (strcmp(method_name, "readLine") == 0) {
                    /* BufferedReader.readLine() - returns String */
                    BufferedReader* br;
                    char* line;
                    uint16_t str_idx;
                    
                    /* Pop object reference */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Resolve handle to pointer */
                    br = (BufferedReader*)memory_get_object_ptr(object_handle);
                    if (br == NULL) {
                        printf("ERROR: Invalid BufferedReader handle\n");
                        return -1;
                    }
                    
                    /* Call native method */
                    line = bufferedreader_read_line(br);
                    
                    if (line == NULL) {
                        /* Return null (represented as 0) */
                        stack_push_shared(ctx, 0);
                    } else {
                        /* Add string to constant pool and return index */
                        str_idx = djc_add_string(ctx->djc_file, line);
                        stack_push_shared(ctx, str_idx);
                    }
                    break;
                }
                else if (strcmp(method_name, "writeLine") == 0) {
                    /* BufferedWriter.writeLine(String) */
                    uint16_t str_idx;
                    const char* str_value;
                    BufferedWriter* bw;
                    
                    /* Pop string argument */
                    str_idx = stack_pop_shared(ctx);
                    
                    /* Pop object reference */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Get string value */
                    str_value = djc_get_utf8(ctx->djc_file, str_idx);
                    if (str_value == NULL) {
                        printf("ERROR: Invalid string for writeLine()\n");
                        return -1;
                    }
                    
                    /* Resolve handle to pointer */
                    bw = (BufferedWriter*)memory_get_object_ptr(object_handle);
                    if (bw == NULL) {
                        printf("ERROR: Invalid BufferedWriter handle\n");
                        return -1;
                    }
                    
                    /* Call native method */
                    bufferedwriter_write_line(bw, str_value);
                    break;
                }
                else if (strcmp(method_name, "close") == 0) {
                    /* close() for various stream types */
                    void* stream_obj;
                    BufferedWriter* bw;
                    FileOutputStream* fos;
                    FileInputStream* fis;
                    
                    /* Pop object reference */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Resolve handle to pointer */
                    stream_obj = memory_get_object_ptr(object_handle);
                    if (stream_obj == NULL) {
                        printf("ERROR: Invalid stream handle in close()\n");
                        return -1;
                    }
                    
                    /* Determine object type by checking structure
                     * BufferedWriter has output_stream as first field (non-NULL pointer)
                     * FileOutputStream has OutputStream base as first field (is_open flag)
                     * We check if first word looks like a valid pointer (> 256)
                     */
                    bw = (BufferedWriter*)stream_obj;
                    if (bw->output_stream != NULL && (uintptr_t)bw->output_stream > 256) {
                        /* Looks like BufferedWriter - has valid output_stream pointer */
                        bufferedwriter_close(bw);
                    } else {
                        /* Try FileOutputStream or FileInputStream */
                        fos = (FileOutputStream*)stream_obj;
                        if (fos->base.is_open) {
                            fileoutputstream_close(fos);
                        } else {
                            /* Try FileInputStream */
                            fis = (FileInputStream*)stream_obj;
                            fileinputstream_close(fis);
                        }
                    }
                    break;
                }
                else if (strcmp(method_name, "getTime") == 0) {
                    /* Date.getTime() - returns long (milliseconds as 64-bit value) */
                    Date* date_obj;
                    uint32_t time_ms_low;
                    
                    /* Pop object reference (1 word: handle) */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Resolve handle to pointer */
                    date_obj = (Date*)memory_get_object_ptr(object_handle);
                    if (date_obj == NULL) {
                        printf("ERROR: Invalid Date handle in getTime(), handle=%u\n", object_handle);
                        return -1;
                    }
                    
                    time_ms_low = date_get_time_low(date_obj);
                    
                    /* Push result as long (32-bit value, high word is always 0 for now) */
                    /* For dates until year 2106, high 32 bits are 0 */
                    if (stack_push_long(ctx, time_ms_low) != 0) {
                        printf("ERROR: Stack overflow\n");
                        return -1;
                    }
                    
                    break;
                }
                else if (strcmp(method_name, "setTime") == 0) {
                    /* Date.setTime(int/long timestamp) - set time in seconds
                     * Supports both int (1 word) and long (2 words) arguments
                     * Stack layout: [object_handle, arg_low] or [object_handle, arg_high, arg_low]
                     */
                    Date* date_obj;
                    uint32_t time_sec;
                    uint16_t arg_low, arg_high;
                    uint16_t saved_object_handle;
                    
                    /* Determine argument count by checking stack depth
                     * We need at least 2 words (object + 1 arg) or 3 words (object + 2 args)
                     */
                    if (ctx->stack_pointer < 2) {
                        printf("ERROR: Not enough arguments for setTime()\n");
                        return -1;
                    }
                    
                    /* Pop first argument word (low word for both int and long) */
                    arg_low = stack_pop_shared(ctx);
                    
                    /* Peek at next word to determine if it's object handle or high word */
                    if (ctx->stack_pointer > 0) {
                        /* Check if next word looks like an object handle (1-255)
                         * or like a high word of a long value (could be 0 or any value)
                         * For int literals like 20000 (0x4E20), we have just [object, 0x4E20]
                         * For long values, we have [object, high, low]
                         */
                        if (ctx->stack_pointer >= 2) {
                            /* We have at least 2 more words, check if it's long format */
                            uint16_t potential_object = ctx->shared_stack[ctx->stack_pointer - 2];
                            
                            /* If potential_object looks like a valid handle (1-255) and
                             * we have 3 words total, treat as long */
                            if (potential_object > 0 && potential_object < 256) {
                                /* setTime(long) - 3 words: [object, high, low] */
                                arg_high = stack_pop_shared(ctx);
                                saved_object_handle = stack_pop_shared(ctx);
                                time_sec = ((uint32_t)arg_high << 16) | arg_low;
                            } else {
                                /* setTime(int) - 2 words: [object, value] */
                                saved_object_handle = stack_pop_shared(ctx);
                                time_sec = (uint32_t)arg_low;
                            }
                        } else {
                            /* Only 1 word left, must be object handle - setTime(int) */
                            saved_object_handle = stack_pop_shared(ctx);
                            time_sec = (uint32_t)arg_low;
                        }
                    } else {
                        printf("ERROR: Missing object reference for setTime()\n");
                        return -1;
                    }
                    
                    /* Resolve handle to pointer */
                    date_obj = (Date*)memory_get_object_ptr(saved_object_handle);
                    if (date_obj == NULL) {
                        printf("ERROR: Invalid Date handle in setTime(), handle=%u\n", saved_object_handle);
                        return -1;
                    }
                    
                    /* Call native method (time_sec is in seconds) */
                    date_set_time(date_obj, 0, time_sec);
                    break;
                }
                else if (strcmp(method_name, "getFullYear") == 0) {
                    /* Date.getFullYear() - returns int */
                    Date* date_obj;
                    uint16_t year;
                    
                    /* Pop object reference (1 word: handle) */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Resolve handle to pointer */
                    date_obj = (Date*)memory_get_object_ptr(object_handle);
                    if (date_obj == NULL) {
                        printf("ERROR: Invalid Date handle in getFullYear()\n");
                        return -1;
                    }
                    
                    /* Call native method */
                    year = date_get_full_year(date_obj);
                    
                    /* Push result */
                    stack_push_shared(ctx, year);
                    break;
                }
                else if (strcmp(method_name, "getMonth") == 0) {
                    /* Date.getMonth() - returns int (0-11) */
                    Date* date_obj;
                    uint8_t month;
                    
                    /* Pop object reference (1 word: handle) */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Resolve handle to pointer */
                    date_obj = (Date*)memory_get_object_ptr(object_handle);
                    if (date_obj == NULL) {
                        printf("ERROR: Invalid Date handle in getMonth()\n");
                        return -1;
                    }
                    
                    /* Call native method */
                    month = date_get_month(date_obj);
                    
                    /* Push result */
                    stack_push_shared(ctx, (uint16_t)month);
                    break;
                }
                else if (strcmp(method_name, "getDate") == 0) {
                    /* Date.getDate() - returns int (1-31) */
                    Date* date_obj;
                    uint8_t day;
                    
                    /* Pop object reference (1 word: handle) */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Resolve handle to pointer */
                    date_obj = (Date*)memory_get_object_ptr(object_handle);
                    if (date_obj == NULL) {
                        printf("ERROR: Invalid Date handle in getDate()\n");
                        return -1;
                    }
                    
                    /* Call native method */
                    day = date_get_date(date_obj);
                    
                    /* Push result */
                    stack_push_shared(ctx, (uint16_t)day);
                    break;
                }
                else if (strcmp(method_name, "getHours") == 0) {
                    /* Date.getHours() - returns int (0-23) */
                    Date* date_obj;
                    uint8_t hours;
                    
                    /* Pop object reference (1 word: handle) */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Resolve handle to pointer */
                    date_obj = (Date*)memory_get_object_ptr(object_handle);
                    if (date_obj == NULL) {
                        printf("ERROR: Invalid Date handle in getHours()\n");
                        return -1;
                    }
                    
                    /* Call native method */
                    hours = date_get_hours(date_obj);
                    
                    /* Push result */
                    stack_push_shared(ctx, (uint16_t)hours);
                    break;
                }
                else if (strcmp(method_name, "getMinutes") == 0) {
                    /* Date.getMinutes() - returns int (0-59) */
                    Date* date_obj;
                    uint8_t minutes;
                    
                    /* Pop object reference (1 word: handle) */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Resolve handle to pointer */
                    date_obj = (Date*)memory_get_object_ptr(object_handle);
                    if (date_obj == NULL) {
                        printf("ERROR: Invalid Date handle in getMinutes()\n");
                        return -1;
                    }
                    
                    /* Call native method */
                    minutes = date_get_minutes(date_obj);
                    
                    /* Push result */
                    stack_push_shared(ctx, (uint16_t)minutes);
                    break;
                }
                else if (strcmp(method_name, "getSeconds") == 0) {
                    /* Date.getSeconds() - returns int (0-59) */
                    Date* date_obj;
                    uint8_t seconds;
                    
                    /* Pop object reference (1 word: handle) */
                    object_handle = stack_pop_shared(ctx);
                    
                    /* Resolve handle to pointer */
                    date_obj = (Date*)memory_get_object_ptr(object_handle);
                    if (date_obj == NULL) {
                        printf("ERROR: Invalid Date handle in getSeconds()\n");
                        return -1;
                    }
                    
                    /* Call native method */
                    seconds = date_get_seconds(date_obj);
                    
                    /* Push result */
                    stack_push_shared(ctx, (uint16_t)seconds);
                    break;
                }
            }
            
            /* Find method by name */
            method = djc_find_method_by_name(ctx->djc_file, method_name);
            if (method == NULL) {
                printf("ERROR: Method not found: %s\n", method_name);
                return -1;
            }
            
            /* Get parameter count from method descriptor */
            {
                const char* descriptor_str;
                descriptor_str = djc_get_utf8(ctx->djc_file, method->descriptor_index);
                if (descriptor_str == NULL) {
                    printf("ERROR: Invalid descriptor index\n");
                    return -1;
                }
                arg_count = descriptor_param_count(descriptor_str);
                if (arg_count == 0xFF) {
                    printf("ERROR: Invalid method descriptor: %s\n", descriptor_str);
                    return -1;
                }
            }
            
            /* Check call depth */
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
            
            /* Set up new frame */
            ctx->call_frames[ctx->call_depth - 1].local_base = frame->local_base;
            ctx->call_frames[ctx->call_depth - 1].local_count = frame->local_count;
            
            /* Initialize new locals to 0 */
            if (method->max_locals > 0) {
                memset(&ctx->shared_locals[frame->local_base], 0,
                       method->max_locals * sizeof(uint16_t));
            }
            
            /* Allocate space for new method's locals */
            ctx->local_pointer += method->max_locals;
            
            /* Move arguments and object reference from operand stack into callee locals
             * Stack layout before call: [object, arg1, arg2, ...]
             * Local layout after call: [this=object, arg1, arg2, ...]
             */
            {
                uint8_t arg_index;
                uint8_t total_args = arg_count + 1;  /* +1 for 'this' */
                
                if (total_args > method->max_locals) {
                    total_args = method->max_locals;
                }
                if (total_args > ctx->stack_pointer) {
                    total_args = (uint8_t)ctx->stack_pointer;
                }
                
                /* Pop arguments in reverse order (last arg first) */
                for (arg_index = 0; arg_index < total_args; arg_index++) {
                    uint16_t arg_value = stack_pop_shared(ctx);
                    uint8_t local_idx = total_args - arg_index - 1;
                    ctx->shared_locals[frame->local_base + local_idx] = arg_value;
                }
            }
            
            /* Set PC to method code */
            ctx->pc = method_code;
            ctx->code_start = method_code;
            ctx->code_length = method->code_length;
            
            if (g_debug_mode) {
                printf("DEBUG: OP_INVOKE_VIRTUAL '%s' (args=%u)\n", method_name, arg_count);
            }
            break;
        }
        
        /* Exception Handling Opcodes (Simple Implementation) */
        case OP_TRY_BEGIN: {
            /* Mark try block begin - read catch offset and set catch_pc */
            uint16_t catch_offset;
            
            catch_offset = interpreter_read_u16(ctx);
            ctx->in_try_block = 1;
            
            /* Set catch_pc to the absolute address of catch block */
            if (catch_offset != 0) {
                ctx->catch_pc = ctx->code_start + catch_offset;
            } else {
                ctx->catch_pc = NULL;
            }
            break;
        }
        
        case OP_TRY_END:
            /* Mark try block end - clear try block flag */
            ctx->in_try_block = 0;
            break;
        
        case OP_CATCH_BEGIN:
            /* Mark catch block begin - catch_pc already set by OP_TRY_BEGIN */
            break;
        
        case OP_CATCH_END:
            /* Mark catch block end - clear catch PC */
            ctx->catch_pc = NULL;
            break;
        
        case OP_FINALLY_BEGIN:
            /* Mark finally block begin - no-op for simple implementation */
            break;
        
        case OP_FINALLY_END:
            /* Mark finally block end - no-op for simple implementation */
            break;
        
        case OP_THROW:
            /* Throw exception - simple implementation: jump to catch if available */
            
            /* Pop exception value from stack (we don't use it in simple implementation) */
            if (ctx->stack_pointer > 0) {
                ctx->stack_pointer--;
            }
            
            /* If we have a catch block, jump to it */
            if (ctx->catch_pc != NULL) {
                ctx->pc = ctx->catch_pc;
                ctx->catch_pc = NULL;  /* Clear catch PC after jumping */
            } else {
                /* No catch block - halt execution */
                printf("Unhandled exception - halting\n");
                ctx->running = 0;
                return 1;
            }
            break;
        
        /* ===== Long Type Operations ===== */
        
        case OP_PUSH_LONG: {
            /* Push 32-bit long constant [high:2] [low:2] */
            uint16_t high;
            uint16_t low;
            uint32_t value;
            
            high = interpreter_read_u16(ctx);
            low = interpreter_read_u16(ctx);
            value = ((uint32_t)high << 16) | low;
            
            if (stack_push_long(ctx, value) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_LOAD_LONG: {
            /* Load long from locals [idx:1] (2 slots) */
            uint8_t index;
            uint32_t value;
            
            index = interpreter_read_u8(ctx);
            value = load_local_long(ctx, index);
            
            
            if (stack_push_long(ctx, value) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_STORE_LONG: {
            /* Store long to locals [idx:1] (2 slots) */
            uint8_t index;
            uint32_t value;
            
            index = interpreter_read_u8(ctx);
            value = stack_pop_long(ctx);
            
            
            store_local_long(ctx, index, value);
            break;
        }
        
        case OP_I2L: {
            /* int to long conversion (sign extension) */
            uint16_t int_val;
            int16_t signed_val;
            uint32_t long_val;
            
            int_val = stack_pop_shared(ctx);
            signed_val = (int16_t)int_val;
            
            /* Sign extend: if negative, high word is 0xFFFF, else 0x0000 */
            if (signed_val < 0) {
                long_val = (uint32_t)(int32_t)signed_val;  /* Sign extend */
            } else {
                long_val = (uint32_t)signed_val;
            }
            
            if (stack_push_long(ctx, long_val) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_L2I: {
            /* long to int conversion (truncate) */
            uint32_t long_val;
            uint16_t int_val;
            
            long_val = stack_pop_long(ctx);
            int_val = (uint16_t)(long_val & 0xFFFF);  /* Keep low 16 bits */
            
            if (stack_push_shared(ctx, int_val) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_LADD: {
            /* Long addition */
            uint32_t value2;
            uint32_t value1;
            uint32_t result;
            
            value2 = stack_pop_long(ctx);
            value1 = stack_pop_long(ctx);
            result = value1 + value2;
            
            if (stack_push_long(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_LSUB: {
            /* Long subtraction */
            uint32_t value2;
            uint32_t value1;
            uint32_t result;
            
            value2 = stack_pop_long(ctx);
            value1 = stack_pop_long(ctx);
            result = value1 - value2;
            
            if (stack_push_long(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_LMUL: {
            /* Long multiplication */
            uint32_t value2;
            uint32_t value1;
            uint32_t result;
            
            value2 = stack_pop_long(ctx);
            value1 = stack_pop_long(ctx);
            result = value1 * value2;
            
            if (stack_push_long(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_LDIV: {
            /* Long division with zero check */
            uint32_t value2;
            uint32_t value1;
            int32_t signed1;
            int32_t signed2;
            int32_t result;
            
            value2 = stack_pop_long(ctx);
            value1 = stack_pop_long(ctx);
            
            /* Check for division by zero */
            if (value2 == 0) {
                if (throw_runtime_exception(ctx, "Division by zero") != 0) {
                    return -1;
                }
                break;
            }
            
            /* Signed division */
            signed1 = (int32_t)value1;
            signed2 = (int32_t)value2;
            result = signed1 / signed2;
            
            if (stack_push_long(ctx, (uint32_t)result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_LMOD: {
            /* Long modulo with zero check */
            uint32_t value2;
            uint32_t value1;
            int32_t signed1;
            int32_t signed2;
            int32_t result;
            
            value2 = stack_pop_long(ctx);
            value1 = stack_pop_long(ctx);
            
            /* Check for division by zero */
            if (value2 == 0) {
                if (throw_runtime_exception(ctx, "Division by zero") != 0) {
                    return -1;
                }
                break;
            }
            
            /* Signed modulo */
            signed1 = (int32_t)value1;
            signed2 = (int32_t)value2;
            result = signed1 % signed2;
            
            if (stack_push_long(ctx, (uint32_t)result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_LNEG: {
            /* Long negation (two's complement) */
            uint32_t value;
            int32_t signed_val;
            int32_t result;
            
            value = stack_pop_long(ctx);
            signed_val = (int32_t)value;
            result = -signed_val;
            
            if (stack_push_long(ctx, (uint32_t)result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_LCMP: {
            /* Long comparison: returns -1, 0, or 1 */
            uint32_t value2;
            uint32_t value1;
            int32_t signed1;
            int32_t signed2;
            uint16_t result;
            
            value2 = stack_pop_long(ctx);
            value1 = stack_pop_long(ctx);
            
            signed1 = (int32_t)value1;
            signed2 = (int32_t)value2;
            
            if (signed1 < signed2) {
                result = (uint16_t)-1;  /* 0xFFFF */
            } else if (signed1 > signed2) {
                result = 1;
            } else {
                result = 0;
            }
            
            if (stack_push_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        /* ===== Long Array Operations ===== */
        
        case OP_NEW_LONG_ARRAY: {
            /* Create new long[] array
             * Stack: [size] -> [array_ref]
             * Memory layout: [length:2][elem0_high:2][elem0_low:2]...
             * Each long element uses 2 words (4 bytes)
             */
            uint16_t size;
            uint16_t total_size;
            uint16_t* array_data;
            uint16_t array_handle;
            
            size = stack_pop_shared(ctx);
            
            /* Validate size */
            if (size > 8192) {  /* Reasonable limit for DOS */
                printf("ERROR: Array size too large: %u\n", size);
                return -1;
            }
            
            /* Calculate total size: (size * 2 + 1) * 2 bytes
             * size * 2 = number of words for all long elements
             * + 1 = length field
             * * 2 = convert words to bytes
             */
            total_size = (uint16_t)((size * 2 + 1) * sizeof(uint16_t));
            array_data = (uint16_t*)memory_alloc(total_size);
            if (array_data == NULL) {
                printf("ERROR: Out of memory allocating long array\n");
                return -1;
            }
            
            /* Initialize array: length + all elements to 0 */
            memset(array_data, 0, total_size);
            array_data[0] = size;  /* Store element count (not word count) */
            
            /* Allocate handle for array pointer */
            array_handle = memory_alloc_array_handle(array_data);
            if (array_handle == 0) {
                printf("ERROR: Out of array handles\n");
                memory_free(array_data);
                return -1;
            }
            
            if (stack_push_shared(ctx, array_handle) != 0) {
                printf("ERROR: Stack overflow\n");
                memory_free_array_handle(array_handle);
                memory_free(array_data);
                return -1;
            }
            break;
        }
        
        case OP_LARRAY_LOAD: {
            /* Load long element from array
             * Stack: [array_ref, index] -> [high, low]
             * Pushes 2 words: high word first, then low word
             */
            uint16_t index;
            uint16_t array_handle;
            uint16_t* array_data;
            uint16_t length;
            uint16_t offset;
            uint16_t high;
            uint16_t low;
            
            index = stack_pop_shared(ctx);
            array_handle = stack_pop_shared(ctx);
            array_data = (uint16_t*)memory_get_array_ptr(array_handle);
            
            if (array_data == NULL) {
                printf("ERROR: Null array reference (LARRAY_LOAD)\n");
                return -1;
            }
            
            length = array_data[0];
            if (index >= length) {
                printf("ERROR: Array index out of bounds (index=%u, length=%u)\n",
                       index, length);
                return -1;
            }
            
            /* Calculate offset: 1 (length) + index * 2 (each long = 2 words) */
            offset = 1 + (index * 2);
            high = array_data[offset];
            low = array_data[offset + 1];
            
            /* Push high word first, then low word */
            if (stack_push_shared(ctx, high) != 0 ||
                stack_push_shared(ctx, low) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_LARRAY_STORE: {
            /* Store long element into array
             * Stack: [array_ref, index, high, low] -> [high, low]
             * Pops 4 values, stores long, pushes value back for assignment result
             */
            uint16_t low;
            uint16_t high;
            uint16_t index;
            uint16_t array_handle;
            uint16_t* array_data;
            uint16_t length;
            uint16_t offset;
            
            low = stack_pop_shared(ctx);
            high = stack_pop_shared(ctx);
            index = stack_pop_shared(ctx);
            array_handle = stack_pop_shared(ctx);
            
            array_data = (uint16_t*)memory_get_array_ptr(array_handle);
            
            if (array_data == NULL) {
                printf("ERROR: Null array reference (LARRAY_STORE)\n");
                return -1;
            }
            
            length = array_data[0];
            if (index >= length) {
                printf("ERROR: Array index out of bounds (index=%u, length=%u)\n",
                       index, length);
                return -1;
            }
            
            /* Calculate offset: 1 (length) + index * 2 (each long = 2 words) */
            offset = 1 + (index * 2);
            array_data[offset] = high;
            array_data[offset + 1] = low;
            
            /* Push value back for assignment expression result */
            if (stack_push_shared(ctx, high) != 0 ||
                stack_push_shared(ctx, low) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        /* ===== Float Operations ===== */
        
        case OP_FCONST_0: {
            /* Push float constant 0.0f */
            if (stack_push_float_shared(ctx, 0.0f) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FCONST_1: {
            /* Push float constant 1.0f */
            if (stack_push_float_shared(ctx, 1.0f) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FCONST_2: {
            /* Push float constant 2.0f */
            if (stack_push_float_shared(ctx, 2.0f) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FCONST: {
            /* Push float constant from bytecode
             * Format: [opcode:1] [high:2] [low:2]
             * Note: Compiler emits in little-endian format
             */
            uint16_t high;
            uint16_t low;
            uint32_t bits;
            float value;
            
            /* Read little-endian (low byte first) */
            high = (uint16_t)(ctx->pc[0]) | ((uint16_t)(ctx->pc[1]) << 8);
            low = (uint16_t)(ctx->pc[2]) | ((uint16_t)(ctx->pc[3]) << 8);
            ctx->pc += 4;
            
            bits = ((uint32_t)high << 16) | low;
            memcpy(&value, &bits, sizeof(float));
            
            if (stack_push_float_shared(ctx, value) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FADD: {
            /* Float addition */
            float value2;
            float value1;
            float result;
            
            value2 = stack_pop_float_shared(ctx);
            value1 = stack_pop_float_shared(ctx);
            result = value1 + value2;
            
            if (stack_push_float_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FSUB: {
            /* Float subtraction */
            float value2;
            float value1;
            float result;
            
            value2 = stack_pop_float_shared(ctx);
            value1 = stack_pop_float_shared(ctx);
            result = value1 - value2;
            
            if (stack_push_float_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FMUL: {
            /* Float multiplication */
            float value2;
            float value1;
            float result;
            
            value2 = stack_pop_float_shared(ctx);
            value1 = stack_pop_float_shared(ctx);
            result = value1 * value2;
            
            if (stack_push_float_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FDIV: {
            /* Float division */
            float value2;
            float value1;
            float result;
            
            value2 = stack_pop_float_shared(ctx);
            value1 = stack_pop_float_shared(ctx);
            
            /* IEEE 754 handles division by zero (returns Infinity or NaN) */
            result = value1 / value2;
            
            if (stack_push_float_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FREM: {
            /* Float remainder (modulo)
             * Uses fmod() from math.h for IEEE 754 compliance
             */
            float value2;
            float value1;
            float result;
            
            value2 = stack_pop_float_shared(ctx);
            value1 = stack_pop_float_shared(ctx);
            
            /* Use fmod for proper IEEE 754 remainder */
            result = (float)fmod((double)value1, (double)value2);
            
            if (stack_push_float_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FNEG: {
            /* Float negation */
            float value;
            float result;
            
            value = stack_pop_float_shared(ctx);
            result = -value;
            
            if (stack_push_float_shared(ctx, result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FCMPG: {
            /* Float compare (NaN returns 1)
             * Returns: -1 if value1 < value2
             *           0 if value1 == value2
             *           1 if value1 > value2 or either is NaN
             */
            float value2;
            float value1;
            int16_t result;
            
            value2 = stack_pop_float_shared(ctx);
            value1 = stack_pop_float_shared(ctx);
            
            /* Check for NaN (NaN != NaN is true) */
            if (value1 != value1 || value2 != value2) {
                result = 1;  /* NaN bias: return 1 */
            } else if (value1 < value2) {
                result = -1;
            } else if (value1 > value2) {
                result = 1;
            } else {
                result = 0;
            }
            
            if (stack_push_shared(ctx, (uint16_t)result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FCMPL: {
            /* Float compare (NaN returns -1)
             * Returns: -1 if value1 < value2 or either is NaN
             *           0 if value1 == value2
             *           1 if value1 > value2
             */
            float value2;
            float value1;
            int16_t result;
            
            value2 = stack_pop_float_shared(ctx);
            value1 = stack_pop_float_shared(ctx);
            
            /* Check for NaN (NaN != NaN is true) */
            if (value1 != value1 || value2 != value2) {
                result = -1;  /* NaN bias: return -1 */
            } else if (value1 < value2) {
                result = -1;
            } else if (value1 > value2) {
                result = 1;
            } else {
                result = 0;
            }
            
            if (stack_push_shared(ctx, (uint16_t)result) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_I2F: {
            /* Convert int to float */
            int16_t int_val;
            float float_val;
            
            int_val = (int16_t)stack_pop_shared(ctx);
            float_val = (float)int_val;
            
            if (stack_push_float_shared(ctx, float_val) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_L2F: {
            /* Convert long to float */
            uint32_t long_val;
            int32_t signed_long;
            float float_val;
            
            long_val = stack_pop_long(ctx);
            signed_long = (int32_t)long_val;
            float_val = (float)signed_long;
            
            if (stack_push_float_shared(ctx, float_val) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_F2I: {
            /* Convert float to int (truncate towards zero) */
            float float_val;
            int16_t int_val;
            
            float_val = stack_pop_float_shared(ctx);
            
            /* Truncate towards zero */
            if (float_val >= 32767.0f) {
                int_val = 32767;  /* Max int16_t */
            } else if (float_val <= -32768.0f) {
                int_val = -32768;  /* Min int16_t */
            } else if (float_val != float_val) {
                int_val = 0;  /* NaN -> 0 */
            } else {
                int_val = (int16_t)float_val;
            }
            
            if (stack_push_shared(ctx, (uint16_t)int_val) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_F2L: {
            /* Convert float to long (truncate towards zero) */
            float float_val;
            int32_t long_val;
            
            float_val = stack_pop_float_shared(ctx);
            
            /* Truncate towards zero */
            if (float_val >= 2147483647.0f) {
                long_val = 2147483647L;  /* Max int32_t */
            } else if (float_val <= -2147483648.0f) {
                long_val = -2147483648L;  /* Min int32_t */
            } else if (float_val != float_val) {
                long_val = 0;  /* NaN -> 0 */
            } else {
                long_val = (int32_t)float_val;
            }
            
            if (stack_push_long(ctx, (uint32_t)long_val) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_LOAD_FLOAT: {
            /* Load float from local variable (2 slots)
             * Format: [opcode:1] [idx:1]
             * Loads from shared_locals[local_base + idx] and [local_base + idx + 1]
             */
            uint8_t idx;
            uint16_t local_base;
            uint8_t local_count;
            uint16_t high;
            uint16_t low;
            
            idx = *ctx->pc++;
            
            /* Get current frame's local base and count */
            if (ctx->call_depth > 0) {
                local_base = ctx->call_frames[ctx->call_depth - 1].local_base;
                local_count = ctx->call_frames[ctx->call_depth - 1].local_count;
            } else {
                local_base = 0;
                local_count = ctx->local_pointer;
            }
            
            if (idx + 1 >= local_count) {
                printf("ERROR: Local variable index out of bounds: %u\n", idx);
                return -1;
            }
            
            high = ctx->shared_locals[local_base + idx];
            low = ctx->shared_locals[local_base + idx + 1];
            
            if (stack_push_shared(ctx, high) != 0 ||
                stack_push_shared(ctx, low) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_STORE_FLOAT: {
            /* Store float to local variable (2 slots)
             * Format: [opcode:1] [idx:1]
             * Stores to shared_locals[local_base + idx] and [local_base + idx + 1]
             */
            uint8_t idx;
            uint16_t local_base;
            uint8_t local_count;
            uint16_t low;
            uint16_t high;
            
            idx = *ctx->pc++;
            
            /* Get current frame's local base and count */
            if (ctx->call_depth > 0) {
                local_base = ctx->call_frames[ctx->call_depth - 1].local_base;
                local_count = ctx->call_frames[ctx->call_depth - 1].local_count;
            } else {
                local_base = 0;
                local_count = ctx->local_pointer;
            }
            
            if (idx + 1 >= local_count) {
                printf("ERROR: Local variable index out of bounds: %u\n", idx);
                return -1;
            }
            
            low = stack_pop_shared(ctx);
            high = stack_pop_shared(ctx);
            
            ctx->shared_locals[local_base + idx] = high;
            ctx->shared_locals[local_base + idx + 1] = low;
            break;
        }
        
        /* ===== Float Array Operations ===== */
        
        case OP_NEW_FLOAT_ARRAY: {
            /* Create new float[] array
             * Stack: [size] -> [array_ref]
             * Memory layout: [length:2][elem0_high:2][elem0_low:2]...
             * Each float element uses 2 words (4 bytes)
             */
            uint16_t size;
            uint16_t total_size;
            uint16_t* array_data;
            uint16_t array_handle;
            
            size = stack_pop_shared(ctx);
            
            /* Validate size */
            if (size > 8192) {  /* Reasonable limit for DOS */
                printf("ERROR: Float array size too large: %u\n", size);
                return -1;
            }
            
            /* Calculate total size: (size * 2 + 1) * 2 bytes
             * size * 2 = number of words for all float elements
             * + 1 = length field
             * * 2 = convert words to bytes
             */
            total_size = (uint16_t)((size * 2 + 1) * sizeof(uint16_t));
            array_data = (uint16_t*)memory_alloc(total_size);
            if (array_data == NULL) {
                printf("ERROR: Out of memory allocating float array\n");
                return -1;
            }
            
            /* Initialize array: length + all elements to 0 */
            memset(array_data, 0, total_size);
            array_data[0] = size;  /* Store element count (not word count) */
            
            /* Allocate handle for array pointer */
            array_handle = memory_alloc_array_handle(array_data);
            if (array_handle == 0) {
                printf("ERROR: Out of array handles\n");
                memory_free(array_data);
                return -1;
            }
            
            if (stack_push_shared(ctx, array_handle) != 0) {
                printf("ERROR: Stack overflow\n");
                memory_free_array_handle(array_handle);
                memory_free(array_data);
                return -1;
            }
            break;
        }
        
        case OP_FARRAY_LOAD: {
            /* Load float element from array
             * Stack: [array_ref, index] -> [high, low]
             * Pushes 2 words: high word first, then low word
             */
            uint16_t index;
            uint16_t array_handle;
            uint16_t* array_data;
            uint16_t length;
            uint16_t offset;
            uint16_t high;
            uint16_t low;
            
            index = stack_pop_shared(ctx);
            array_handle = stack_pop_shared(ctx);
            array_data = (uint16_t*)memory_get_array_ptr(array_handle);
            
            if (array_data == NULL) {
                printf("ERROR: Null array reference (FARRAY_LOAD)\n");
                return -1;
            }
            
            length = array_data[0];
            if (index >= length) {
                printf("ERROR: Float array index out of bounds (index=%u, length=%u)\n",
                       index, length);
                return -1;
            }
            
            /* Calculate offset: 1 (length) + index * 2 (each float = 2 words) */
            offset = 1 + (index * 2);
            high = array_data[offset];
            low = array_data[offset + 1];
            
            /* Push high word first, then low word */
            if (stack_push_shared(ctx, high) != 0 ||
                stack_push_shared(ctx, low) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
            break;
        }
        
        case OP_FARRAY_STORE: {
            /* Store float element into array
             * Stack: [array_ref, index, high, low] -> [high, low]
             * Pops 4 values, stores float, pushes value back for assignment result
             */
            uint16_t low;
            uint16_t high;
            uint16_t index;
            uint16_t array_handle;
            uint16_t* array_data;
            uint16_t length;
            uint16_t offset;
            
            low = stack_pop_shared(ctx);
            high = stack_pop_shared(ctx);
            index = stack_pop_shared(ctx);
            array_handle = stack_pop_shared(ctx);
            
            array_data = (uint16_t*)memory_get_array_ptr(array_handle);
            
            if (array_data == NULL) {
                printf("ERROR: Null array reference (FARRAY_STORE)\n");
                return -1;
            }
            
            length = array_data[0];
            if (index >= length) {
                printf("ERROR: Float array index out of bounds (index=%u, length=%u)\n",
                       index, length);
                return -1;
            }
            
            /* Calculate offset: 1 (length) + index * 2 (each float = 2 words) */
            offset = 1 + (index * 2);
            array_data[offset] = high;
            array_data[offset + 1] = low;
            
            /* Push value back for assignment expression result */
            if (stack_push_shared(ctx, high) != 0 ||
                stack_push_shared(ctx, low) != 0) {
                printf("ERROR: Stack overflow\n");
                return -1;
            }
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


