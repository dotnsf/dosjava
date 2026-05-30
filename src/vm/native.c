#include "native.h"
#include "interpreter.h"
#include "http.h"
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
 * String.charAt(int index)
 * Returns the character at the specified index as a String
 */
int native_string_charAt(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t str_value;
    uint16_t index;
    const char* str;
    uint16_t len;
    char char_str[2];
    uint16_t char_index;
    
    if (arg_count != 2) {
        printf("ERROR: charAt expects 2 arguments, got %u\n", arg_count);
        return -1;
    }
    
    str_value = args[0];
    index = args[1];
    str = NULL;
    
    /* Get string from constant pool */
    if (str_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
            str = ctx->djc_file->constants[str_value].data.utf8_data;
        }
    }
    
    if (!str) {
        printf("ERROR: Invalid string constant index for charAt: %d\n", str_value);
        return -1;
    }
    
    len = (uint16_t)strlen(str);
    
    /* Check bounds */
    if (index >= len) {
        char error_msg[64];
        int throw_result;
        sprintf(error_msg, "String index out of range: %u", index);
        throw_result = throw_runtime_exception(ctx, EXCEPTION_TYPE_STRING_INDEX_OUT_OF_BOUNDS, error_msg);
        return throw_result;
    }
    
    /* Create single-character string */
    char_str[0] = str[index];
    char_str[1] = '\0';
    
    /* Add to constant pool and return index */
    char_index = djc_add_string(ctx->djc_file, char_str);
    if (char_index == 0xFFFF) {
        printf("ERROR: Failed to add character string to constant pool\n");
        return -1;
    }
    
    *result = char_index;
    return 0;
}

/**
 * String.isEmpty()
 * Returns true if string length is 0
 */
int native_string_isEmpty(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t str_value;
    const char* str;
    uint16_t len;
    
    if (arg_count != 1) {
        printf("ERROR: isEmpty expects 1 argument, got %u\n", arg_count);
        return -1;
    }
    
    str_value = args[0];
    str = NULL;
    
    /* Get string from constant pool */
    if (str_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
            str = ctx->djc_file->constants[str_value].data.utf8_data;
        }
    }
    
    if (!str) {
        printf("ERROR: Invalid string constant index for isEmpty: %d\n", str_value);
        return -1;
    }
    
    len = (uint16_t)strlen(str);
    
    /* Return 1 (true) if empty, 0 (false) otherwise */
    *result = (len == 0) ? 1 : 0;
    return 0;
}

/**
 * String.trim()
 * Returns a string with leading and trailing whitespace removed
 */
int native_string_trim(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t str_value;
    const char* src_str;
    char trim_buf[256];
    uint16_t len;
    uint16_t start, end;
    uint16_t new_len;
    uint16_t i;
    uint16_t const_idx;
    
    if (arg_count != 1) {
        printf("ERROR: trim expects 1 argument, got %u\n", arg_count);
        return -1;
    }
    
    str_value = args[0];
    src_str = NULL;
    
    /* Get string from constant pool */
    if (str_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
            src_str = ctx->djc_file->constants[str_value].data.utf8_data;
        }
    }
    
    if (!src_str) {
        printf("ERROR: Invalid string constant index for trim: %d\n", str_value);
        return -1;
    }
    
    len = (uint16_t)strlen(src_str);
    
    /* Find first non-whitespace character */
    start = 0;
    while (start < len && (src_str[start] == ' ' || src_str[start] == '\t' ||
                           src_str[start] == '\n' || src_str[start] == '\r')) {
        start++;
    }
    
    /* If all whitespace, return empty string */
    if (start >= len) {
        const_idx = djc_add_string(ctx->djc_file, "");
        if (const_idx == 0) {
            printf("ERROR: Failed to add UTF8 constant for trim\n");
            return -1;
        }
        *result = const_idx;
        return 0;
    }
    
    /* Find last non-whitespace character */
    end = len - 1;
    while (end > start && (src_str[end] == ' ' || src_str[end] == '\t' ||
                           src_str[end] == '\n' || src_str[end] == '\r')) {
        end--;
    }
    
    /* Calculate new length */
    new_len = end - start + 1;
    
    if (new_len >= sizeof(trim_buf)) {
        printf("ERROR: Trimmed string too long\n");
        return -1;
    }
    
    /* Copy trimmed content */
    for (i = 0; i < new_len; i++) {
        trim_buf[i] = src_str[start + i];
    }
    trim_buf[new_len] = '\0';
    
    /* Add constant to pool */
    const_idx = djc_add_string(ctx->djc_file, trim_buf);
    if (const_idx == 0) {
        printf("ERROR: Failed to add UTF8 constant for trim\n");
        return -1;
    }
    
    *result = const_idx;
    return 0;
}

