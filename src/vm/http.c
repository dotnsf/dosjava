/**
 * HTTP Client for DOSJava
 *
 * Provides HTTP client functionality by integrating with doscurl.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "interpreter.h"
#include "../format/djc.h"

/**
 * Global proxy URL storage
 */
static char proxy_url[256] = "";

/**
 * Get string from constant pool
 */
static const char* get_string_from_constant_pool(ExecutionContext* ctx, uint16_t index) {
    if (index >= ctx->djc_file->header.constant_pool_count) {
        return NULL;
    }
    
    if (ctx->djc_file->constants[index].tag != CONST_UTF8) {
        return NULL;
    }
    
    return ctx->djc_file->constants[index].data.utf8_data;
}

/**
 * Native method: Http.get(String url)
 * 
 * Executes HTTP GET request using doscurl.exe
 */
int native_http_get(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    const char* url;
    char cmd[256];
    char* response;
    FILE* f;
    int exit_code;
    size_t len;
    size_t total_len;
    size_t bytes_read;
    uint16_t const_idx;
    char buffer[512];
    
    /* Argument check */
    if (arg_count != 1) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.get requires 1 argument");
    }
    
    /* Get URL from constant pool */
    url = get_string_from_constant_pool(ctx, args[0]);
    if (!url) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "URL cannot be null");
    }
    
    /* Build doscurl command with proxy support */
    if (proxy_url[0] != '\0') {
        sprintf(cmd, "doscurl.exe --proxy %s -o _http.tmp %s 2>_err.tmp", proxy_url, url);
    } else {
        sprintf(cmd, "doscurl.exe -o _http.tmp %s 2>_err.tmp", url);
    }
    
    /* Execute doscurl */
    exit_code = system(cmd);
    
    /* Note: In DOS, system() return value is unreliable, especially with redirects.
     * Instead of checking exit_code, we check if the output file was created.
     */
    (void)exit_code;  /* Suppress unused variable warning */
    
    /* Read response file */
    f = fopen("_http.tmp", "rb");
    if (!f) {
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO,
                                       "Failed to read HTTP response");
    }
    
    /* Allocate buffer for response (2KB max for DOS) */
    response = (char*)malloc(2048);
    if (!response) {
        fclose(f);
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Out of memory");
    }
    
    /* Read entire file in chunks */
    total_len = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (total_len + bytes_read >= 2047) {
            /* Truncate if too large */
            bytes_read = 2047 - total_len;
            memcpy(response + total_len, buffer, bytes_read);
            total_len += bytes_read;
            break;
        }
        memcpy(response + total_len, buffer, bytes_read);
        total_len += bytes_read;
    }
    response[total_len] = '\0';
    
    fclose(f);
    
    /* Check if we got any data */
    if (total_len == 0) {
        free(response);
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO,
                                       "Empty HTTP response");
    }
    
    /* Remove trailing whitespace */
    len = total_len;
    while (len > 0 && (response[len-1] == '\n' || response[len-1] == '\r' || response[len-1] == ' ')) {
        response[len-1] = '\0';
        len--;
    }
    
    /* Clean up temporary files */
    remove("_http.tmp");
    remove("_err.tmp");
    
    /* Add response string to constant pool */
    const_idx = djc_add_string(ctx->djc_file, response);
    free(response);
    
    if (const_idx == 0xFFFF) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to create response string");
    }
    
    *result = const_idx;
    return 0;
}

/**
 * Native method: Http.get(String url, String headers)
 * Executes HTTP GET request with custom headers
 * Headers are newline-separated (e.g., "Content-Type: application/json\nUser-Agent: MyApp")
 */
