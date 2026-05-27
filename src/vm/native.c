#include "native.h"
#include "../runtime/system.h"
#include "../runtime/string.h"
#include "../format/djc.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* Global native method registry */
static NativeMethodRegistry g_native_registry;

/**
 * Initialize native method registry
 */
int native_init(void) {
    memset(&g_native_registry, 0, sizeof(NativeMethodRegistry));
    return 0;
}

/**
 * Shutdown native method registry
 */
void native_shutdown(void) {
    /* Nothing to clean up for now */
}

/**
 * Register a native method
 */
int native_register(
    const char* class_name,
    const char* method_name,
    const char* descriptor,
    NativeMethodFunc func,
    uint8_t param_count,
    NativeParamType* param_types,
    NativeReturnType return_type
) {
    NativeMethodDescriptor* method;
    
    if (g_native_registry.method_count >= MAX_NATIVE_METHODS) {
        return -1;  /* Registry full */
    }
    
    method = &g_native_registry.methods[g_native_registry.method_count];
    method->class_name = class_name;
    method->method_name = method_name;
    method->descriptor = descriptor;
    method->func = func;
    method->param_count = param_count;
    method->param_types = param_types;
    method->return_type = return_type;
    
    g_native_registry.method_count++;
    return 0;
}

/**
 * Find a native method by name and descriptor
 */
NativeMethodDescriptor* native_find(
    const char* class_name,
    const char* method_name,
    const char* descriptor
) {
    uint8_t i;
    NativeMethodDescriptor* method;
    
    for (i = 0; i < g_native_registry.method_count; i++) {
        method = &g_native_registry.methods[i];
        
        /* Match method name and descriptor */
        if (strcmp(method->method_name, method_name) == 0 &&
            strcmp(method->descriptor, descriptor) == 0) {
            /* If class name is specified, match it too */
            if (class_name && method->class_name) {
                if (strcmp(method->class_name, class_name) == 0) {
                    return method;
                }
            } else {
                /* No class name specified, match by method name only */
                return method;
            }
        }
    }
    
    return NULL;  /* Not found */
}

/**
 * Invoke a native method
 */
int native_invoke(
    ExecutionContext* ctx,
    NativeMethodDescriptor* method,
    uint16_t* args,
    uint8_t arg_count,
    uint16_t* result
) {
    if (method == NULL || method->func == NULL) {
        return -1;
    }
    
    /* Verify argument count */
    if (arg_count != method->param_count) {
        return -1;
    }
    
    /* Call native function */
    return method->func(ctx, args, arg_count, result);
}

/* ===== Built-in Native Methods ===== */

/**
 * System.out.println(int)
 */
static int native_system_println_int(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    (void)ctx;
    (void)arg_count;
    (void)result;
    
    system_println_int((int16_t)args[0]);
    return 0;
}

/**
 * System.out.println(long)
 */
static int native_system_println_long(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint32_t long_value;
    uint16_t high, low;
    int32_t signed_value;
    
    (void)ctx;
    (void)result;
    
    /* Long is passed as 2 words: args[0]=high, args[1]=low */
    if (arg_count >= 2) {
        high = args[0];
        low = args[1];
        long_value = ((uint32_t)high << 16) | (uint32_t)low;
        /* Convert to signed 32-bit for proper display */
        signed_value = (int32_t)long_value;
        
        printf("%ld\n", (long)signed_value);
    } else {
        printf("ERROR: println(long) requires 2 arguments\n");
    }
    
    return 0;
}

/**
 * System.out.println(float)
 */
static int native_system_println_float(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float float_value;
    uint32_t float_bits;
    
    (void)ctx;
    (void)result;
    
    if (arg_count >= 2) {
        /* Float is stored as 2 words: [high:2] [low:2] */
        float_bits = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
        
        /* Convert bits to float using memcpy for safe type punning */
        memcpy(&float_value, &float_bits, sizeof(float));
        
        /* Print float value */
        printf("%.2f\n", float_value);
    } else {
        printf("ERROR: println(float) requires 2 arguments\n");
    }
    
    return 0;
}

/**
 * System.out.println(String)
 */
static int native_system_println_string(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t value;
    const char* str;
    
    (void)arg_count;
    (void)result;
    
    value = args[0];
    str = NULL;
    
    /* Value is UTF8 constant index */
    if (value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[value].tag == CONST_UTF8) {
            str = ctx->djc_file->constants[value].data.utf8_data;
        }
    }
    
    if (str) {
        system_println_cstr(str);
    } else {
        return -1;
    }
    
    return 0;
}

/**
 * System.out.print(int)
 */
static int native_system_print_int(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    (void)ctx;
    (void)arg_count;
    (void)result;
    
    system_print_int((int16_t)args[0]);
    return 0;
}

/**
 * System.out.print(long)
 */
