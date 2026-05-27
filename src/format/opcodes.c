#include "opcodes.h"
#include <stddef.h>

/**
 * Opcode name table for debugging
 */
static const char* opcode_names[] = {
    /* 0x00-0x0F: Stack Operations */
    "NOP",           /* 0x00 */
    "PUSH_CONST",    /* 0x01 */
    "PUSH_INT",      /* 0x02 */
    "POP",           /* 0x03 */
    "DUP",           /* 0x04 */
    "LOAD_LOCAL",    /* 0x05 */
    "STORE_LOCAL",   /* 0x06 */
    "LOAD_0",        /* 0x07 */
    "LOAD_1",        /* 0x08 */
    "LOAD_2",        /* 0x09 */
    "STORE_0",       /* 0x0A */
    "STORE_1",       /* 0x0B */
    "STORE_2",       /* 0x0C */
    "UNKNOWN_0D",    /* 0x0D */
    "UNKNOWN_0E",    /* 0x0E */
    "UNKNOWN_0F",    /* 0x0F */
    
    /* 0x10-0x1F: Arithmetic Operations */
    "ADD",           /* 0x10 */
    "SUB",           /* 0x11 */
    "MUL",           /* 0x12 */
    "DIV",           /* 0x13 */
    "MOD",           /* 0x14 */
    "NEG",           /* 0x15 */
    "INC_LOCAL",     /* 0x16 */
    "UNKNOWN_17",    /* 0x17 */
    "UNKNOWN_18",    /* 0x18 */
    "UNKNOWN_19",    /* 0x19 */
    "UNKNOWN_1A",    /* 0x1A */
    "UNKNOWN_1B",    /* 0x1B */
    "UNKNOWN_1C",    /* 0x1C */
    "UNKNOWN_1D",    /* 0x1D */
    "UNKNOWN_1E",    /* 0x1E */
    "UNKNOWN_1F",    /* 0x1F */
    
    /* 0x20-0x2F: Comparison Operations */
    "CMP_EQ",        /* 0x20 */
    "CMP_NE",        /* 0x21 */
    "CMP_LT",        /* 0x22 */
    "CMP_LE",        /* 0x23 */
    "CMP_GT",        /* 0x24 */
    "CMP_GE",        /* 0x25 */
    "UNKNOWN_26",    /* 0x26 */
    "UNKNOWN_27",    /* 0x27 */
    "UNKNOWN_28",    /* 0x28 */
    "UNKNOWN_29",    /* 0x29 */
    "UNKNOWN_2A",    /* 0x2A */
    "UNKNOWN_2B",    /* 0x2B */
    "UNKNOWN_2C",    /* 0x2C */
    "UNKNOWN_2D",    /* 0x2D */
    "UNKNOWN_2E",    /* 0x2E */
    "UNKNOWN_2F",    /* 0x2F */
    
    /* 0x30-0x3F: Control Flow */
    "GOTO",          /* 0x30 */
    "IF_TRUE",       /* 0x31 */
    "IF_FALSE",      /* 0x32 */
    "IF_EQ",         /* 0x33 */
    "IF_NE",         /* 0x34 */
    "IF_LT",         /* 0x35 */
    "IF_LE",         /* 0x36 */
    "IF_GT",         /* 0x37 */
    "IF_GE",         /* 0x38 */
    "UNKNOWN_39",    /* 0x39 */
    "UNKNOWN_3A",    /* 0x3A */
    "UNKNOWN_3B",    /* 0x3B */
    "UNKNOWN_3C",    /* 0x3C */
    "UNKNOWN_3D",    /* 0x3D */
    "UNKNOWN_3E",    /* 0x3E */
    "UNKNOWN_3F",    /* 0x3F */
    
    /* 0x40-0x4F: Method Invocation */
    "INVOKE_STATIC", /* 0x40 */
    "INVOKE_VIRTUAL",/* 0x41 */
    "RETURN",        /* 0x42 */
    "RETURN_VALUE",  /* 0x43 */
};