/**
 * String.replace(String oldStr, String newStr)
 * Returns a string with all occurrences of oldStr replaced by newStr
 */
int native_string_replace(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t str_value;
    uint16_t old_value;
    uint16_t new_value;
    const char* src_str;
    const char* old_str;
    const char* new_str;
    char replace_buf[256];
    uint16_t src_len, old_len, new_len;
    uint16_t result_len;
    uint16_t i, j;
    uint8_t match;
    uint16_t const_idx;
    
    if (arg_count != 3) {
        printf("ERROR: replace expects 3 arguments, got %u\n", arg_count);
        return -1;
    }
    
    str_value = args[0];
    old_value = args[1];
    new_value = args[2];
    src_str = NULL;
    old_str = NULL;
    new_str = NULL;
    
    /* Get strings from constant pool */
    if (str_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
            src_str = ctx->djc_file->constants[str_value].data.utf8_data;
        }
    }
    
    if (old_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[old_value].tag == CONST_UTF8) {
            old_str = ctx->djc_file->constants[old_value].data.utf8_data;
        }
    }
    
    if (new_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[new_value].tag == CONST_UTF8) {
            new_str = ctx->djc_file->constants[new_value].data.utf8_data;
        }
    }
    
    if (!src_str || !old_str || !new_str) {
        printf("ERROR: Invalid string constant for replace\n");
        return -1;
    }
    
    src_len = (uint16_t)strlen(src_str);
    old_len = (uint16_t)strlen(old_str);
    new_len = (uint16_t)strlen(new_str);
    
    /* Empty old string - return original */
    if (old_len == 0) {
        *result = str_value;
        return 0;
    }
    
    result_len = 0;
    i = 0;
    
    while (i < src_len) {
        /* Check if we have a match at current position */
        match = 1;
        if (i + old_len <= src_len) {
            for (j = 0; j < old_len; j++) {
                if (src_str[i + j] != old_str[j]) {
                    match = 0;
                    break;
                }
            }
        } else {
            match = 0;
        }
        
        if (match) {
            /* Replace: copy new string */
            if (result_len + new_len >= sizeof(replace_buf)) {
                printf("ERROR: Result string too long for replace\n");
                return -1;
            }
            for (j = 0; j < new_len; j++) {
                replace_buf[result_len++] = new_str[j];
            }
            i += old_len;  /* Skip old string */
        } else {
            /* No match: copy original character */
            if (result_len >= sizeof(replace_buf)) {
                printf("ERROR: Result string too long for replace\n");
                return -1;
            }
            replace_buf[result_len++] = src_str[i];
            i++;
        }
    }
    
    replace_buf[result_len] = '\0';
    
    /* Add constant to pool */
    const_idx = djc_add_string(ctx->djc_file, replace_buf);
    if (const_idx == 0) {
        printf("ERROR: Failed to add UTF8 constant for replace\n");
        return -1;
    }
    
    *result = const_idx;
    return 0;
}

/**
 * String.compareTo(String anotherString)
 * Compares two strings lexicographically
 */
int native_string_compareTo(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t str1_value;
    uint16_t str2_value;
    const char* str1;
    const char* str2;
    int cmp_result;
    int throw_result;
    
    if (arg_count != 2) {
        printf("ERROR: compareTo expects 2 arguments, got %u\n", arg_count);
        return -1;
    }
    
    str1_value = args[0];
    str2_value = args[1];
    str1 = NULL;
    str2 = NULL;
    
    /* Get first string from constant pool */
    if (str1_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[str1_value].tag == CONST_UTF8) {
            str1 = ctx->djc_file->constants[str1_value].data.utf8_data;
        }
    }
    
    /* Get second string from constant pool */
    if (str2_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[str2_value].tag == CONST_UTF8) {
            str2 = ctx->djc_file->constants[str2_value].data.utf8_data;
        }
    }
    
    if (!str1 || !str2) {
        /* Throw NullPointerException if either string is null */
        throw_result = throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER, "Cannot compare null string");
        return throw_result;
    }
    
    /* Compare strings lexicographically */
    cmp_result = strcmp(str1, str2);
    
    /* Return comparison result as int16_t */
    *result = (uint16_t)(int16_t)cmp_result;
    return 0;
}