int native_http_get_with_headers(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    const char* url;
    const char* headers;
    char* response;
    FILE* f;
    int exit_code;
    size_t len;
    uint16_t const_idx;
    size_t total_len;
    size_t bytes_read;
    char buffer[512];
    char header_opts[256];
    const char* p;
    const char* line_start;
    int header_len;
    
    /* Argument check */
    if (arg_count != 2) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.get requires 1 or 2 arguments");
    }
    
    /* Get URL from constant pool */
    url = get_string_from_constant_pool(ctx, args[0]);
    if (!url) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "URL cannot be null");
    }
    
    /* Get headers from constant pool */
    headers = get_string_from_constant_pool(ctx, args[1]);
    if (!headers) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "Headers cannot be null");
    }
    
    /* Build header options: parse newline-separated headers and create -H options */
    header_opts[0] = '\0';
    p = headers;
    line_start = p;
    
    while (*p) {
        if (*p == '\n' || *p == '\r') {
            /* Found end of line */
            header_len = (int)(p - line_start);
            if (header_len > 0 && header_len < 100) {
                /* Add -H "header" option */
                /* Need space for: " -H \"" (5) + header + "\"" (1) + null (1) = header_len + 7 */
                if (strlen(header_opts) + header_len + 7 <= sizeof(header_opts)) {
                    strcat(header_opts, " -H \"");
                    strncat(header_opts, line_start, header_len);
                    strcat(header_opts, "\"");
                }
            }
            /* Skip CR/LF */
            while (*p == '\n' || *p == '\r') p++;
            line_start = p;
        } else {
            p++;
        }
    }
    
    /* Handle last line if no trailing newline */
    header_len = (int)(p - line_start);
    if (header_len > 0 && header_len < 100) {
        if (strlen(header_opts) + header_len + 7 <= sizeof(header_opts)) {
            strcat(header_opts, " -H \"");
            strncat(header_opts, line_start, header_len);
            strcat(header_opts, "\"");
        }
    }
    
    /* Build doscurl command with headers */
    /* Note: DOS has 127-char command line limit, so use batch file for long commands */
    {
        FILE* batch_file;
        batch_file = fopen("_http.bat", "w");
        if (batch_file) {
            fprintf(batch_file, "@echo off\n");
            if (proxy_url[0] != '\0') {
                fprintf(batch_file, "doscurl.exe --proxy %s%s -o _http.tmp %s 2>_err.tmp\n", proxy_url, header_opts, url);
            } else {
                fprintf(batch_file, "doscurl.exe%s -o _http.tmp %s 2>_err.tmp\n", header_opts, url);
            }
            fclose(batch_file);
            exit_code = system("_http.bat");
            remove("_http.bat");
        } else {
            return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO,
                                           "Failed to create batch file");
        }
    }
    
    /* Note: In DOS, system() return value is unreliable, so we check file existence */
    (void)exit_code;
    
    /* Check if output file exists */
    f = fopen("_http.tmp", "rb");
    if (!f) {
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NETWORK,
                                       "HTTP GET request failed");
    }
    
    /* Allocate buffer for response (2KB max) */
    response = (char*)malloc(2048);
    if (!response) {
        fclose(f);
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to allocate memory for response");
    }
    
    /* Read response in chunks */
    total_len = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (total_len + bytes_read >= 2048) {
            /* Truncate if response is too large */
            bytes_read = 2048 - total_len - 1;
            if (bytes_read > 0) {
                memcpy(response + total_len, buffer, bytes_read);
                total_len += bytes_read;
            }
            break;
        }
        memcpy(response + total_len, buffer, bytes_read);
        total_len += bytes_read;
    }
    response[total_len] = '\0';
    
    fclose(f);
    
    /* Check if we got any data */
    if (total_len == 0) {
        free(response);
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO,
                                       "Empty HTTP response");
    }
    
    /* Remove trailing whitespace */
    len = total_len;
    while (len > 0 && (response[len-1] == '\n' || response[len-1] == '\r' || response[len-1] == ' ')) {
        response[len-1] = '\0';
        len--;
    }
    
    /* Clean up temporary files */
    remove("_http.tmp");
    remove("_err.tmp");
    
    /* Add response string to constant pool */
    const_idx = djc_add_string(ctx->djc_file, response);
    free(response);
    
    if (const_idx == 0xFFFF) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to create response string");
    }
    
    *result = const_idx;
    return 0;
}

/**
 * Native method: Http.getStatusCode(String url)
 *
 * Executes HTTP GET request and returns HTTP status code
 */
