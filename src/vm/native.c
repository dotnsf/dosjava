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
 * Socket.init() - Initialize socket subsystem (no-op with external helper)
 */
static int native_socket_init(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    (void)ctx;
    (void)args;
    (void)arg_count;
    (void)result;
    
    /* Socket initialization is now handled by sockhelp.exe */
    return 0;
}

/**
 * Socket.create(String host, int port) - Create and connect socket
 * Returns socket handle (int)
 */

static int native_socket_create(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t host_idx;
    uint16_t port;
    const char* host_str;
    char command[256];
    FILE* fp;
    char line[256];
    char status[32];
    int handle_value;
    
    (void)arg_count;
    
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
        printf("ERROR: Invalid host string\n");
        return -1;
    }
    
    printf("Connecting to %s:%u...\n", host_str, port);
    
    /* Build command to execute sockhelp.exe */
    sprintf(command, "sockhelp.exe connect %s %u", host_str, port);
    
    /* Execute sockhelp.exe */
    {
        int ret = system(command);
        if (ret != 0) {
            printf("ERROR: Failed to execute sockhelp.exe (return code: %d)\n", ret);
            return -1;
        }
    }
    
    /* Read result from SOCK.OUT */
    fp = fopen("SOCK.OUT", "r");
    if (!fp) {
        printf("ERROR: Failed to read SOCK.OUT\n");
        return -1;
    }
    
    /* Parse result */
    status[0] = '\0';
    handle_value = -1;
    
    while (fgets(line, sizeof(line), fp)) {
        char* msg;
        
        if (strncmp(line, "STATUS:", 7) == 0) {
            sscanf(line + 7, "%s", status);
        }
        else if (strncmp(line, "HANDLE:", 7) == 0) {
            sscanf(line + 7, "%d", &handle_value);
        }
        else if (strncmp(line, "MESSAGE:", 8) == 0) {
            /* Print message (strip newline) */
            msg = line + 8;
            while (*msg == ' ') msg++;
            printf("%s", msg);
        }
    }
    
    fclose(fp);
    
    /* Check if successful */
    if (strcmp(status, "OK") != 0 || handle_value < 0) {
        printf("ERROR: Socket connection failed\n");
        return -1;
    }
    
    printf("Connected successfully!\n");
    
    /* Return socket handle */
    *result = (uint16_t)handle_value;
    return 0;
}

/**
 * Socket.send(int handle, String data) - Send data through socket
 * Returns number of bytes sent
 */
static int native_socket_send(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    int handle;
    uint16_t data_idx;
    const char* data_str;
    char command[512];
    FILE* fp;
    char line[256];
    char status[32];
    int bytes_sent;
    
    (void)arg_count;
    
    /* Get arguments: socket handle (int), data (String constant index) */
    handle = (int)args[0];
    data_idx = args[1];
    
    /* Get data string from constant pool */
    data_str = NULL;
    if (data_idx < ctx->djc_file->header.constant_pool_count) {
        if (ctx->djc_file->constants[data_idx].tag == CONST_UTF8) {
            data_str = ctx->djc_file->constants[data_idx].data.utf8_data;
        }
    }
    
    if (!data_str) {
        printf("ERROR: Invalid data string\n");
        return -1;
    }
    
    printf("Sending data...\n");
    
    /* Build command to execute sockhelp.exe */
    sprintf(command, "sockhelp.exe send %d \"%s\"", handle, data_str);
    
    /* Execute sockhelp.exe */
    if (system(command) != 0) {
        printf("ERROR: Failed to execute sockhelp.exe\n");
        return -1;
    }
    
    /* Read result from SOCK.OUT */
    fp = fopen("SOCK.OUT", "r");
    if (!fp) {
        printf("ERROR: Failed to read SOCK.OUT\n");
        return -1;
    }
    
    /* Parse result */
    status[0] = '\0';
    bytes_sent = -1;
    
    while (fgets(line, sizeof(line), fp)) {
        char* msg;
        
        if (strncmp(line, "STATUS:", 7) == 0) {
            sscanf(line + 7, "%s", status);
        }
        else if (strncmp(line, "BYTES:", 6) == 0) {
            sscanf(line + 6, "%d", &bytes_sent);
        }
        else if (strncmp(line, "MESSAGE:", 8) == 0) {
            msg = line + 8;
            while (*msg == ' ') msg++;
            printf("%s", msg);
        }
    }
    
    fclose(fp);
    
    /* Check if successful */
    if (strcmp(status, "OK") != 0 || bytes_sent < 0) {
        printf("ERROR: Socket send failed\n");
        return -1;
    }
    
    printf("Sent %d bytes\n", bytes_sent);
    
    *result = (uint16_t)bytes_sent;
    return 0;
}