/**
 * String.lastIndexOf(String str)
 * Returns the index of the last occurrence of the specified substring
 */
int native_string_lastIndexOf(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t str_value;
    uint16_t search_value;
    const char* str;
    const char* search_str;
    uint16_t str_len;
    uint16_t search_len;
    int16_t i;
    uint16_t j;
    int match;
    
    if (arg_count != 2) {
        printf("ERROR: lastIndexOf expects 2 arguments, got %u\n", arg_count);
        return -1;
    }
    
    str_value = args[0];
    search_value = args[1];
    str = NULL;
    search_str = NULL;
    
    /* Get strings from constant pool */
    if (str_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
            str = ctx->djc_file->constants[str_value].data.utf8_data;
        }
    }
    
    if (search_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[search_value].tag == CONST_UTF8) {
            search_str = ctx->djc_file->constants[search_value].data.utf8_data;
        }
    }
    
    if (!str || !search_str) {
        printf("ERROR: Invalid string constant for lastIndexOf\n");
        return -1;
    }
    
    str_len = (uint16_t)strlen(str);
    search_len = (uint16_t)strlen(search_str);
    
    /* Empty search string returns string length */
    if (search_len == 0) {
        *result = str_len;
        return 0;
    }
    
    /* Search string longer than source string */
    if (search_len > str_len) {
        *result = (uint16_t)-1;
        return 0;
    }
    
    /* Search from end to beginning */
    for (i = (int16_t)(str_len - search_len); i >= 0; i--) {
        match = 1;
        for (j = 0; j < search_len; j++) {
            if (str[i + j] != search_str[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            *result = (uint16_t)i;
            return 0;
        }
    }
    
    /* Not found */
    *result = (uint16_t)-1;
    return 0;
}

/**
 * String.contains(String str)
 * Returns true if the string contains the specified substring
 */
int native_string_contains(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t str_value;
    uint16_t search_value;
    const char* str;
    const char* search_str;
    
    if (arg_count != 2) {
        printf("ERROR: contains expects 2 arguments, got %u\n", arg_count);
        return -1;
    }
    
    str_value = args[0];
    search_value = args[1];
    str = NULL;
    search_str = NULL;
    
    /* Get strings from constant pool */
    if (str_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
            str = ctx->djc_file->constants[str_value].data.utf8_data;
        }
    }
    
    if (search_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[search_value].tag == CONST_UTF8) {
            search_str = ctx->djc_file->constants[search_value].data.utf8_data;
        }
    }
    
    if (!str || !search_str) {
        printf("ERROR: Invalid string constant for contains\n");
        return -1;
    }
    
    /* Empty search string is always contained */
    if (strlen(search_str) == 0) {
        *result = 1;
        return 0;
    }
    
    /* Use strstr to check if substring exists */
    *result = (strstr(str, search_str) != NULL) ? 1 : 0;
    return 0;
}

/**
 * String.repeat(int count)
 * Returns a string repeated count times
 */
int native_string_repeat(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t str_value;
    uint16_t count;
    const char* src_str;
    char repeat_buf[256];
    uint16_t str_len;
    uint16_t total_len;
    uint16_t i, j;
    uint16_t pos;
    uint16_t const_idx;
    
    if (arg_count != 2) {
        printf("ERROR: repeat expects 2 arguments, got %u\n", arg_count);
        return -1;
    }
    
    str_value = args[0];
    count = args[1];
    src_str = NULL;
    
    /* Get string from constant pool */
    if (str_value < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[str_value].tag == CONST_UTF8) {
            src_str = ctx->djc_file->constants[str_value].data.utf8_data;
        }
    }
    
    if (!src_str) {
        printf("ERROR: Invalid string constant index for repeat: %d\n", str_value);
        return -1;
    }
    
    /* Check for negative count (treated as signed) */
    if ((int16_t)count < 0) {
        char error_msg[64];
        int throw_result;
        sprintf(error_msg, "Negative repeat count: %d", (int16_t)count);
        throw_result = throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT, error_msg);
        return throw_result;
    }
    
    str_len = (uint16_t)strlen(src_str);
    
    /* count = 0 returns empty string */
    if (count == 0) {
        const_idx = djc_add_string(ctx->djc_file, "");
        if (const_idx == 0) {
            printf("ERROR: Failed to add UTF8 constant for repeat\n");
            return -1;
        }
        *result = const_idx;
        return 0;
    }
    
    /* Calculate total length */
    total_len = str_len * count;
    
    /* Check for overflow or buffer size */
    if (total_len / count != str_len || total_len >= sizeof(repeat_buf)) {
        printf("ERROR: Repeated string too long\n");
        return -1;
    }
    
    /* Copy string count times */
    pos = 0;
    for (i = 0; i < count; i++) {
        for (j = 0; j < str_len; j++) {
            repeat_buf[pos++] = src_str[j];
        }
    }
    repeat_buf[total_len] = '\0';
    
    /* Add constant to pool */
    const_idx = djc_add_string(ctx->djc_file, repeat_buf);
    if (const_idx == 0) {
        printf("ERROR: Failed to add UTF8 constant for repeat\n");
        return -1;
    }
    
    *result = const_idx;
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
 * Integer.parseInt(String) - Parse string to integer
 * Throws NumberFormatException if string is not a valid integer
 */