static int native_system_print_long(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    int32_t value;
    
    (void)ctx;
    (void)arg_count;
    (void)result;
    
    /* Long value is passed as two 16-bit words: args[0]=high, args[1]=low */
    value = ((int32_t)args[0] << 16) | args[1];
    system_print_long(value);
    return 0;
}

/**
 * System.out.print(String)
 */
static int native_system_print_string(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t value;
    const char* str;
    
    (void)arg_count;
    (void)result;
    
    value = args[0];
    str = NULL;
    
    /* Value is UTF8 constant index */
    if (value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[value].tag == CONST_UTF8) {
            str = ctx->djc_file->constants[value].data.utf8_data;
        }
    }
    
    if (str) {
        system_print_cstr(str);
    } else {
        return -1;
    }
    
    return 0;
}

/**
 * String.length()
 */
static int native_string_length(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t value;
    const char* str;
    uint16_t len;
    
    (void)arg_count;
    
    value = args[0];
    str = NULL;
    
    /* Value is UTF8 constant index */
    if (value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[value].tag == CONST_UTF8) {
            str = ctx->djc_file->constants[value].data.utf8_data;
        }
    }
    
    if (!str) {
        return -1;
    }
    
    len = (uint16_t)strlen(str);
    *result = len;
    return 0;
}

/**
 * String.toUpperCase()
 */
static int native_string_toUpperCase(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t value;
    const char* src_str;
    char conv_buf[256];
    uint16_t len;
    uint16_t i;
    uint16_t const_idx;
    
    (void)arg_count;
    
    value = args[0];
    src_str = NULL;
    
    /* Value is UTF8 constant index */
    if (value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[value].tag == CONST_UTF8) {
            src_str = ctx->djc_file->constants[value].data.utf8_data;
        }
    }
    
    if (!src_str) {
        printf("ERROR: Invalid string constant index for toUpperCase: %d\n", value);
        return -1;
    }
    
    len = (uint16_t)strlen(src_str);
    if (len >= sizeof(conv_buf)) {
        printf("ERROR: String too long for toUpperCase\n");
        return -1;
    }
    
    /* Convert to uppercase */
    for (i = 0; i < len; i++) {
        conv_buf[i] = (char)toupper((unsigned char)src_str[i]);
    }
    conv_buf[len] = '\0';
    
    /* Add constant to pool */
    const_idx = djc_add_string(ctx->djc_file, conv_buf);
    if (const_idx == 0) {
        printf("ERROR: Failed to add UTF8 constant for toUpperCase\n");
        return -1;
    }
    
    *result = const_idx;
    return 0;
}

/**
 * String.toLowerCase()
 */
static int native_string_toLowerCase(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t value;
    const char* src_str;
    char conv_buf[256];
    uint16_t len;
    uint16_t i;
    uint16_t const_idx;
    
    (void)arg_count;
    
    value = args[0];
    src_str = NULL;
    
    /* Value is UTF8 constant index */
    if (value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[value].tag == CONST_UTF8) {
            src_str = ctx->djc_file->constants[value].data.utf8_data;
        }
    }
    
    if (!src_str) {
        printf("ERROR: Invalid string constant index for toLowerCase: %d\n", value);
        return -1;
    }
    
    len = (uint16_t)strlen(src_str);
    if (len >= sizeof(conv_buf)) {
        printf("ERROR: String too long for toLowerCase\n");
        return -1;
    }
    
    /* Convert to lowercase */
    for (i = 0; i < len; i++) {
        conv_buf[i] = (char)tolower((unsigned char)src_str[i]);
    }
    conv_buf[len] = '\0';
    
    /* Add constant to pool */
    const_idx = djc_add_string(ctx->djc_file, conv_buf);
    if (const_idx == 0) {
        printf("ERROR: Failed to add UTF8 constant for toLowerCase\n");
        return -1;
    }
    
    *result = const_idx;
    return 0;
}

/* ===== Math Class Methods ===== */

/**
 * Math.abs(float) - Absolute value
 */
