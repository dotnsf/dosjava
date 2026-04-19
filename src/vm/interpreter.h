#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../types.h"
#include "stack.h"
#include "../format/djc.h"

/**
 * Bytecode Interpreter for DOS Java VM
 * 
 * Executes .djc bytecode using a fetch-decode-execute loop
 */

/* Maximum local variables */
#define MAX_LOCALS 256

/**
 * Execution context for a method
 */
typedef struct {
    uint8_t* pc;                 /* Program counter */
    uint8_t* code_start;         /* Start of method code */
    uint16_t code_length;        /* Length of method code */
    Stack* stack;                /* Operand stack */
    uint16_t* locals;            /* Local variables */
    uint16_t local_count;        /* Number of local variables */
    DJCFile* djc_file;           /* Current .djc file */
    uint8_t running;             /* 1 if executing, 0 if stopped */
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
