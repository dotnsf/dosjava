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
            return -1;
        }
    }
    
    return 0;
}

/**
 * Socket.create(String host, int port) - Create and connect socket
 * Returns socket handle (object reference)
 */

/* Global socket table to map int handles to Socket objects */
#define MAX_SOCKETS 4
static Socket* g_socket_table[MAX_SOCKETS] = { NULL, NULL, NULL, NULL };

/* Find free socket slot and return handle (0-3), or -1 if full */
static int socket_table_add(Socket* sock) {
    int i;
    for (i = 0; i < MAX_SOCKETS; i++) {
        if (g_socket_table[i] == NULL) {
            g_socket_table[i] = sock;
            return i;
        }
    }
    return -1;  /* Table full */
}

/* Get Socket object from handle */
static Socket* socket_table_get(int handle) {
    if (handle < 0 || handle >= MAX_SOCKETS) {
        return NULL;
    }
    return g_socket_table[handle];
}

/* Remove socket from table */
static void socket_table_remove(int handle) {
    if (handle >= 0 && handle < MAX_SOCKETS) {
        g_socket_table[handle] = NULL;
    }
}

static int native_socket_create(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t host_idx;
    uint16_t port;
    const char* host_str;
    String* host;
    Socket* sock;
    int handle;
    
    (void)arg_count;
    
    /* Initialize socket subsystem if needed */
    if (!socket_subsystem_is_initialized()) {
        if (socket_subsystem_init() != 0) {
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
        return -1;
    }
    
    /* Create String object for host */
    host = string_new(host_str);
    if (!host) {
        return -1;
    }
    
    /* Create and connect socket */
    sock = socket_runtime_new_connected(host, port);
    if (!sock) {
        string_delete(host);
        return -1;
    }
    
    /* Add socket to global table and get handle (0-3) */
    handle = socket_table_add(sock);
    if (handle < 0) {
        socket_runtime_delete(sock);
        string_delete(host);
        return -1;
    }
    
    /* Return socket table handle as int */
    *result = (uint16_t)handle;
    return 0;
}

/**
 * Socket.send(int handle, String data) - Send data through socket
 * Returns number of bytes sent
 */
static int native_socket_send(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    int handle;
    uint16_t data_idx;
    Socket* sock;
    const char* data_str;
    int sent;
    
    (void)arg_count;
    
    /* Get arguments: socket handle (int), data (String constant index) */
    handle = (int)args[0];
    data_idx = args[1];
    
    /* Get socket object from table */
    sock = socket_table_get(handle);
    if (!sock) {
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
        return -1;
    }
    
    /* Send data */
    sent = socket_runtime_send(sock, (const uint8_t*)data_str, (uint16_t)strlen(data_str));
    if (sent < 0) {
        return -1;
    }
    
    *result = (uint16_t)sent;
    return 0;
}

/**
 * Socket.recv(int handle) - Receive data from socket
 * Returns received data as String
 */
static int native_socket_recv(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    int handle;
    Socket* sock;
    uint8_t buffer[256];
    int received;
    uint16_t const_idx;
    
    (void)arg_count;
    
    /* Get argument: socket handle (int) */
    handle = (int)args[0];
    
    /* Get socket object from table */
    sock = socket_table_get(handle);
    if (!sock) {
        return -1;
    }
    
    /* Receive data (use full buffer size) */
    received = socket_runtime_recv(sock, buffer, sizeof(buffer) - 1);
    if (received < 0) {
        return -1;
    }
    
    /* Null-terminate received data */
    buffer[received] = '\0';
    
    /* Add received data to constant pool */
    const_idx = djc_add_string(ctx->djc_file, (const char*)buffer);
    if (const_idx == 0) {
        return -1;
    }
    
    *result = const_idx;
    return 0;
}

/**
 * Socket.close(int handle) - Close socket
 */
static int native_socket_close(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    int handle;
    Socket* sock;
    
    (void)ctx;
    (void)arg_count;
    (void)result;
    
    /* Get argument: socket handle (int) */
    handle = (int)args[0];
    
    /* Get socket object from table */
    sock = socket_table_get(handle);
    if (!sock) {
        return -1;
    }
    
    /* Close socket */
    if (socket_runtime_close(sock) != 0) {
        return -1;
    }
    
    /* Remove from socket table and delete */
    socket_table_remove(handle);
    socket_runtime_delete(sock);
    
    return 0;
}

/**
 * Socket.isConnected(int handle) - Check if socket is connected
 * Returns 1 if connected, 0 otherwise
 */
static int native_socket_is_connected(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    int handle;
    Socket* sock;
    
    (void)ctx;
    (void)arg_count;
    
    /* Get argument: socket handle (int) */
    handle = (int)args[0];
    
    /* Get socket object from table */
    sock = socket_table_get(handle);
    if (!sock) {
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
    static NativeParamType param_string_int[] = { NATIVE_PARAM_STRING, NATIVE_PARAM_INT };
    static NativeParamType param_int_string[] = { NATIVE_PARAM_INT, NATIVE_PARAM_STRING };
    static NativeParamType param_int[] = { NATIVE_PARAM_INT };
    static NativeParamType param_object_string[] = { NATIVE_PARAM_OBJECT, NATIVE_PARAM_STRING };
    static NativeParamType param_object_int[] = { NATIVE_PARAM_OBJECT, NATIVE_PARAM_INT };
    static NativeParamType param_object[] = { NATIVE_PARAM_OBJECT };
    
    /* Socket.init() - no parameters, so param_types is NULL */
    if (native_register(
        "java/net/Socket",
        "init",
        "()V",
        native_socket_init,
        0,
        NULL,
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* Socket.create(String, int) - Static factory method returns int handle */
    if (native_register(
        "Socket",
        "create",
        "(Ljava/lang/String;I)I",
        native_socket_create,
        2,
        param_string_int,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    /* Socket.send(int sock, String data) */
    if (native_register(
        "Socket",
        "send",
        "(ILjava/lang/String;)I",
        native_socket_send,
        2,
        param_int_string,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    /* Socket.recv(int sock) */
    if (native_register(
        "Socket",
        "recv",
        "(I)Ljava/lang/String;",
        native_socket_recv,
        1,
        param_int,
        NATIVE_RETURN_STRING
    ) != 0) {
        return -1;
    }
    
    /* Socket.close(int sock) */
    if (native_register(
        "Socket",
        "close",
        "(I)V",
        native_socket_close,
        1,
        param_int,
        NATIVE_RETURN_VOID
    ) != 0) {
        return -1;
    }
    
    /* Socket.isConnected(int sock) */
    if (native_register(
        "Socket",
        "isConnected",
        "(I)I",
        native_socket_is_connected,
        1,
        param_int,
        NATIVE_RETURN_INT
    ) != 0) {
        return -1;
    }
    
    return 0;
}
#endif /* ENABLE_SOCKETS */