static int native_math_abs(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float value, result_value;
    uint32_t float_bits, result_bits;
    
    (void)ctx;
    
    if (arg_count < 2) {
        return -1;
    }
    
    /* Convert args to float */
    float_bits = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    memcpy(&value, &float_bits, sizeof(float));
    
    /* Calculate absolute value */
    result_value = (float)fabs((double)value);
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Math.min(float, float) - Minimum of two values
 */
static int native_math_min(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float a, b, result_value;
    uint32_t float_bits_a, float_bits_b, result_bits;
    
    (void)ctx;
    
    if (arg_count < 4) {
        return -1;
    }
    
    /* Convert args to floats */
    float_bits_a = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    float_bits_b = ((uint32_t)args[2] << 16) | (uint32_t)args[3];
    memcpy(&a, &float_bits_a, sizeof(float));
    memcpy(&b, &float_bits_b, sizeof(float));
    
    /* Calculate minimum */
    result_value = (a < b) ? a : b;
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Math.max(float, float) - Maximum of two values
 */
static int native_math_max(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float a, b, result_value;
    uint32_t float_bits_a, float_bits_b, result_bits;
    
    (void)ctx;
    
    if (arg_count < 4) {
        return -1;
    }
    
    /* Convert args to floats */
    float_bits_a = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    float_bits_b = ((uint32_t)args[2] << 16) | (uint32_t)args[3];
    memcpy(&a, &float_bits_a, sizeof(float));
    memcpy(&b, &float_bits_b, sizeof(float));
    
    /* Calculate maximum */
    result_value = (a > b) ? a : b;
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Math.sqrt(float) - Square root
 */
static int native_math_sqrt(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float value, result_value;
    uint32_t float_bits, result_bits;
    
    (void)ctx;
    
    if (arg_count < 2) {
        return -1;
    }
    
    /* Convert args to float */
    float_bits = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    memcpy(&value, &float_bits, sizeof(float));
    
    /* Calculate square root */
    result_value = (float)sqrt((double)value);
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Math.sin(float) - Sine (radians)
 */
static int native_math_sin(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float value, result_value;
    uint32_t float_bits, result_bits;
    
    (void)ctx;
    
    if (arg_count < 2) {
        return -1;
    }
    
    /* Convert args to float */
    float_bits = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    memcpy(&value, &float_bits, sizeof(float));
    
    /* Calculate sine */
    result_value = (float)sin((double)value);
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Math.floor(float) - Floor function (round down)
 */
static int native_math_floor(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float value, result_value;
    uint32_t float_bits, result_bits;
    
    (void)ctx;
    
    if (arg_count < 2) {
        return -1;
    }
    
    /* Convert args to float */
    float_bits = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    memcpy(&value, &float_bits, sizeof(float));
    
    /* Calculate floor */
    result_value = (float)floor((double)value);
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Math.ceil(float) - Ceiling function (round up)
 */
static int native_math_ceil(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float value, result_value;
    uint32_t float_bits, result_bits;
    
    (void)ctx;
    
    if (arg_count < 2) {
        return -1;
    }
    
    /* Convert args to float */
    float_bits = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    memcpy(&value, &float_bits, sizeof(float));
    
    /* Calculate ceiling */
    result_value = (float)ceil((double)value);
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Math.cos(float) - Cosine (radians)
 */
static int native_math_cos(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float value, result_value;
    uint32_t float_bits, result_bits;
    
    (void)ctx;
    
    if (arg_count < 2) {
        return -1;
    }
    
    /* Convert args to float */
    float_bits = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    memcpy(&value, &float_bits, sizeof(float));
    
    /* Calculate cosine */
    result_value = (float)cos((double)value);
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Math.tan(float) - Tangent (radians)
 */
static int native_math_tan(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float value, result_value;
    uint32_t float_bits, result_bits;
    
    (void)ctx;
    
    if (arg_count < 2) {
        return -1;
    }
    
    /* Convert args to float */
    float_bits = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    memcpy(&value, &float_bits, sizeof(float));
    
    /* Calculate tangent */
    result_value = (float)tan((double)value);
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Math.pow(float, float) - Power (base^exponent)
 */
static int native_math_pow(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float base, exponent, result_value;
    uint32_t float_bits_base, float_bits_exp, result_bits;
    
    (void)ctx;
    
    if (arg_count < 4) {
        return -1;
    }
    
    /* Convert args to floats */
    float_bits_base = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    float_bits_exp = ((uint32_t)args[2] << 16) | (uint32_t)args[3];
    memcpy(&base, &float_bits_base, sizeof(float));
    memcpy(&exponent, &float_bits_exp, sizeof(float));
    
    /* Calculate power */
    result_value = (float)pow((double)base, (double)exponent);
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Math.exp(float) - Exponential (e^x)
 */
static int native_math_exp(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float value, result_value;
    uint32_t float_bits, result_bits;
    
    (void)ctx;
    
    if (arg_count < 2) {
        return -1;
    }
    
    /* Convert args to float */
    float_bits = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    memcpy(&value, &float_bits, sizeof(float));
    
    /* Calculate exponential */
    result_value = (float)exp((double)value);
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Math.log(float) - Natural logarithm
 */
static int native_math_log(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    float value, result_value;
    uint32_t float_bits, result_bits;
    
    (void)ctx;
    
    if (arg_count < 2) {
        return -1;
    }
    
    /* Convert args to float */
    float_bits = ((uint32_t)args[0] << 16) | (uint32_t)args[1];
    memcpy(&value, &float_bits, sizeof(float));
    
    /* Calculate natural logarithm */
    result_value = (float)log((double)value);
    
    /* Convert result to 2 words */
    memcpy(&result_bits, &result_value, sizeof(float));
    result[0] = (uint16_t)(result_bits >> 16);
    result[1] = (uint16_t)(result_bits & 0xFFFF);
    
    return 0;
}

/**
 * Register all built-in native methods
 */
int native_register_builtins(void) {
    static NativeParamType param_int[] = { NATIVE_PARAM_INT };
    static NativeParamType param_string[] = { NATIVE_PARAM_STRING };
    
    /* System.out.println(int) */
    if (native_register(
        "java/lang/System",
        "println",
        "(I)V",
        native_system_println_int,
        1,
        param_int,
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* System.out.println(long) */
    if (native_register(
        "java/lang/System",
        "println",
        "(J)V",
        native_system_println_long,
        2,
        NULL,  /* No param types needed for now */
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* System.out.println(String) */
    if (native_register(
        "java/lang/System",
        "println",
        "(Ljava/lang/String;)V",
        native_system_println_string,
        1,
        param_string,
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* System.out.println(float) */
    if (native_register(
        "java/lang/System",
        "println",
        "(F)V",
        native_system_println_float,
        2,
        NULL,  /* No param types needed for now */
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* System.printInt(int) */
    if (native_register(
        "java/lang/System",
        "printInt",
        "(I)V",
        native_system_print_int,
        1,
        param_int,
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* System.out.print(int) - legacy alias */
    if (native_register(
        "java/lang/System",
        "print",
        "(I)V",
        native_system_print_int,
        1,
        param_int,
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* System.out.print(long) */
    if (native_register(
        "java/lang/System",
        "printLong",
        "(J)V",
        native_system_print_long,
        2,  /* Long takes 2 words */
        NULL,
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* System.out.print(String) */
    if (native_register(
        "java/lang/System",
        "print",
        "(Ljava/lang/String;)V",
        native_system_print_string,
        1,
        param_string,
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* String.length() */
    if (native_register(
        "java/lang/String",
        "length",
        "(Ljava/lang/String;)I",
        native_string_length,
        1,
        param_string,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    /* String.toUpperCase() */
    if (native_register(
        "java/lang/String",
        "toUpperCase",
        "(Ljava/lang/String;)Ljava/lang/String;",
        native_string_toUpperCase,
        1,
        param_string,
        NATIVE_RETURN_STRING
    ) != 0) {
        return -1;
    }
    
    /* String.toLowerCase() */
    if (native_register(
        "java/lang/String",
        "toLowerCase",
        "(Ljava/lang/String;)Ljava/lang/String;",
        native_string_toLowerCase,
        1,
        param_string,
        NATIVE_RETURN_STRING
    ) != 0) {
        return -1;
    }
    
    /* Math.abs(float) */
    if (native_register(
        "Math",
        "abs",
        "(F)F",
        native_math_abs,
        2,  /* Float takes 2 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    /* Math.min(float, float) */
    if (native_register(
        "Math",
        "min",
        "(FF)F",
        native_math_min,
        4,  /* Two floats take 4 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    /* Math.max(float, float) */
    if (native_register(
        "Math",
        "max",
        "(FF)F",
        native_math_max,
        4,  /* Two floats take 4 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    /* Math.sqrt(float) */
    if (native_register(
        "Math",
        "sqrt",
        "(F)F",
        native_math_sqrt,
        2,  /* Float takes 2 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    /* Math.sin(float) */
    if (native_register(
        "Math",
        "sin",
        "(F)F",
        native_math_sin,
        2,  /* Float takes 2 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    /* Math.cos(float) */
    if (native_register(
        "Math",
        "cos",
        "(F)F",
        native_math_cos,
        2,  /* Float takes 2 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    /* Math.tan(float) */
    if (native_register(
        "Math",
        "tan",
        "(F)F",
        native_math_tan,
        2,  /* Float takes 2 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    /* Math.pow(float, float) */
    if (native_register(
        "Math",
        "pow",
        "(FF)F",
        native_math_pow,
        4,  /* Two floats take 4 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    /* Math.exp(float) */
    if (native_register(
        "Math",
        "exp",
        "(F)F",
        native_math_exp,
        2,  /* Float takes 2 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    /* Math.log(float) */
    if (native_register(
        "Math",
        "log",
        "(F)F",
        native_math_log,
        2,  /* Float takes 2 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    /* Math.floor(float) */
    if (native_register(
        "Math",
        "floor",
        "(F)F",
        native_math_floor,
        2,  /* Float takes 2 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    /* Math.ceil(float) */
    if (native_register(
        "Math",
        "ceil",
        "(F)F",
        native_math_ceil,
        2,  /* Float takes 2 words */
        NULL,
        NATIVE_RETURN_FLOAT
    ) != 0) {
        return -1;
    }
    
    return 0;
}