int native_http_getStatusCode(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    const char* url;
    char cmd[256];
    FILE* f;
    int exit_code;
    int status_code;
    char line[256];
    char* status_pos;
    
    /* Argument check */
    if (arg_count != 1) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.getStatusCode requires 1 argument");
    }
    
    /* Get URL from constant pool */
    url = get_string_from_constant_pool(ctx, args[0]);
    if (!url) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "URL cannot be null");
    }
    
    /* Build doscurl command with -i option to include headers in output */
    sprintf(cmd, "doscurl.exe -i -o _http.tmp %s 2>_err.tmp", url);
    
    /* Execute doscurl */
    exit_code = system(cmd);
    
    /* Note: In DOS, system() return value is unreliable */
    (void)exit_code;
    
    /* Read response from _http.tmp */
    f = fopen("_http.tmp", "r");
    if (!f) {
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO,
                                       "Failed to read HTTP response");
    }
    
    /* Read first line: HTTP/1.1 200 OK */
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO,
                                       "Failed to read HTTP status line");
    }
    
    fclose(f);
    
    /* Parse status code from "HTTP/1.1 200 OK" */
    status_pos = line;
    /* Skip "HTTP/1.x " */
    while (*status_pos && *status_pos != ' ') status_pos++;
    if (*status_pos) status_pos++;
    
    /* Parse status code */
    status_code = 0;
    while (*status_pos >= '0' && *status_pos <= '9') {
        status_code = status_code * 10 + (*status_pos - '0');
        status_pos++;
    }
    
    /* Clean up temporary files */
    remove("_http.tmp");
    remove("_err.tmp");
    
    /* Validate status code */
    if (status_code < 100 || status_code > 599) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO,
                                       "Invalid HTTP status code");
    }
    
    /* Return status code as int */
    *result = (uint16_t)status_code;
    return 0;
}

/**
 * Native method implementation: Http.post(String url, String data)
 * Executes HTTP POST request using doscurl.exe with -d option
 */
int native_http_post(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    char cmd[512];
    const char* url;
    const char* data;
    FILE* f;
    int exit_code;
    char* response;
    size_t total_size;
    size_t bytes_read;
    char buffer[512];
    
    /* Argument check */
    if (arg_count != 2) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.post requires 2 arguments");
    }
    
    /* Get URL from constant pool */
    url = get_string_from_constant_pool(ctx, args[0]);
    if (!url) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "URL cannot be null");
    }
    
    /* Get POST data from constant pool */
    data = get_string_from_constant_pool(ctx, args[1]);
    if (!data) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "POST data cannot be null");
    }
    
    /* Build doscurl command with -d option for POST and proxy support */
    if (proxy_url[0] != '\0') {
        sprintf(cmd, "doscurl.exe --proxy %s -d \"%s\" -o _http.tmp %s 2>_err.tmp", proxy_url, data, url);
    } else {
        sprintf(cmd, "doscurl.exe -d \"%s\" -o _http.tmp %s 2>_err.tmp", data, url);
    }
    
    /* Execute doscurl */
    exit_code = system(cmd);
    
    /* Note: In DOS, system() return value is unreliable, so we check file existence */
    (void)exit_code;
    
    /* Check if output file exists */
    f = fopen("_http.tmp", "rb");
    if (!f) {
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NETWORK,
                                       "HTTP POST request failed");
    }
    
    /* Allocate buffer for response (2KB max) */
    response = (char*)malloc(2048);
    if (!response) {
        fclose(f);
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to allocate memory for response");
    }
    
    /* Read response in chunks */
    total_size = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (total_size + bytes_read >= 2048) {
            /* Truncate if response is too large */
            bytes_read = 2048 - total_size - 1;
            if (bytes_read > 0) {
                memcpy(response + total_size, buffer, bytes_read);
                total_size += bytes_read;
            }
            break;
        }
        memcpy(response + total_size, buffer, bytes_read);
        total_size += bytes_read;
    }
    
    fclose(f);
    
    /* Null-terminate the response */
    response[total_size] = '\0';
    
    /* Clean up temporary files */
    remove("_http.tmp");
    remove("_err.tmp");
    
    /* Add response to constant pool */
    *result = djc_add_string(ctx->djc_file, response);
    
    /* Free allocated memory */
    free(response);
    
    if (*result == 0xFFFF) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to add response to constant pool");
    }
    
    return 0;
}

