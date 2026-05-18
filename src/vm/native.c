#include "native.h"
#include "../runtime/system.h"
#ifdef ENABLE_SOCKETS
#include "../runtime/socket.h"
#endif
#include "../runtime/string.h"
#include "../format/djc.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
        printf("ERROR: Native method %s expects %u args, got %u\n",
               method->method_name, method->param_count, arg_count);
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
        printf("ERROR: Invalid string constant index: %d\n", value);
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
        printf("ERROR: Invalid string constant index: %d\n", value);
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
        printf("ERROR: Invalid string constant index for length: %d\n", value);
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
    
    /* System.out.print(int) */
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
    
#ifdef ENABLE_SOCKETS
    /* Register socket native methods */
    if (native_register_socket_methods() != 0) {
        return -1;
    }
#endif
    
    return 0;
}

// Made with Bob


#ifdef ENABLE_SOCKETS
/* ===== Socket Native Methods ===== */

/**
 * Socket.init() - Initialize socket subsystem
 * Called automatically on first socket operation
 */
static int native_socket_init(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    (void)ctx;
    (void)args;
    (void)arg_count;
    (void)result;
    
    if (!socket_subsystem_is_initialized()) {
        if (socket_subsystem_init() != 0) {
            printf("ERROR: Failed to initialize socket subsystem\n");
            return -1;
        }
    }
    
    return 0;
}

/**
 * Socket.new(String host, int port) - Create and connect socket
 * Returns socket handle (object reference)
 */
static int native_socket_new(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t host_idx;
    uint16_t port;
    const char* host_str;
    String* host;
    Socket* sock;
    
    (void)arg_count;
    
    /* Initialize socket subsystem if needed */
    if (!socket_subsystem_is_initialized()) {
        if (socket_subsystem_init() != 0) {
            printf("ERROR: Failed to initialize socket subsystem\n");
            return -1;
        }
    }
    
    /* Get arguments: host (String constant index), port (int) */
    host_idx = args[0];
    port = args[1];
    
    /* Get host string from constant pool */
    host_str = NULL;
    if (host_idx < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[host_idx].tag == CONST_UTF8) {
            host_str = ctx->djc_file->constants[host_idx].data.utf8_data;
        }
    }
    
    if (!host_str) {
        printf("ERROR: Invalid host string constant index: %d\n", host_idx);
        return -1;
    }
    
    /* Create String object for host */
    host = string_new(host_str);
    if (!host) {
        printf("ERROR: Failed to create host string\n");
        return -1;
    }
    
    /* Create and connect socket */
    sock = socket_runtime_new_connected(host, port);
    if (!sock) {
        string_delete(host);
        printf("ERROR: Failed to create socket\n");
        return -1;
    }
    
    /* Return socket handle as object reference */
    *result = (uint16_t)sock;
    return 0;
}

/**
 * Socket.send(Socket sock, String data) - Send data through socket
 * Returns number of bytes sent
 */
static int native_socket_send(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t sock_ref;
    uint16_t data_idx;
    Socket* sock;
    const char* data_str;
    int sent;
    
    (void)arg_count;
    
    /* Get arguments: socket reference, data (String constant index) */
    sock_ref = args[0];
    data_idx = args[1];
    
    /* Get socket object */
    sock = (Socket*)sock_ref;
    if (!sock) {
        printf("ERROR: Invalid socket reference\n");
        return -1;
    }
    
    /* Get data string from constant pool */
    data_str = NULL;
    if (data_idx < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[data_idx].tag == CONST_UTF8) {
            data_str = ctx->djc_file->constants[data_idx].data.utf8_data;
        }
    }
    
    if (!data_str) {
        printf("ERROR: Invalid data string constant index: %d\n", data_idx);
        return -1;
    }
    
    /* Send data */
    sent = socket_runtime_send(sock, (const uint8_t*)data_str, (uint16_t)strlen(data_str));
    if (sent < 0) {
        printf("ERROR: Failed to send data\n");
        return -1;
    }
    
    *result = (uint16_t)sent;
    return 0;
}

/**
 * Socket.recv(Socket sock, int max_length) - Receive data from socket
 * Returns received data as String
 */