/**
 * Get instruction name for debugging
 */
const char* opcode_name(uint8_t opcode) {
    if (opcode < sizeof(opcode_names) / sizeof(opcode_names[0])) {
        return opcode_names[opcode];
    }
    
    /* Special opcodes */
    switch (opcode) {
        case OP_NEW:           return "NEW";
        case OP_GET_FIELD:     return "GET_FIELD";
        case OP_PUT_FIELD:     return "PUT_FIELD";
        case OP_GET_STATIC:    return "GET_STATIC";
        case OP_PUT_STATIC:    return "PUT_STATIC";
        case OP_ACONST_NULL:   return "ACONST_NULL";
        case OP_NEW_ARRAY:     return "NEW_ARRAY";
        case OP_ARRAY_LENGTH:  return "ARRAY_LENGTH";
        case OP_ARRAY_LOAD:    return "ARRAY_LOAD";
        case OP_ARRAY_STORE:   return "ARRAY_STORE";
        case OP_INSTANCEOF:    return "INSTANCEOF";
        case OP_CHECKCAST:     return "CHECKCAST";
        case OP_PRINT_INT:     return "PRINT_INT";
        case OP_PRINT_CHAR:    return "PRINT_CHAR";
        case OP_TRY_BEGIN:     return "TRY_BEGIN";
        case OP_TRY_END:       return "TRY_END";
        case OP_CATCH_BEGIN:   return "CATCH_BEGIN";
        case OP_CATCH_END:     return "CATCH_END";
        case OP_FINALLY_BEGIN: return "FINALLY_BEGIN";
        case OP_FINALLY_END:   return "FINALLY_END";
        case OP_THROW:         return "THROW";
        case OP_EXCEPTION_TO_STRING: return "EXCEPTION_TO_STRING";
        case OP_PUSH_LONG:     return "PUSH_LONG";
        case OP_LADD:          return "LADD";
        case OP_LSUB:          return "LSUB";
        case OP_LMUL:          return "LMUL";
        case OP_LDIV:          return "LDIV";
        case OP_LMOD:          return "LMOD";
        case OP_LNEG:          return "LNEG";
        case OP_I2L:           return "I2L";
        case OP_L2I:           return "L2I";
        case OP_LCMP:          return "LCMP";
        case OP_LOAD_LONG:     return "LOAD_LONG";
        case OP_STORE_LONG:    return "STORE_LONG";
        case OP_NEW_LONG_ARRAY: return "NEW_LONG_ARRAY";
        case OP_LARRAY_LOAD:   return "LARRAY_LOAD";
        case OP_LARRAY_STORE:  return "LARRAY_STORE";
        case OP_FCONST_0:      return "FCONST_0";
        case OP_FCONST_1:      return "FCONST_1";
        case OP_FCONST_2:      return "FCONST_2";
        case OP_FCONST:        return "FCONST";
        case OP_FADD:          return "FADD";
        case OP_FSUB:          return "FSUB";
        case OP_FMUL:          return "FMUL";
        case OP_FDIV:          return "FDIV";
        case OP_FREM:          return "FREM";
        case OP_FNEG:          return "FNEG";
        case OP_FCMPG:         return "FCMPG";
        case OP_FCMPL:         return "FCMPL";
        case OP_I2F:           return "I2F";
        case OP_L2F:           return "L2F";
        case OP_F2I:           return "F2I";
        case OP_F2L:           return "F2L";
        case OP_LOAD_FLOAT:    return "LOAD_FLOAT";
        case OP_STORE_FLOAT:   return "STORE_FLOAT";
        case OP_NEW_FLOAT_ARRAY: return "NEW_FLOAT_ARRAY";
        case OP_FARRAY_LOAD:   return "FARRAY_LOAD";
        case OP_FARRAY_STORE:  return "FARRAY_STORE";
        case OP_HALT:          return "HALT";
        default:               return "UNKNOWN";
    }
}