/**
 * Native method implementation: Http.put(String url, String data)
 * Executes HTTP PUT request using doscurl.exe with -X PUT option
 */
int native_http_put(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    char cmd[512];
    const char* url;
    const char* data;
    FILE* f;
    int exit_code;
    char* response;
    size_t total_size;
    size_t bytes_read;
    char buffer[512];
    
    /* Argument check */
    if (arg_count != 2) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.put requires 2 arguments");
    }
    
    /* Get URL from constant pool */
    url = get_string_from_constant_pool(ctx, args[0]);
    if (!url) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "URL cannot be null");
    }
    
    /* Get PUT data from constant pool */
    data = get_string_from_constant_pool(ctx, args[1]);
    if (!data) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "PUT data cannot be null");
    }
    
    /* Build doscurl command with -X PUT option and proxy support */
    if (proxy_url[0] != '\0') {
        sprintf(cmd, "doscurl.exe --proxy %s -X PUT -d \"%s\" -o _http.tmp %s 2>_err.tmp", proxy_url, data, url);
    } else {
        sprintf(cmd, "doscurl.exe -X PUT -d \"%s\" -o _http.tmp %s 2>_err.tmp", data, url);
    }
    
    /* Execute doscurl */
    exit_code = system(cmd);
    
    /* Note: In DOS, system() return value is unreliable */
    (void)exit_code;
    
    /* Check if output file exists */
    f = fopen("_http.tmp", "rb");
    if (!f) {
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NETWORK,
                                       "HTTP PUT request failed");
    }
    
    /* Allocate buffer for response (2KB max) */
    response = (char*)malloc(2048);
    if (!response) {
        fclose(f);
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to allocate memory for response");
    }
    
    /* Read response in chunks */
    total_size = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (total_size + bytes_read >= 2048) {
            /* Truncate if response is too large */
            bytes_read = 2048 - total_size - 1;
            if (bytes_read > 0) {
                memcpy(response + total_size, buffer, bytes_read);
                total_size += bytes_read;
            }
            break;
        }
        memcpy(response + total_size, buffer, bytes_read);
        total_size += bytes_read;
    }
    
    fclose(f);
    
    /* Null-terminate the response */
    response[total_size] = '\0';
    
    /* Clean up temporary files */
    remove("_http.tmp");
    remove("_err.tmp");
    
    /* Add response to constant pool */
    *result = djc_add_string(ctx->djc_file, response);
    
    /* Free allocated memory */
    free(response);
    
    if (*result == 0xFFFF) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to add response to constant pool");
    }
    
    return 0;
}

/**
 * Native method implementation: Http.delete(String url)
 * Executes HTTP DELETE request using doscurl.exe with -X DELETE option
 */
int native_http_delete(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    char cmd[512];
    const char* url;
    FILE* f;
    int exit_code;
    char* response;
    size_t total_size;
    size_t bytes_read;
    char buffer[512];
    
    /* Argument check */
    if (arg_count != 1) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.delete requires 1 argument");
    }
    
    /* Get URL from constant pool */
    url = get_string_from_constant_pool(ctx, args[0]);
    if (!url) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "URL cannot be null");
    }
    
    /* Build doscurl command with -X DELETE option and proxy support */
    if (proxy_url[0] != '\0') {
        sprintf(cmd, "doscurl.exe --proxy %s -X DELETE -o _http.tmp %s 2>_err.tmp", proxy_url, url);
    } else {
        sprintf(cmd, "doscurl.exe -X DELETE -o _http.tmp %s 2>_err.tmp", url);
    }
    
    /* Execute doscurl */
    exit_code = system(cmd);
    
    /* Note: In DOS, system() return value is unreliable */
    (void)exit_code;
    
    /* Check if output file exists */
    f = fopen("_http.tmp", "rb");
    if (!f) {
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NETWORK,
                                       "HTTP DELETE request failed");
    }
    
    /* Allocate buffer for response (2KB max) */
    response = (char*)malloc(2048);
    if (!response) {
        fclose(f);
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to allocate memory for response");
    }
    
    /* Read response in chunks */
    total_size = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (total_size + bytes_read >= 2048) {
            /* Truncate if response is too large */
            bytes_read = 2048 - total_size - 1;
            if (bytes_read > 0) {
                memcpy(response + total_size, buffer, bytes_read);
                total_size += bytes_read;
            }
            break;
        }
        memcpy(response + total_size, buffer, bytes_read);
        total_size += bytes_read;
    }
    
    fclose(f);
    
    /* Null-terminate the response */
    response[total_size] = '\0';
    
    /* Clean up temporary files */
    remove("_http.tmp");
    remove("_err.tmp");
    
    /* Add response to constant pool */
    *result = djc_add_string(ctx->djc_file, response);
    
    /* Free allocated memory */
    free(response);
    
    if (*result == 0xFFFF) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to add response to constant pool");
    }
    
    return 0;
}


