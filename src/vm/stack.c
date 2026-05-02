#include "stack.h"
#include "memory.h"
#include <stdio.h>
#include <string.h>

/**
 * Initialize a stack
 */
int stack_init(Stack* stack, uint16_t size) {
    if (stack == NULL) {
        return -1;
    }
    
    /* Use default size if not specified */
    if (size == 0) {
        size = DEFAULT_STACK_SIZE;
    }
    
    /* Allocate stack data */
    stack->data = (StackValue*)memory_alloc(sizeof(StackValue) * size);
    if (stack->data == NULL) {
        return -1;
    }
    
    /* Initialize stack state */
    stack->size = size;
    stack->sp = 0;
    stack->max_sp = 0;
    
    return 0;
}

/**
 * Free stack resources
 */
void stack_free(Stack* stack) {
    if (stack == NULL) {
        return;
    }
    
    if (stack->data != NULL) {
        memory_free(stack->data);
        stack->data = NULL;
    }
    
    stack->size = 0;
    stack->sp = 0;
    stack->max_sp = 0;
}

/**
 * Push a value onto the stack
 */
int stack_push(Stack* stack, StackValue value) {
    if (stack == NULL || stack->data == NULL) {
        return -1;
    }
    
    /* Check for stack overflow */
    if (stack->sp >= stack->size) {
        return -1;
    }
    
    /* Push value */
    stack->data[stack->sp] = value;
    stack->sp++;
    
    /* Update max SP */
    if (stack->sp > stack->max_sp) {
        stack->max_sp = stack->sp;
    }
    
    return 0;
}

/**
 * Pop a value from the stack
 */
StackValue stack_pop(Stack* stack) {
    if (stack == NULL || stack->data == NULL || stack->sp == 0) {
        return 0;
    }
    
    /* Pop value */
    stack->sp--;
    return stack->data[stack->sp];
}

/**
 * Peek at top of stack without popping
 */
StackValue stack_peek(Stack* stack) {
    if (stack == NULL || stack->data == NULL || stack->sp == 0) {
        return 0;
    }
    
    return stack->data[stack->sp - 1];
}

/**
 * Peek at value at offset from top
 */
StackValue stack_peek_at(Stack* stack, uint16_t offset) {
    if (stack == NULL || stack->data == NULL) {
        return 0;
    }
    
    if (offset >= stack->sp) {
        return 0;
    }
    
    return stack->data[stack->sp - 1 - offset];
}

/**
 * Duplicate top of stack
 */
int stack_dup(Stack* stack) {
    StackValue value;
    
    if (stack == NULL || stack->data == NULL || stack->sp == 0) {
        return -1;
    }
    
    /* Get top value */
    value = stack->data[stack->sp - 1];
    
    /* Push it again */
    return stack_push(stack, value);
}

/**
 * Get current stack depth
 */
uint16_t stack_depth(Stack* stack) {
    if (stack == NULL) {
        return 0;
    }
    
    return stack->sp;
}

/**
 * Check if stack is empty
 */
uint8_t stack_is_empty(Stack* stack) {
    if (stack == NULL) {
        return 1;
    }
    
    return (stack->sp == 0) ? 1 : 0;
}

/**
 * Check if stack is full
 */
uint8_t stack_is_full(Stack* stack) {
    if (stack == NULL) {
        return 1;
    }
    
    return (stack->sp >= stack->size) ? 1 : 0;
}

/**
 * Clear the stack
 */
void stack_clear(Stack* stack) {
    if (stack == NULL) {
        return;
    }
    
    stack->sp = 0;
}

/**
 * Print stack contents (for debugging)
 */
void stack_print(Stack* stack) {
    uint16_t i;
    
    if (stack == NULL || stack->data == NULL) {
        printf("Stack: NULL\n");
        return;
    }
    
    printf("Stack (depth=%u, max=%u):\n", stack->sp, stack->max_sp);
    
    if (stack->sp == 0) {
        printf("  (empty)\n");
        return;
    }
    
    for (i = 0; i < stack->sp; i++) {
        printf("  [%u] = %u (0x%04X)\n", i, stack->data[i], stack->data[i]);
    }
}

/**
 * Get stack statistics
 */
void stack_stats(Stack* stack, uint16_t* current_depth, uint16_t* max_depth) {
    if (stack == NULL) {
        if (current_depth != NULL) *current_depth = 0;
        if (max_depth != NULL) *max_depth = 0;
        return;
    }
    
    if (current_depth != NULL) {
        *current_depth = stack->sp;
    }
    
    if (max_depth != NULL) {
        *max_depth = stack->max_sp;
    }
}


