#include "outputstream.h"
#include <stdio.h>
#include <string.h>

/**
 * Initialize an OutputStream
 */
int outputstream_init(OutputStream* stream) {
    if (!stream) {
        return -1;
    }
    
    /* Initialize base object */
    stream->base.class_id = 0;  /* Will be set by subclass */
    stream->base.ref_count = 1;
    
    /* Initialize stream state */
    stream->is_open = 1;
    stream->position = 0;
    
    /* Initialize virtual function pointers to default implementations */
    stream->write = (void (*)(void*, int))outputstream_write;
    stream->write_bytes = (void (*)(void*, const unsigned char*, int))outputstream_write_bytes;
    stream->flush = (void (*)(void*))outputstream_flush;
    stream->close = (void (*)(void*))outputstream_close;
    
    return 0;
}

/**
 * Close an OutputStream
 * Base implementation - subclasses should override
 */
void outputstream_close(OutputStream* stream) {
    if (!stream) {
        return;
    }
    
    /* Flush any buffered data before closing */
    outputstream_flush(stream);
    
    stream->is_open = 0;
}

/**
 * Flush any buffered output
 * Base implementation does nothing - subclasses should override if needed
 */
void outputstream_flush(OutputStream* stream) {
    if (!stream || !stream->is_open) {
        return;
    }
    
    /* Base implementation: no buffering, so nothing to flush */
}

/**
 * Write a single byte to the stream
 * Base implementation returns error - subclasses must override
 */
int outputstream_write(OutputStream* stream, uint8_t byte) {
    if (!stream || !stream->is_open) {
        return -1;
    }
    
    /* This is a virtual function - subclasses must implement */
    fprintf(stderr, "Error: outputstream_write() not implemented by subclass\n");
    return -1;
}

/**
 * Write multiple bytes from a buffer
 * Base implementation uses write() - subclasses can override for efficiency
 */
int outputstream_write_bytes(OutputStream* stream, const uint8_t* buf, uint16_t len) {
    uint16_t i;
    int result;
    
    if (!stream || !buf || len == 0) {
        return -1;
    }
    
    if (!stream->is_open) {
        return -1;
    }
    
    /* Write bytes one at a time using write() */
    for (i = 0; i < len; i++) {
        result = outputstream_write(stream, buf[i]);
        if (result == -1) {
            /* Error - return number of bytes written so far */
            return (int)i;
        }
    }
    
    return (int)len;
}

/**
 * Check if stream is open
 */
uint8_t outputstream_is_open(OutputStream* stream) {
    if (!stream) {
        return 0;
    }
    return stream->is_open;
}

/**
 * Get current position in stream
 */
uint16_t outputstream_get_position(OutputStream* stream) {
    if (!stream) {
        return 0;
    }
    return stream->position;
}

// Made with Bob