/**
 * Socket.recv(int handle) - Receive data from socket
 * Returns received data as String
 */
static int native_socket_recv(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    int handle;
    char command[256];
    FILE* fp;
    char line[256];
    char status[32];
    char data[256];
    uint16_t const_idx;
    
    (void)arg_count;
    
    /* Get argument: socket handle (int) */
    handle = (int)args[0];
    
    printf("Receiving data...\n");
    
    /* Build command to execute sockhelp.exe */
    sprintf(command, "sockhelp.exe recv %d", handle);
    
    /* Execute sockhelp.exe */
    if (system(command) != 0) {
        printf("ERROR: Failed to execute sockhelp.exe\n");
        return -1;
    }
    
    /* Read result from SOCK.OUT */
    fp = fopen("SOCK.OUT", "r");
    if (!fp) {
        printf("ERROR: Failed to read SOCK.OUT\n");
        return -1;
    }
    
    /* Parse result */
    status[0] = '\0';
    data[0] = '\0';
    
    while (fgets(line, sizeof(line), fp)) {
        char* d;
        char* nl;
        char* msg;
        
        if (strncmp(line, "STATUS:", 7) == 0) {
            sscanf(line + 7, "%s", status);
        }
        else if (strncmp(line, "DATA:", 5) == 0) {
            /* Copy data (strip newline) */
            d = line + 5;
            while (*d == ' ') d++;
            strncpy(data, d, sizeof(data) - 1);
            data[sizeof(data) - 1] = '\0';
            /* Remove trailing newline */
            nl = strchr(data, '\n');
            if (nl) *nl = '\0';
        }
        else if (strncmp(line, "MESSAGE:", 8) == 0) {
            msg = line + 8;
            while (*msg == ' ') msg++;
            printf("%s", msg);
        }
    }
    
    fclose(fp);
    
    /* Check if successful */
    if (strcmp(status, "OK") != 0) {
        printf("ERROR: Socket recv failed\n");
        return -1;
    }
    
    printf("Received: %s\n", data);
    
    /* Add received data to constant pool */
    const_idx = djc_add_string(ctx->djc_file, data);
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
    char command[256];
    FILE* fp;
    char line[256];
    char status[32];
    
    (void)ctx;
    (void)arg_count;
    (void)result;
    
    /* Get argument: socket handle (int) */
    handle = (int)args[0];
    
    printf("Closing socket...\n");
    
    /* Build command to execute sockhelp.exe */
    sprintf(command, "sockhelp.exe close %d", handle);
    
    /* Execute sockhelp.exe */
    if (system(command) != 0) {
        printf("ERROR: Failed to execute sockhelp.exe\n");
        return -1;
    }
    
    /* Read result from SOCK.OUT */
    fp = fopen("SOCK.OUT", "r");
    if (!fp) {
        printf("ERROR: Failed to read SOCK.OUT\n");
        return -1;
    }
    
    /* Parse result */
    status[0] = '\0';
    
    while (fgets(line, sizeof(line), fp)) {
        char* msg;
        
        if (strncmp(line, "STATUS:", 7) == 0) {
            sscanf(line + 7, "%s", status);
        }
        else if (strncmp(line, "MESSAGE:", 8) == 0) {
            msg = line + 8;
            while (*msg == ' ') msg++;
            printf("%s", msg);
        }
    }
    
    fclose(fp);
    
    /* Check if successful */
    if (strcmp(status, "OK") != 0) {
        printf("ERROR: Socket close failed\n");
        return -1;
    }
    
    printf("Socket closed\n");
    
    return 0;
}

/**
 * Socket.isConnected(int handle) - Check if socket is connected
 * Returns 1 if connected, 0 otherwise
 */
static int native_socket_is_connected(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    int handle;
    char command[256];
    FILE* fp;
    char line[256];
    char status[32];
    int connected;
    
    (void)ctx;
    (void)arg_count;
    
    /* Get argument: socket handle (int) */
    handle = (int)args[0];
    
    /* Build command to execute sockhelp.exe */
    sprintf(command, "sockhelp.exe status %d", handle);
    
    /* Execute sockhelp.exe */
    if (system(command) != 0) {
        *result = 0;
        return 0;
    }
    
    /* Read result from SOCK.OUT */
    fp = fopen("SOCK.OUT", "r");
    if (!fp) {
        *result = 0;
        return 0;
    }
    
    /* Parse result */
    status[0] = '\0';
    connected = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "STATUS:", 7) == 0) {
            sscanf(line + 7, "%s", status);
        }
        else if (strncmp(line, "CONNECTED:", 10) == 0) {
            sscanf(line + 10, "%d", &connected);
        }
    }
    
    fclose(fp);
    
    *result = (strcmp(status, "OK") == 0 && connected) ? 1 : 0;
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