/**
 * Native method: Http.post(String url, String data, String headers)
 * Executes HTTP POST request with custom headers
 * Headers are newline-separated (e.g., "Content-Type: application/json\nUser-Agent: MyApp")
 */
int native_http_post_with_headers(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    const char* url;
    const char* data;
    const char* headers;
    char* response;
    FILE* f;
    int exit_code;
    size_t len;
    uint16_t const_idx;
    size_t total_len;
    size_t bytes_read;
    char buffer[512];
    char header_opts[256];
    const char* p;
    const char* line_start;
    int header_len;
    
    /* Argument check */
    if (arg_count != 3) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.post requires 2 or 3 arguments");
    }
    
    /* Get URL from constant pool */
    url = get_string_from_constant_pool(ctx, args[0]);
    if (!url) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "URL cannot be null");
    }
    
    /* Get POST data from constant pool */
    data = get_string_from_constant_pool(ctx, args[1]);
    if (!data) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "POST data cannot be null");
    }
    
    /* Get headers from constant pool */
    headers = get_string_from_constant_pool(ctx, args[2]);
    if (!headers) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "Headers cannot be null");
    }
    
    /* Build header options: parse newline-separated headers and create -H options */
    header_opts[0] = '\0';
    p = headers;
    line_start = p;
    
    while (*p) {
        if (*p == '\n' || *p == '\r') {
            /* Found end of line */
            header_len = (int)(p - line_start);
            if (header_len > 0 && header_len < 100) {
                /* Add -H "header" option */
                if (strlen(header_opts) + header_len + 7 <= sizeof(header_opts)) {
                    strcat(header_opts, " -H \"");
                    strncat(header_opts, line_start, header_len);
                    strcat(header_opts, "\"");
                }
            }
            /* Skip CR/LF */
            while (*p == '\n' || *p == '\r') p++;
            line_start = p;
        } else {
            p++;
        }
    }
    
    /* Handle last line if no trailing newline */
    header_len = (int)(p - line_start);
    if (header_len > 0 && header_len < 100) {
        if (strlen(header_opts) + header_len + 7 <= sizeof(header_opts)) {
            strcat(header_opts, " -H \"");
            strncat(header_opts, line_start, header_len);
            strcat(header_opts, "\"");
        }
    }
    
    /* Build doscurl command with headers using batch file */
    {
        FILE* batch_file;
        batch_file = fopen("_http.bat", "w");
        if (batch_file) {
            fprintf(batch_file, "@echo off\n");
            if (proxy_url[0] != '\0') {
                fprintf(batch_file, "doscurl.exe --proxy %s -d \"%s\"%s -o _http.tmp %s 2>_err.tmp\n", proxy_url, data, header_opts, url);
            } else {
                fprintf(batch_file, "doscurl.exe -d \"%s\"%s -o _http.tmp %s 2>_err.tmp\n", data, header_opts, url);
            }
            fclose(batch_file);
            exit_code = system("_http.bat");
            remove("_http.bat");
        } else {
            return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO,
                                           "Failed to create batch file");
        }
    }
    
    /* Note: In DOS, system() return value is unreliable, so we check file existence */
    (void)exit_code;
    
    /* Check if output file exists */
    f = fopen("_http.tmp", "rb");
    if (!f) {
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NETWORK,
                                       "HTTP POST request failed");
    }
    
    /* Allocate buffer for response (2KB max) */
    response = (char*)malloc(2048);
    if (!response) {
        fclose(f);
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to allocate memory for response");
    }
    
    /* Read response in chunks */
    total_len = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (total_len + bytes_read >= 2048) {
            /* Truncate if response is too large */
            bytes_read = 2048 - total_len - 1;
            if (bytes_read > 0) {
                memcpy(response + total_len, buffer, bytes_read);
                total_len += bytes_read;
            }
            break;
        }
        memcpy(response + total_len, buffer, bytes_read);
        total_len += bytes_read;
    }
    response[total_len] = '\0';
    
    fclose(f);
    
    /* Remove trailing whitespace */
    len = total_len;
    while (len > 0 && (response[len-1] == '\n' || response[len-1] == '\r' || response[len-1] == ' ')) {
        response[len-1] = '\0';
        len--;
    }
    
    /* Clean up temporary files */
    remove("_http.tmp");
    remove("_err.tmp");
    
    /* Add response string to constant pool */
    const_idx = djc_add_string(ctx->djc_file, response);
    free(response);
    
    if (const_idx == 0xFFFF) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to add response to constant pool");
    }
    
    *result = const_idx;
    return 0;
}

