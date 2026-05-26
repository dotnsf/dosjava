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
#define OP_INVOKE_STATIC 0x40  /* Static method call [method:2] [argc:1] */
#define OP_INVOKE_VIRTUAL 0x41 /* Virtual method call [method:2] */
#define OP_RETURN        0x42  /* Return void */
#define OP_RETURN_VALUE  0x43  /* Return with value */

/* Object Operations (0x50-0x5F) */
#define OP_NEW           0x50  /* Create new object [class:2] */
#define OP_GET_FIELD     0x51  /* Read instance field [field:2] */
#define OP_PUT_FIELD     0x52  /* Write instance field [field:2] */
#define OP_GET_STATIC    0x53  /* Read static field [field:2] */
#define OP_PUT_STATIC    0x54  /* Write static field [field:2] */
#define OP_ACONST_NULL   0x55  /* Push null reference */

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

/* Exception Handling (0x90-0x9F) */
#define OP_TRY_BEGIN     0x90  /* Mark try block begin [catch_offset:2] */
#define OP_TRY_END       0x91  /* Mark try block end */
#define OP_CATCH_BEGIN   0x92  /* Mark catch block begin */
#define OP_CATCH_END     0x93  /* Mark catch block end */
#define OP_FINALLY_BEGIN 0x94  /* Mark finally block begin */
#define OP_FINALLY_END   0x95  /* Mark finally block end */
#define OP_THROW         0x96  /* Throw exception */

/* Long Operations (0xA0-0xAF) */
/* Long values are stored as two 16-bit words on stack: [high:2] [low:2] */
#define OP_PUSH_LONG     0xA0  /* Push 32-bit long constant [high:2] [low:2] */
#define OP_LADD          0xA1  /* Long addition: pop L2, pop L1, push (L1+L2) */
#define OP_LSUB          0xA2  /* Long subtraction: pop L2, pop L1, push (L1-L2) */
#define OP_LMUL          0xA3  /* Long multiplication: pop L2, pop L1, push (L1*L2) */
#define OP_LDIV          0xA4  /* Long division: pop L2, pop L1, push (L1/L2) */
#define OP_LMOD          0xA5  /* Long modulo: pop L2, pop L1, push (L1%L2) */
#define OP_LNEG          0xA6  /* Long negation: pop L, push (-L) */
#define OP_I2L           0xA7  /* int to long: pop int, push long */
#define OP_L2I           0xA8  /* long to int: pop long, push int (truncate) */
#define OP_LCMP          0xA9  /* Long compare: pop L2, pop L1, push (-1/0/1) */
#define OP_LOAD_LONG     0xAA  /* Load long from locals [idx:1] (2 slots) */
#define OP_STORE_LONG    0xAB  /* Store long to locals [idx:1] (2 slots) */

/* Long Array Operations (0xB0-0xBF) */
/* Long arrays store 32-bit values: [length:2][elem0_high:2][elem0_low:2]... */
#define OP_NEW_LONG_ARRAY  0xB0  /* Create long array: [size] -> [array_ref] */
#define OP_LARRAY_LOAD     0xB1  /* Load long from array: [arr,idx] -> [high,low] */
#define OP_LARRAY_STORE    0xB2  /* Store long to array: [arr,idx,high,low] -> [high,low] */

/* Float Operations (0xC0-0xCF) */
/* Float values are stored as two 16-bit words on stack: [high:2] [low:2] */
/* IEEE 754 single precision format */
#define OP_FCONST_0      0xC0  /* Push float constant 0.0f */
#define OP_FCONST_1      0xC1  /* Push float constant 1.0f */
#define OP_FCONST_2      0xC2  /* Push float constant 2.0f */
#define OP_FCONST        0xC3  /* Push float constant [high:2] [low:2] */
#define OP_FADD          0xC4  /* Float addition: pop F2, pop F1, push (F1+F2) */
#define OP_FSUB          0xC5  /* Float subtraction: pop F2, pop F1, push (F1-F2) */
#define OP_FMUL          0xC6  /* Float multiplication: pop F2, pop F1, push (F1*F2) */
#define OP_FDIV          0xC7  /* Float division: pop F2, pop F1, push (F1/F2) */
#define OP_FREM          0xC8  /* Float remainder: pop F2, pop F1, push (F1%F2) */
#define OP_FNEG          0xC9  /* Float negation: pop F, push (-F) */
#define OP_FCMPG         0xCA  /* Float compare (NaN -> 1): pop F2, pop F1, push (-1/0/1) */
#define OP_FCMPL         0xCB  /* Float compare (NaN -> -1): pop F2, pop F1, push (-1/0/1) */
#define OP_I2F           0xCC  /* int to float: pop int, push float */
#define OP_L2F           0xCD  /* long to float: pop long, push float */
#define OP_F2I           0xCE  /* float to int: pop float, push int (truncate) */
#define OP_F2L           0xCF  /* float to long: pop float, push long (truncate) */

/* Float Local Variable Operations (0xD0-0xDF) */
#define OP_LOAD_FLOAT    0xD0  /* Load float from locals [idx:1] (2 slots) */
#define OP_STORE_FLOAT   0xD1  /* Store float to locals [idx:1] (2 slots) */

/* Float Array Operations (0xE0-0xEF) */
/* Float arrays store 32-bit values: [length:2][elem0_high:2][elem0_low:2]... */
#define OP_NEW_FLOAT_ARRAY  0xE0  /* Create float array: [size] -> [array_ref] */
#define OP_FARRAY_LOAD      0xE1  /* Load float from array: [arr,idx] -> [high,low] */
#define OP_FARRAY_STORE     0xE2  /* Store float to array: [arr,idx,high,low] -> [] */

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


