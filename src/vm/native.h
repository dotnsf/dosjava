#ifndef NATIVE_H
#define NATIVE_H

#include "../types.h"
#include "interpreter.h"

/**
 * Native Method Mechanism
 * 
 * Provides a registration system for native methods that can be called
 * from Java bytecode. Native methods are implemented in C and registered
 * with the VM at initialization time.
 */

/* Maximum number of native methods that can be registered */
#define MAX_NATIVE_METHODS 64

/* Native method signature types */
typedef enum {
    NATIVE_PARAM_VOID = 0,      /* No parameter */
    NATIVE_PARAM_INT = 1,       /* int (16-bit) */
    NATIVE_PARAM_STRING = 2,    /* String (UTF8 constant index) */
    NATIVE_PARAM_OBJECT = 3     /* Object reference */
} NativeParamType;

typedef enum {
    NATIVE_RETURN_VOID = 0,     /* No return value */
    NATIVE_RETURN_INT = 1,      /* int (16-bit) */
    NATIVE_RETURN_STRING = 2,   /* String (UTF8 constant index) */
    NATIVE_RETURN_OBJECT = 3    /* Object reference */
} NativeReturnType;

/**
 * Native method function pointer
 * 
 * @param ctx Execution context
 * @param args Array of arguments (popped from stack)
 * @param arg_count Number of arguments
 * @param result Pointer to store return value (if any)
 * @return 0 on success, -1 on error
 */
typedef int (*NativeMethodFunc)(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result);

/**
 * Native method descriptor
 */
typedef struct {
    const char* class_name;         /* Class name (e.g., "java/lang/System") */
    const char* method_name;        /* Method name (e.g., "println") */
    const char* descriptor;         /* Method descriptor (e.g., "(I)V") */
    NativeMethodFunc func;          /* Function pointer */
    uint8_t param_count;            /* Number of parameters */
    NativeParamType* param_types;   /* Parameter types array */
    NativeReturnType return_type;   /* Return type */
} NativeMethodDescriptor;

/**
 * Native method registry
 */
typedef struct {
    NativeMethodDescriptor methods[MAX_NATIVE_METHODS];
    uint8_t method_count;
} NativeMethodRegistry;

/**
 * Initialize native method registry
 * @return 0 on success, -1 on error
 */
int native_init(void);

/**
 * Shutdown native method registry
 */
void native_shutdown(void);

/**
 * Register a native method
 * 
 * @param class_name Class name (e.g., "java/lang/System")
 * @param method_name Method name (e.g., "println")
 * @param descriptor Method descriptor (e.g., "(I)V")
 * @param func Function pointer
 * @param param_count Number of parameters
 * @param param_types Parameter types array
 * @param return_type Return type
 * @return 0 on success, -1 on error
 */
int native_register(
    const char* class_name,
    const char* method_name,
    const char* descriptor,
    NativeMethodFunc func,
    uint8_t param_count,
    NativeParamType* param_types,
    NativeReturnType return_type
);

/**
 * Find a native method by name and descriptor
 * 
 * @param class_name Class name
 * @param method_name Method name
 * @param descriptor Method descriptor
 * @return Pointer to native method descriptor, or NULL if not found
 */
NativeMethodDescriptor* native_find(
    const char* class_name,
    const char* method_name,
    const char* descriptor
);

/**
 * Invoke a native method
 * 
 * @param ctx Execution context
 * @param method Native method descriptor
 * @param args Array of arguments (from stack)
 * @param arg_count Number of arguments
 * @param result Pointer to store return value (if any)
 * @return 0 on success, -1 on error
 */
int native_invoke(
    ExecutionContext* ctx,
    NativeMethodDescriptor* method,
    uint16_t* args,
    uint8_t arg_count,
    uint16_t* result
);

/**
 * Register all built-in native methods
 * Called during VM initialization
 * @return 0 on success, -1 on error
 */
int native_register_builtins(void);

/**
 * Register socket-related native methods
 * Called by native_register_builtins()
 * @return 0 on success, -1 on error
 */
int native_register_socket_methods(void);

#endif /* NATIVE_H */

// Made with Bob