/**
 * Get instruction length (including operands)
 */
uint8_t opcode_length(uint8_t opcode) {
    switch (opcode) {
        /* Instructions with no operands (1 byte) */
        case OP_NOP:
        case OP_POP:
        case OP_DUP:
        case OP_LOAD_0:
        case OP_LOAD_1:
        case OP_LOAD_2:
        case OP_STORE_0:
        case OP_STORE_1:
        case OP_STORE_2:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        case OP_NEG:
        case OP_CMP_EQ:
        case OP_CMP_NE:
        case OP_CMP_LT:
        case OP_CMP_LE:
        case OP_CMP_GT:
        case OP_CMP_GE:
        case OP_RETURN:
        case OP_RETURN_VALUE:
        case OP_ARRAY_LENGTH:
        case OP_ARRAY_LOAD:
        case OP_ARRAY_STORE:
        case OP_PRINT_INT:
        case OP_PRINT_CHAR:
        case OP_TRY_END:
        case OP_CATCH_BEGIN:
        case OP_CATCH_END:
        case OP_FINALLY_BEGIN:
        case OP_FINALLY_END:
        case OP_THROW:
        case OP_EXCEPTION_TO_STRING:
        case OP_LADD:
        case OP_LSUB:
        case OP_LMUL:
        case OP_LDIV:
        case OP_LMOD:
        case OP_LNEG:
        case OP_I2L:
        case OP_L2I:
        case OP_LCMP:
        case OP_NEW_LONG_ARRAY:
        case OP_LARRAY_LOAD:
        case OP_LARRAY_STORE:
        case OP_FCONST_0:
        case OP_FCONST_1:
        case OP_FCONST_2:
        case OP_FADD:
        case OP_FSUB:
        case OP_FMUL:
        case OP_FDIV:
        case OP_FREM:
        case OP_FNEG:
        case OP_FCMPG:
        case OP_FCMPL:
        case OP_I2F:
        case OP_L2F:
        case OP_F2I:
        case OP_F2L:
        case OP_NEW_FLOAT_ARRAY:
        case OP_FARRAY_LOAD:
        case OP_FARRAY_STORE:
        case OP_HALT:
            return 1;
        
        /* Instructions with 1-byte operand (2 bytes) */
        case OP_LOAD_LOCAL:
        case OP_STORE_LOCAL:
        case OP_NEW_ARRAY:
        case OP_LOAD_LONG:
        case OP_STORE_LONG:
        case OP_LOAD_FLOAT:
        case OP_STORE_FLOAT:
            return 2;
        
        /* Instructions with 2-byte operand (3 bytes) */
        case OP_PUSH_CONST:
        case OP_PUSH_INT:
        case OP_GOTO:
        case OP_IF_TRUE:
        case OP_IF_FALSE:
        case OP_IF_EQ:
        case OP_IF_NE:
        case OP_IF_LT:
        case OP_IF_LE:
        case OP_IF_GT:
        case OP_IF_GE:
        case OP_INVOKE_VIRTUAL:
        case OP_NEW:
        case OP_GET_FIELD:
        case OP_PUT_FIELD:
        case OP_GET_STATIC:
        case OP_PUT_STATIC:
        case OP_INSTANCEOF:
        case OP_CHECKCAST:
        case OP_TRY_BEGIN:  /* TRY_BEGIN with catch offset */
            return 3;
        
        /* Instructions with 2-byte + 1-byte operands (4 bytes) */
        case OP_INVOKE_STATIC:
            return 4;
        
        /* Instructions with 2 operands (3 bytes) */
        case OP_INC_LOCAL:
            return 3;
        
        /* Instructions with 4-byte operand (5 bytes) */
        case OP_PUSH_LONG:  /* [opcode:1] [high:2] [low:2] */
        case OP_FCONST:     /* [opcode:1] [high:2] [low:2] */
            return 5;
        
        /* Unknown instruction */
        default:
            return 1;
    }
}