static int native_integer_parseInt(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t string_index;
    const char* str;
    int value;
    int sign;
    int i;
    char c;
    int has_digits;
    char msg[64];
    
    (void)arg_count;
    
    string_index = args[0];
    str = NULL;
    
    /* Get string from constant pool */
    if (string_index < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[string_index].tag == CONST_UTF8) {
            str = ctx->djc_file->constants[string_index].data.utf8_data;
        }
    }
    
    if (str == NULL || str[0] == '\0') {
        sprintf(msg, "Cannot parse empty string as integer");
        if (throw_runtime_exception(ctx, EXCEPTION_TYPE_NUMBER_FORMAT, msg) != 0) {
            return -1;
        }
        *result = 0;
        return 0;
    }
    
    /* Parse integer */
    value = 0;
    sign = 1;
    i = 0;
    has_digits = 0;
    
    /* Check for sign */
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }
    
    /* Parse digits */
    while (str[i] != '\0') {
        c = str[i];
        if (c >= '0' && c <= '9') {
            value = value * 10 + (c - '0');
            has_digits = 1;
        } else {
            /* Invalid character */
            sprintf(msg, "Cannot parse '%s' as integer", str);
            if (throw_runtime_exception(ctx, EXCEPTION_TYPE_NUMBER_FORMAT, msg) != 0) {
                return -1;
            }
            *result = 0;
            return 0;
        }
        i++;
    }
    
    if (!has_digits) {
        sprintf(msg, "Cannot parse '%s' as integer", str);
        if (throw_runtime_exception(ctx, EXCEPTION_TYPE_NUMBER_FORMAT, msg) != 0) {
            return -1;
        }
        *result = 0;
        return 0;
    }
    
    *result = (uint16_t)(sign * value);
    return 0;
}

/**
 * Exception.getType() - Get exception type code
 * Returns integer type code (EXCEPTION_TYPE_*)
 */
static int native_exception_getType(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    (void)args;
    (void)arg_count;
    
    /* Return the exception type from context */
    *result = (uint16_t)ctx->exception_type;
    return 0;
}

/**
 * Exception.getMessage() - Get exception message
 * Returns string constant pool index
 *
 * For dynamically generated messages (e.g., with line numbers),
 * this will create a temporary string in the constant pool.
 */
