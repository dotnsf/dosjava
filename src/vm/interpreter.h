#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../types.h"
#include "../format/djc.h"

/**
 * Bytecode Interpreter for DOS Java VM
 *
 * Executes .djc bytecode using a fetch-decode-execute loop
 * Uses shared stack design to minimize memory usage in DOS environment
 */

/* Maximum call depth (nested method calls) */
#define MAX_CALL_DEPTH 4

/* Shared stack size (2048 slots = 4KB) */
#define SHARED_STACK_SIZE 2048

/* Shared locals size (128 slots = 256 bytes) */
#define SHARED_LOCALS_SIZE 128

/**
 * Call frame structure
 * Stores state for each method invocation
 */
typedef struct {
    uint8_t* return_pc;         /* Return address */
    uint8_t* return_code_start; /* Code start for return context */
    uint16_t return_code_length;/* Code length for return context */
    uint16_t frame_pointer;     /* Stack frame base offset */
    uint16_t local_base;        /* Local variables base offset */
    uint8_t local_count;        /* Number of locals in this frame */
} CallFrame;

/**
 * Execution context for bytecode execution
 * Uses shared stack design to minimize memory usage
 */
typedef struct {
    /* Program counter */
    uint8_t* pc;                /* Current instruction pointer */
    uint8_t* code_start;        /* Start of method code */
    uint16_t code_length;       /* Length of method code */
    
    /* DJC file reference */
    DJCFile* djc_file;          /* Current .djc file */
    
    /* Shared operand stack */
    uint16_t shared_stack[SHARED_STACK_SIZE];
    uint16_t stack_pointer;     /* Current stack position */
    
    /* Shared local variables */
    uint16_t shared_locals[SHARED_LOCALS_SIZE];
    uint16_t local_pointer;     /* Current locals position */
    
    /* Call frames */
    CallFrame call_frames[MAX_CALL_DEPTH];
    uint8_t call_depth;         /* Current call depth (0-3) */
    
    /* Execution state */
    int running;                /* 1 if executing, 0 if stopped */
} ExecutionContext;

/**
 * Initialize execution context
 * @param ctx Context to initialize
 * @param djc_file DJC file containing code
 * @param method Method to execute
 * @return 0 on success, -1 on error
 */
int interpreter_init_context(ExecutionContext* ctx, DJCFile* djc_file, DJCMethod* method);

/**
 * Free execution context resources
 * @param ctx Context to free
 */
void interpreter_free_context(ExecutionContext* ctx);

/**
 * Execute a method
 * @param ctx Execution context
 * @return 0 on success, -1 on error
 */
int interpreter_execute(ExecutionContext* ctx);

/**
 * Execute a single instruction
 * @param ctx Execution context
 * @return 0 to continue, 1 to return, -1 on error
 */
int interpreter_step(ExecutionContext* ctx);

/**
 * Get local variable
 * @param ctx Execution context
 * @param index Local variable index
 * @return Variable value
 */
uint16_t interpreter_get_local(ExecutionContext* ctx, uint16_t index);

/**
 * Set local variable
 * @param ctx Execution context
 * @param index Local variable index
 * @param value Value to set
 */
void interpreter_set_local(ExecutionContext* ctx, uint16_t index, uint16_t value);

/**
 * Read 8-bit operand from code
 * @param ctx Execution context
 * @return Operand value
 */
uint8_t interpreter_read_u8(ExecutionContext* ctx);

/**
 * Read 16-bit operand from code
 * @param ctx Execution context
 * @return Operand value
 */
uint16_t interpreter_read_u16(ExecutionContext* ctx);

/**
 * Print execution context state (for debugging)
 * @param ctx Execution context
 */
void interpreter_print_state(ExecutionContext* ctx);

#endif /* INTERPRETER_H */

// Made with Bob
