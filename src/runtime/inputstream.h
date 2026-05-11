#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include "object.h"
#include "../types.h"

/**
 * InputStream - Base class for input streams
 * 
 * This is the C representation of java.io.InputStream
 * Provides abstract interface for reading bytes from various sources
 */

/**
 * InputStream structure
 */
typedef struct InputStream {
    Object base;              /* Base object */
    uint8_t is_open;          /* Open state flag */
    uint16_t position;        /* Current position in stream */
    
    /* Virtual function pointers */
    int (*read)(void* stream);
    int (*read_bytes)(void* stream, unsigned char* buf, int len);
    int (*available)(void* stream);
    long (*skip)(void* stream, long n);
    void (*close)(void* stream);
} InputStream;

/**
 * Initialize an InputStream
 * @param stream InputStream to initialize
 * @return 0 on success, -1 on error
 */
int inputstream_init(InputStream* stream);

/**
 * Close an InputStream
 * Subclasses should override this to release resources
 * @param stream InputStream to close
 */
void inputstream_close(InputStream* stream);

/**
 * Read a single byte from the stream
 * This is a virtual function - subclasses must implement
 * @param stream InputStream to read from
 * @return Byte value (0-255), or -1 on EOF or error
 */
int inputstream_read(InputStream* stream);

/**
 * Read multiple bytes into a buffer
 * This is a virtual function - subclasses must implement
 * @param stream InputStream to read from
 * @param buf Buffer to read into
 * @param len Maximum number of bytes to read
 * @return Number of bytes actually read, or -1 on error
 */
int inputstream_read_bytes(InputStream* stream, uint8_t* buf, uint16_t len);

/**
 * Get number of bytes available for reading without blocking
 * This is a virtual function - subclasses must implement
 * @param stream InputStream to check
 * @return Number of bytes available, or -1 on error
 */
int inputstream_available(InputStream* stream);

/**
 * Skip over and discard n bytes of data
 * @param stream InputStream to skip in
 * @param n Number of bytes to skip
 * @return Number of bytes actually skipped, or -1 on error
 */
int inputstream_skip(InputStream* stream, uint16_t n);

/**
 * Check if stream is open
 * @param stream InputStream to check
 * @return 1 if open, 0 if closed
 */
uint8_t inputstream_is_open(InputStream* stream);

/**
 * Get current position in stream
 * @param stream InputStream to query
 * @return Current position
 */
uint16_t inputstream_get_position(InputStream* stream);

#endif /* INPUTSTREAM_H */

// Made with Bob