static int native_socket_recv(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t sock_ref;
    uint16_t max_length;
    Socket* sock;
    uint8_t buffer[256];
    int received;
    uint16_t const_idx;
    
    (void)arg_count;
    
    /* Get arguments: socket reference, max_length */
    sock_ref = args[0];
    max_length = args[1];
    
    /* Get socket object */
    sock = (Socket*)sock_ref;
    if (!sock) {
        printf("ERROR: Invalid socket reference\n");
        return -1;
    }
    
    /* Limit max_length to buffer size */
    if (max_length > sizeof(buffer) - 1) {
        max_length = sizeof(buffer) - 1;
    }
    
    /* Receive data */
    received = socket_runtime_recv(sock, buffer, max_length);
    if (received < 0) {
        printf("ERROR: Failed to receive data\n");
        return -1;
    }
    
    /* Null-terminate received data */
    buffer[received] = '\0';
    
    /* Add received data to constant pool */
    const_idx = djc_add_string(ctx->djc_file, (const char*)buffer);
    if (const_idx == 0) {
        printf("ERROR: Failed to add received data to constant pool\n");
        return -1;
    }
    
    *result = const_idx;
    return 0;
}

/**
 * Socket.close(Socket sock) - Close socket
 */
static int native_socket_close(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t sock_ref;
    Socket* sock;
    
    (void)ctx;
    (void)arg_count;
    (void)result;
    
    /* Get argument: socket reference */
    sock_ref = args[0];
    
    /* Get socket object */
    sock = (Socket*)sock_ref;
    if (!sock) {
        printf("ERROR: Invalid socket reference\n");
        return -1;
    }
    
    /* Close socket */
    if (socket_runtime_close(sock) != 0) {
        printf("ERROR: Failed to close socket\n");
        return -1;
    }
    
    return 0;
}

/**
 * Socket.isConnected(Socket sock) - Check if socket is connected
 * Returns 1 if connected, 0 otherwise
 */
static int native_socket_is_connected(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t sock_ref;
    Socket* sock;
    
    (void)ctx;
    (void)arg_count;
    
    /* Get argument: socket reference */
    sock_ref = args[0];
    
    /* Get socket object */
    sock = (Socket*)sock_ref;
    if (!sock) {
        printf("ERROR: Invalid socket reference\n");
        return -1;
    }
    
    *result = socket_runtime_is_connected(sock);
    return 0;
}

/**
 * Register socket native methods
 * Called from native_register_builtins()
 */
int native_register_socket_methods(void) {
    static NativeParamType param_void[] = { NATIVE_PARAM_VOID };
    static NativeParamType param_string_int[] = { NATIVE_PARAM_STRING, NATIVE_PARAM_INT };
    static NativeParamType param_object_string[] = { NATIVE_PARAM_OBJECT, NATIVE_PARAM_STRING };
    static NativeParamType param_object_int[] = { NATIVE_PARAM_OBJECT, NATIVE_PARAM_INT };
    static NativeParamType param_object[] = { NATIVE_PARAM_OBJECT };
    
    /* Socket.init() */
    if (native_register(
        "java/net/Socket",
        "init",
        "()V",
        native_socket_init,
        0,
        param_void,
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* Socket.new(String, int) - Constructor */
    if (native_register(
        "java/net/Socket",
        "<init>",
        "(Ljava/lang/String;I)Ljava/net/Socket;",
        native_socket_new,
        2,
        param_string_int,
        NATIVE_RETURN_OBJECT
    ) != 0) {
        return -1;
    }
    
    /* Socket.send(Socket, String) */
    if (native_register(
        "java/net/Socket",
        "send",
        "(Ljava/net/Socket;Ljava/lang/String;)I",
        native_socket_send,
        2,
        param_object_string,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    /* Socket.recv(Socket, int) */
    if (native_register(
        "java/net/Socket",
        "recv",
        "(Ljava/net/Socket;I)Ljava/lang/String;",
        native_socket_recv,
        2,
        param_object_int,
        NATIVE_RETURN_STRING
    ) != 0) {
        return -1;
    }
    
    /* Socket.close(Socket) */
    if (native_register(
        "java/net/Socket",
        "close",
        "(Ljava/net/Socket;)V",
        native_socket_close,
        1,
        param_object,
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* Socket.isConnected(Socket) */
    if (native_register(
        "java/net/Socket",
        "isConnected",
        "(Ljava/net/Socket;)I",
        native_socket_is_connected,
        1,
        param_object,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    return 0;
}
#endif /* ENABLE_SOCKETS */
