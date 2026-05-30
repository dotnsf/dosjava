/**
 * HTTP Client for DOSJava
 * 
 * Provides HTTP client functionality by integrating with doscurl.exe
 * Uses external process execution and temporary files for data exchange
 */

#ifndef HTTP_H
#define HTTP_H

#include "interpreter.h"

/**
 * Native method: Http.get(String url)
 * 
 * Executes HTTP GET request using doscurl.exe
 * 
 * @param ctx Execution context
 * @param args Arguments array [url]
 * @param arg_count Number of arguments (must be 1)
 * @param result Pointer to store result (String constant pool index)
 * @return 0 on success, -1 on error (exception thrown)
 */
int native_http_get(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result);

/**
 * Native method: Http.get(String url, String headers)
 *
 * Executes HTTP GET request with custom headers using doscurl.exe
 * Headers should be newline-separated (e.g., "Content-Type: application/json\nUser-Agent: MyApp")
 *
 * @param ctx Execution context
 * @param args Arguments array [url, headers]
 * @param arg_count Number of arguments (must be 2)
 * @param result Pointer to store result (String constant pool index)
 * @return 0 on success, -1 on error (exception thrown)
 */
int native_http_get_with_headers(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result);

/**
 * Native method: Http.getStatusCode(String url)
 *
 * Executes HTTP GET request using doscurl.exe and returns HTTP status code
 *
 * @param ctx Execution context
 * @param args Arguments array [url]
 * @param arg_count Number of arguments (must be 1)
 * @param result Pointer to store result (status code as int)
 * @return 0 on success, -1 on error (exception thrown)
 */
int native_http_getStatusCode(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result);

/**
 * Native method: Http.post(String url, String data)
 *
 * Executes HTTP POST request using doscurl.exe
 *
 * @param ctx Execution context
 * @param args Arguments array [url, data]
 * @param arg_count Number of arguments (must be 2)
 * @param result Pointer to store result (String constant pool index)
 * @return 0 on success, -1 on error (exception thrown)
 */
int native_http_post(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result);

/**
 * Native method: Http.post(String url, String data, String headers)
 *
 * Executes HTTP POST request with custom headers using doscurl.exe
 * Headers should be newline-separated (e.g., "Content-Type: application/json\nUser-Agent: MyApp")
 *
 * @param ctx Execution context
 * @param args Arguments array [url, data, headers]
 * @param arg_count Number of arguments (must be 3)
 * @param result Pointer to store result (String constant pool index)
 * @return 0 on success, -1 on error (exception thrown)
 */
int native_http_post_with_headers(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result);

/**
 * Native method: Http.put(String url, String data)
 *
 * Executes HTTP PUT request using doscurl.exe
 *
 * @param ctx Execution context
 * @param args Arguments array [url, data]
 * @param arg_count Number of arguments (must be 2)
 * @param result Pointer to store result (String constant pool index)
 * @return 0 on success, -1 on error (exception thrown)
 */
int native_http_put(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result);

/**
 * Native method: Http.put(String url, String data, String headers)
 *
 * Executes HTTP PUT request with custom headers using doscurl.exe
 * Headers should be newline-separated (e.g., "Content-Type: application/json\nUser-Agent: MyApp")
 *
 * @param ctx Execution context
 * @param args Arguments array [url, data, headers]
 * @param arg_count Number of arguments (must be 3)
 * @param result Pointer to store result (String constant pool index)
 * @return 0 on success, -1 on error (exception thrown)
 */
int native_http_put_with_headers(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result);

/**
 * Native method: Http.delete(String url)
 *
 * Executes HTTP DELETE request using doscurl.exe
 *
 * @param ctx Execution context
 * @param args Arguments array [url]
 * @param arg_count Number of arguments (must be 1)
 * @param result Pointer to store result (String constant pool index)
 * @return 0 on success, -1 on error (exception thrown)
 */
int native_http_delete(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result);

/**
 * Native method: Http.delete(String url, String headers)
 *
 * Executes HTTP DELETE request with custom headers using doscurl.exe
 * Headers should be newline-separated (e.g., "Content-Type: application/json\nUser-Agent: MyApp")
 *
 * @param ctx Execution context
 * @param args Arguments array [url, headers]
 * @param arg_count Number of arguments (must be 2)
 * @param result Pointer to store result (String constant pool index)
 * @return 0 on success, -1 on error (exception thrown)
 */
int native_http_delete_with_headers(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result);

#endif /* HTTP_H */