static int native_exception_getMessage(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t i;
    
    (void)args;
    (void)arg_count;
    
    /* Try to find message in constant pool */
    for (i = 0; i < ctx->djc_file->header.constant_pool_count; i++) {
        if (ctx->djc_file->constants[i].tag == CONST_UTF8) {
            if (strcmp(ctx->djc_file->constants[i].data.utf8_data, ctx->exception_message) == 0) {
                *result = i;
                return 0;
            }
        }
    }
    
    /* Message not in constant pool - add it dynamically if there's space */
    if (ctx->djc_file->header.constant_pool_count < 256) {
        i = ctx->djc_file->header.constant_pool_count;
        ctx->djc_file->constants[i].tag = CONST_UTF8;
        ctx->djc_file->constants[i].data.utf8_data = ctx->exception_message;
        ctx->djc_file->header.constant_pool_count++;
        *result = i;
        return 0;
    }
    
    /* Constant pool full - return index 0 (empty string) */
    *result = 0;
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
    
    /* String.charAt(int) */
    if (native_register(
        "java/lang/String",
        "charAt",
        "(Ljava/lang/String;I)Ljava/lang/String;",
        native_string_charAt,
        2,  /* String + int */
        param_string,
        NATIVE_RETURN_STRING
    ) != 0) {
        return -1;
    }
    
    /* String.isEmpty() */
    if (native_register(
        "java/lang/String",
        "isEmpty",
        "(Ljava/lang/String;)I",
        native_string_isEmpty,
        1,
        param_string,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    /* String.trim() */
    if (native_register(
        "java/lang/String",
        "trim",
        "(Ljava/lang/String;)Ljava/lang/String;",
        native_string_trim,
        1,
        param_string,
        NATIVE_RETURN_STRING
    ) != 0) {
        return -1;
    }
    
    /* String.replace(String, String) */
    if (native_register(
        "java/lang/String",
        "replace",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
        native_string_replace,
        3,  /* String + String + String */
        param_string,
        NATIVE_RETURN_STRING
    ) != 0) {
        return -1;
    }
    
    /* String.compareTo(String) */
    if (native_register(
        "java/lang/String",
        "compareTo",
        "(Ljava/lang/String;Ljava/lang/String;)I",
        native_string_compareTo,
        2,  /* Two strings */
        param_string,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    /* String.lastIndexOf(String) */
    if (native_register(
        "java/lang/String",
        "lastIndexOf",
        "(Ljava/lang/String;Ljava/lang/String;)I",
        native_string_lastIndexOf,
        2,  /* Two strings */
        param_string,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    /* String.contains(String) */
    if (native_register(
        "java/lang/String",
        "contains",
        "(Ljava/lang/String;Ljava/lang/String;)I",
        native_string_contains,
        2,  /* Two strings */
        param_string,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    /* String.repeat(int) */
    if (native_register(
        "java/lang/String",
        "repeat",
        "(Ljava/lang/String;I)Ljava/lang/String;",
        native_string_repeat,
        2,  /* String + int */
        param_string,
        NATIVE_RETURN_STRING
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
    
    /* Integer.parseInt(String) */
    if (native_register(
        NULL,  /* No class name - match by method name and descriptor only */
        "parseInt",
        "(Ljava/lang/String;)I",
        native_integer_parseInt,
        1,
        param_string,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    /* Exception.getType() */
    if (native_register(
        "Exception",
        "getType",
        "()I",
        native_exception_getType,
        0,
        NULL,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    /* Exception.getMessage() */
    if (native_register(
        "Exception",
        "getMessage",
        "()Ljava/lang/String;",
        native_exception_getMessage,
        0,
        NULL,
        NATIVE_RETURN_STRING
    ) != 0) {
        return -1;
    }
    
    /* Http.get(String) */
    if (native_register(
        NULL,  /* No class name - match by method name and descriptor only */
        "get",
        "(Ljava/lang/String;)Ljava/lang/String;",
        native_http_get,
        1,
        param_string,
        NATIVE_RETURN_STRING
    ) != 0) {
        return -1;
    }
    
    /* Http.get(String, String) - with headers */
    {
        NativeParamType params[2];
        params[0] = NATIVE_PARAM_STRING;
        params[1] = NATIVE_PARAM_STRING;
        
        if (native_register(
            NULL,  /* No class name - match by method name and descriptor only */
            "get",
            "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
            native_http_get_with_headers,
            2,
            params,
            NATIVE_RETURN_STRING
        ) != 0) {
            return -1;
        }
    }
    
    /* Http.getStatusCode(String) */
    if (native_register(
        NULL,  /* No class name - match by method name and descriptor only */
        "getStatusCode",
        "(Ljava/lang/String;)I",
        native_http_getStatusCode,
        1,
        param_string,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    /* Http.post(String, String) */
    {
        NativeParamType params[2];
        params[0] = NATIVE_PARAM_STRING;
        params[1] = NATIVE_PARAM_STRING;
        
        if (native_register(
            NULL,  /* No class name - match by method name and descriptor only */
            "post",
            "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
            native_http_post,
            2,
            params,
            NATIVE_RETURN_STRING
        ) != 0) {
            return -1;
        }
    }
    
    /* Http.put(String, String) */
    {
        NativeParamType params[2];
        params[0] = NATIVE_PARAM_STRING;
        params[1] = NATIVE_PARAM_STRING;
        
        if (native_register(
            NULL,  /* No class name - match by method name and descriptor only */
            "put",
            "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
            native_http_put,
            2,
            params,
            NATIVE_RETURN_STRING
        ) != 0) {
            return -1;
        }
    }
    
    /* Http.delete(String) */
    if (native_register(
        NULL,  /* No class name - match by method name and descriptor only */
        "delete",
        "(Ljava/lang/String;)Ljava/lang/String;",
        native_http_delete,
        1,
        param_string,
        NATIVE_RETURN_STRING
    ) != 0) {
        return -1;
    }
    
    return 0;
}