/**
 * Native method: Http.put(String url, String data, String headers)
 * Executes HTTP PUT request with custom headers
 * Headers are newline-separated (e.g., "Content-Type: application/json\nUser-Agent: MyApp")
 */
int native_http_put_with_headers(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    const char* url;
    const char* data;
    const char* headers;
    char* response;
    FILE* f;
    int exit_code;
    size_t len;
    uint16_t const_idx;
    size_t total_len;
    size_t bytes_read;
    char buffer[512];
    char header_opts[256];
    const char* p;
    const char* line_start;
    int header_len;
    
    /* Argument check */
    if (arg_count != 3) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.put requires 2 or 3 arguments");
    }
    
    /* Get URL from constant pool */
    url = get_string_from_constant_pool(ctx, args[0]);
    if (!url) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "URL cannot be null");
    }
    
    /* Get PUT data from constant pool */
    data = get_string_from_constant_pool(ctx, args[1]);
    if (!data) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "PUT data cannot be null");
    }
    
    /* Get headers from constant pool */
    headers = get_string_from_constant_pool(ctx, args[2]);
    if (!headers) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "Headers cannot be null");
    }
    
    /* Build header options: parse newline-separated headers and create -H options */
    header_opts[0] = '\0';
    p = headers;
    line_start = p;
    
    while (*p) {
        if (*p == '\n' || *p == '\r') {
            /* Found end of line */
            header_len = (int)(p - line_start);
            if (header_len > 0 && header_len < 100) {
                /* Add -H "header" option */
                if (strlen(header_opts) + header_len + 7 <= sizeof(header_opts)) {
                    strcat(header_opts, " -H \"");
                    strncat(header_opts, line_start, header_len);
                    strcat(header_opts, "\"");
                }
            }
            /* Skip CR/LF */
            while (*p == '\n' || *p == '\r') p++;
            line_start = p;
        } else {
            p++;
        }
    }
    
    /* Handle last line if no trailing newline */
    header_len = (int)(p - line_start);
    if (header_len > 0 && header_len < 100) {
        if (strlen(header_opts) + header_len + 7 <= sizeof(header_opts)) {
            strcat(header_opts, " -H \"");
            strncat(header_opts, line_start, header_len);
            strcat(header_opts, "\"");
        }
    }
    
    /* Build doscurl command with headers using batch file */
    {
        FILE* batch_file;
        batch_file = fopen("_http.bat", "w");
        if (batch_file) {
            fprintf(batch_file, "@echo off\n");
            if (proxy_url[0] != '\0') {
                fprintf(batch_file, "doscurl.exe --proxy %s -X PUT -d \"%s\"%s -o _http.tmp %s 2>_err.tmp\n", proxy_url, data, header_opts, url);
            } else {
                fprintf(batch_file, "doscurl.exe -X PUT -d \"%s\"%s -o _http.tmp %s 2>_err.tmp\n", data, header_opts, url);
            }
            fclose(batch_file);
            exit_code = system("_http.bat");
            remove("_http.bat");
        } else {
            return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO,
                                           "Failed to create batch file");
        }
    }
    
    /* Note: In DOS, system() return value is unreliable, so we check file existence */
    (void)exit_code;
    
    /* Check if output file exists */
    f = fopen("_http.tmp", "rb");
    if (!f) {
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NETWORK,
                                       "HTTP PUT request failed");
    }
    
    /* Allocate buffer for response (2KB max) */
    response = (char*)malloc(2048);
    if (!response) {
        fclose(f);
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to allocate memory for response");
    }
    
    /* Read response in chunks */
    total_len = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (total_len + bytes_read >= 2048) {
            /* Truncate if response is too large */
            bytes_read = 2048 - total_len - 1;
            if (bytes_read > 0) {
                memcpy(response + total_len, buffer, bytes_read);
                total_len += bytes_read;
            }
            break;
        }
        memcpy(response + total_len, buffer, bytes_read);
        total_len += bytes_read;
    }
    response[total_len] = '\0';
    
    fclose(f);
    
    /* Remove trailing whitespace */
    len = total_len;
    while (len > 0 && (response[len-1] == '\n' || response[len-1] == '\r' || response[len-1] == ' ')) {
        response[len-1] = '\0';
        len--;
    }
    
    /* Clean up temporary files */
    remove("_http.tmp");
    remove("_err.tmp");
    
    /* Add response string to constant pool */
    const_idx = djc_add_string(ctx->djc_file, response);
    free(response);
    
    if (const_idx == 0xFFFF) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to add response to constant pool");
    }
    
    *result = const_idx;
    return 0;
}

