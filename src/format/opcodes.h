#ifndef OPCODES_H
#define OPCODES_H

#include "../types.h"

/**
 * DOS Java Bytecode Instruction Set
 *
 * Optimized for 16-bit architecture with minimal memory footprint
 */

/* Stack Operations (0x00-0x0F) */
#define OP_NOP           0x00  /* No operation */
#define OP_PUSH_CONST    0x01  /* Push constant from pool [idx:2] */
#define OP_PUSH_INT      0x02  /* Push immediate int [value:2] */
#define OP_POP           0x03  /* Pop top of stack */
#define OP_DUP           0x04  /* Duplicate top of stack */
#define OP_LOAD_LOCAL    0x05  /* Load local variable [idx:1] */
#define OP_STORE_LOCAL   0x06  /* Store to local variable [idx:1] */
#define OP_LOAD_0        0x07  /* Load local 0 (optimized) */
#define OP_LOAD_1        0x08  /* Load local 1 (optimized) */
#define OP_LOAD_2        0x09  /* Load local 2 (optimized) */
#define OP_STORE_0       0x0A  /* Store to local 0 (optimized) */
#define OP_STORE_1       0x0B  /* Store to local 1 (optimized) */
#define OP_STORE_2       0x0C  /* Store to local 2 (optimized) */

/* Arithmetic Operations (0x10-0x1F) */
#define OP_ADD           0x10  /* Integer addition */
#define OP_SUB           0x11  /* Integer subtraction */
#define OP_MUL           0x12  /* Integer multiplication */
#define OP_DIV           0x13  /* Integer division */
#define OP_MOD           0x14  /* Integer modulo */
#define OP_NEG           0x15  /* Negate top of stack */
#define OP_INC_LOCAL     0x16  /* Increment local variable [idx:1] [amount:1] */

/* Comparison Operations (0x20-0x2F) */
#define OP_CMP_EQ        0x20  /* Compare equal (pushes 1 or 0) */
#define OP_CMP_NE        0x21  /* Compare not equal */
#define OP_CMP_LT        0x22  /* Compare less than */
#define OP_CMP_LE        0x23  /* Compare less or equal */
#define OP_CMP_GT        0x24  /* Compare greater than */
#define OP_CMP_GE        0x25  /* Compare greater or equal */

/* Control Flow (0x30-0x3F) */
#define OP_GOTO          0x30  /* Unconditional jump [offset:2] */
#define OP_IF_TRUE       0x31  /* Jump if top is true [offset:2] */
#define OP_IF_FALSE      0x32  /* Jump if top is false [offset:2] */
#define OP_IF_EQ         0x33  /* Jump if equal [offset:2] */
#define OP_IF_NE         0x34  /* Jump if not equal [offset:2] */
#define OP_IF_LT         0x35  /* Jump if less than [offset:2] */
#define OP_IF_LE         0x36  /* Jump if less or equal [offset:2] */
#define OP_IF_GT         0x37  /* Jump if greater than [offset:2] */
#define OP_IF_GE         0x38  /* Jump if greater or equal [offset:2] */

/* Method Invocation (0x40-0x4F) */
#define OP_INVOKE_STATIC 0x40  /* Static method call [method:2] */
#define OP_INVOKE_VIRTUAL 0x41 /* Virtual method call [method:2] */
#define OP_RETURN        0x42  /* Return void */
#define OP_RETURN_VALUE  0x43  /* Return with value */

/* Object Operations (0x50-0x5F) */
#define OP_NEW           0x50  /* Create new object [class:2] */
#define OP_GET_FIELD     0x51  /* Read instance field [field:2] */
#define OP_PUT_FIELD     0x52  /* Write instance field [field:2] */
#define OP_GET_STATIC    0x53  /* Read static field [field:2] */
#define OP_PUT_STATIC    0x54  /* Write static field [field:2] */

/* Array Operations (0x60-0x6F) */
#define OP_NEW_ARRAY     0x60  /* Create new array [type:1] */
#define OP_ARRAY_LENGTH  0x61  /* Get array length */
#define OP_ARRAY_LOAD    0x62  /* Load from array */
#define OP_ARRAY_STORE   0x63  /* Store to array */

/* Type Operations (0x70-0x7F) */
#define OP_INSTANCEOF    0x70  /* Check instance type [class:2] */
#define OP_CHECKCAST     0x71  /* Cast to type [class:2] */

/* Special Operations (0x80-0x8F) */
#define OP_PRINT_INT     0x80  /* Print integer (debug) */
#define OP_PRINT_CHAR    0x81  /* Print character (debug) */
#define OP_HALT          0xFF  /* Halt execution */

/**
 * Get instruction name for debugging
 * @param opcode Instruction opcode
 * @return String name of instruction
 */
const char* opcode_name(uint8_t opcode);

/**
 * Get instruction length (including operands)
 * @param opcode Instruction opcode
 * @return Total instruction length in bytes
 */
uint8_t opcode_length(uint8_t opcode);

#endif /* OPCODES_H */

// Made with Bob
