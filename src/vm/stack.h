#ifndef STACK_H
#define STACK_H

#include "../types.h"

/**
 * Operand Stack for DOS Java VM
 * 
 * Manages the operand stack used during bytecode execution
 */

/* Default stack size */
#define DEFAULT_STACK_SIZE 256

/* Stack value type (16-bit) */
typedef uint16_t StackValue;

/**
 * Stack structure
 */
typedef struct {
    StackValue* data;        /* Stack data array */
    uint16_t size;           /* Maximum stack size */
    uint16_t sp;             /* Stack pointer (top of stack) */
    uint16_t max_sp;         /* Maximum SP reached (for debugging) */
} Stack;

/**
 * Initialize a stack
 * @param stack Stack structure to initialize
 * @param size Maximum stack size (0 = use default)
 * @return 0 on success, -1 on error
 */
int stack_init(Stack* stack, uint16_t size);

/**
 * Free stack resources
 * @param stack Stack to free
 */
void stack_free(Stack* stack);

/**
 * Push a value onto the stack
 * @param stack Stack
 * @param value Value to push
 * @return 0 on success, -1 on stack overflow
 */
int stack_push(Stack* stack, StackValue value);

/**
 * Pop a value from the stack
 * @param stack Stack
 * @return Popped value, or 0 if stack is empty
 */
StackValue stack_pop(Stack* stack);

/**
 * Peek at top of stack without popping
 * @param stack Stack
 * @return Top value, or 0 if stack is empty
 */
StackValue stack_peek(Stack* stack);

/**
 * Peek at value at offset from top
 * @param stack Stack
 * @param offset Offset from top (0 = top, 1 = second, etc.)
 * @return Value at offset, or 0 if out of bounds
 */
StackValue stack_peek_at(Stack* stack, uint16_t offset);

/**
 * Duplicate top of stack
 * @param stack Stack
 * @return 0 on success, -1 on error
 */
int stack_dup(Stack* stack);

/**
 * Get current stack depth
 * @param stack Stack
 * @return Number of values on stack
 */
uint16_t stack_depth(Stack* stack);

/**
 * Check if stack is empty
 * @param stack Stack
 * @return 1 if empty, 0 otherwise
 */
uint8_t stack_is_empty(Stack* stack);

/**
 * Check if stack is full
 * @param stack Stack
 * @return 1 if full, 0 otherwise
 */
uint8_t stack_is_full(Stack* stack);

/**
 * Clear the stack
 * @param stack Stack
 */
void stack_clear(Stack* stack);

/**
 * Print stack contents (for debugging)
 * @param stack Stack
 */
void stack_print(Stack* stack);

/**
 * Get stack statistics
 * @param stack Stack
 * @param current_depth Output: current depth
 * @param max_depth Output: maximum depth reached
 */
void stack_stats(Stack* stack, uint16_t* current_depth, uint16_t* max_depth);

#endif /* STACK_H */