/**
 * Native method: Http.delete(String url, String headers)
 * Executes HTTP DELETE request with custom headers
 * Headers are newline-separated (e.g., "Content-Type: application/json\nUser-Agent: MyApp")
 */
int native_http_delete_with_headers(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    const char* url;
    const char* headers;
    char* response;
    FILE* f;
    int exit_code;
    size_t len;
    uint16_t const_idx;
    size_t total_len;
    size_t bytes_read;
    char buffer[512];
    char header_opts[256];
    const char* p;
    const char* line_start;
    int header_len;
    
    /* Argument check */
    if (arg_count != 2) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.delete requires 1 or 2 arguments");
    }
    
    /* Get URL from constant pool */
    url = get_string_from_constant_pool(ctx, args[0]);
    if (!url) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "URL cannot be null");
    }
    
    /* Get headers from constant pool */
    headers = get_string_from_constant_pool(ctx, args[1]);
    if (!headers) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "Headers cannot be null");
    }
    
    /* Build header options: parse newline-separated headers and create -H options */
    header_opts[0] = '\0';
    p = headers;
    line_start = p;
    
    while (*p) {
        if (*p == '\n' || *p == '\r') {
            /* Found end of line */
            header_len = (int)(p - line_start);
            if (header_len > 0 && header_len < 100) {
                /* Add -H "header" option */
                if (strlen(header_opts) + header_len + 7 <= sizeof(header_opts)) {
                    strcat(header_opts, " -H \"");
                    strncat(header_opts, line_start, header_len);
                    strcat(header_opts, "\"");
                }
            }
            /* Skip CR/LF */
            while (*p == '\n' || *p == '\r') p++;
            line_start = p;
        } else {
            p++;
        }
    }
    
    /* Handle last line if no trailing newline */
    header_len = (int)(p - line_start);
    if (header_len > 0 && header_len < 100) {
        if (strlen(header_opts) + header_len + 7 <= sizeof(header_opts)) {
            strcat(header_opts, " -H \"");
            strncat(header_opts, line_start, header_len);
            strcat(header_opts, "\"");
        }
    }
    
    /* Build doscurl command with headers using batch file */
    {
        FILE* batch_file;
        batch_file = fopen("_http.bat", "w");
        if (batch_file) {
            fprintf(batch_file, "@echo off\n");
            if (proxy_url[0] != '\0') {
                fprintf(batch_file, "doscurl.exe --proxy %s -X DELETE%s -o _http.tmp %s 2>_err.tmp\n", proxy_url, header_opts, url);
            } else {
                fprintf(batch_file, "doscurl.exe -X DELETE%s -o _http.tmp %s 2>_err.tmp\n", header_opts, url);
            }
            fclose(batch_file);
            exit_code = system("_http.bat");
            remove("_http.bat");
        } else {
            return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO,
                                           "Failed to create batch file");
        }
    }
    
    /* Note: In DOS, system() return value is unreliable, so we check file existence */
    (void)exit_code;
    
    /* Check if output file exists */
    f = fopen("_http.tmp", "rb");
    if (!f) {
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NETWORK,
                                       "HTTP DELETE request failed");
    }
    
    /* Allocate buffer for response (2KB max) */
    response = (char*)malloc(2048);
    if (!response) {
        fclose(f);
        remove("_http.tmp");
        remove("_err.tmp");
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to allocate memory for response");
    }
    
    /* Read response in chunks */
    total_len = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (total_len + bytes_read >= 2048) {
            /* Truncate if response is too large */
            bytes_read = 2048 - total_len - 1;
            if (bytes_read > 0) {
                memcpy(response + total_len, buffer, bytes_read);
                total_len += bytes_read;
            }
            break;
        }
        memcpy(response + total_len, buffer, bytes_read);
        total_len += bytes_read;
    }
    response[total_len] = '\0';
    
    fclose(f);
    
    /* Remove trailing whitespace */
    len = total_len;
    while (len > 0 && (response[len-1] == '\n' || response[len-1] == '\r' || response[len-1] == ' ')) {
        response[len-1] = '\0';
        len--;
    }
    
    /* Clean up temporary files */
    remove("_http.tmp");
    remove("_err.tmp");
    
    /* Add response string to constant pool */
    const_idx = djc_add_string(ctx->djc_file, response);
    free(response);
    
    if (const_idx == 0xFFFF) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to add response to constant pool");
    }
    
    *result = const_idx;
    return 0;
}

