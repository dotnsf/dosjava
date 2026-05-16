#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include "object.h"
#include "../types.h"

/**
 * OutputStream - Base class for output streams
 * 
 * This is the C representation of java.io.OutputStream
 * Provides abstract interface for writing bytes to various destinations
 */

/**
 * OutputStream structure
 */
typedef struct OutputStream {
    Object base;              /* Base object */
    uint8_t is_open;          /* Open state flag */
    uint16_t position;        /* Current position in stream */
    
    /* Virtual function pointers for polymorphism */
    void (*write)(void* stream, int byte);
    void (*write_bytes)(void* stream, const unsigned char* data, int len);
    void (*flush)(void* stream);
    void (*close)(void* stream);
} OutputStream;

/**
 * Initialize an OutputStream
 * @param stream OutputStream to initialize
 * @return 0 on success, -1 on error
 */
int outputstream_init(OutputStream* stream);

/**
 * Close an OutputStream
 * Subclasses should override this to release resources
 * @param stream OutputStream to close
 */
void outputstream_close(OutputStream* stream);

/**
 * Flush any buffered output
 * Subclasses should override this if they buffer data
 * @param stream OutputStream to flush
 */
void outputstream_flush(OutputStream* stream);

/**
 * Write a single byte to the stream
 * This is a virtual function - subclasses must implement
 * @param stream OutputStream to write to
 * @param byte Byte value to write (0-255)
 * @return 0 on success, -1 on error
 */
int outputstream_write(OutputStream* stream, uint8_t byte);

/**
 * Write multiple bytes from a buffer
 * This is a virtual function - subclasses must implement
 * @param stream OutputStream to write to
 * @param buf Buffer containing bytes to write
 * @param len Number of bytes to write
 * @return Number of bytes actually written, or -1 on error
 */
int outputstream_write_bytes(OutputStream* stream, const uint8_t* buf, uint16_t len);

/**
 * Check if stream is open
 * @param stream OutputStream to check
 * @return 1 if open, 0 if closed
 */
uint8_t outputstream_is_open(OutputStream* stream);

/**
 * Get current position in stream
 * @param stream OutputStream to query
 * @return Current position
 */
uint16_t outputstream_get_position(OutputStream* stream);

#endif /* OUTPUTSTREAM_H */