/**
 * Native method: Http.setProxy(String proxyUrl)
 * Sets the proxy server URL for all subsequent HTTP requests
 */
int native_http_setProxy(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    const char* proxy;
    
    /* Argument check */
    if (arg_count != 1) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.setProxy requires 1 argument");
    }
    
    /* Get proxy URL from constant pool */
    proxy = get_string_from_constant_pool(ctx, args[0]);
    if (!proxy) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER,
                                       "Proxy URL cannot be null");
    }
    
    /* Check proxy URL length */
    if (strlen(proxy) >= sizeof(proxy_url)) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Proxy URL too long (max 255 chars)");
    }
    
    /* Set proxy URL (empty string clears proxy) */
    strcpy(proxy_url, proxy);
    
    *result = 0;
    return 0;
}

/**
 * Native method: Http.clearProxy()
 * Clears the proxy server setting
 */
int native_http_clearProxy(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    /* Argument check */
    if (arg_count != 0) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.clearProxy requires no arguments");
    }
    
    /* Clear proxy URL */
    proxy_url[0] = '\0';
    
    *result = 0;
    return 0;
}

/**
 * Native method: Http.getProxy()
 * Returns the current proxy server URL
 */
int native_http_getProxy(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    uint16_t const_idx;
    
    /* Argument check */
    if (arg_count != 0) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT,
                                       "Http.getProxy requires no arguments");
    }
    
    /* Add proxy URL to constant pool (empty string if not set) */
    const_idx = djc_add_string(ctx->djc_file, proxy_url);
    
    if (const_idx == 0xFFFF) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY,
                                       "Failed to create proxy URL string");
    }
    
    *result = const_idx;
    return 0;
}
