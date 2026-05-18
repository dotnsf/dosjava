#include "inputstream.h"
#include <stdio.h>
#include <string.h>

/**
 * Initialize an InputStream
 */
int inputstream_init(InputStream* stream) {
    if (!stream) {
        return -1;
    }
    
    /* Initialize base object */
    stream->base.class_id = 0;  /* Will be set by subclass */
    stream->base.ref_count = 1;
    
    /* Initialize stream state */
    stream->is_open = 1;
    stream->position = 0;
    
    /* Initialize virtual function pointers to base implementations */
    stream->read = (int (*)(void*))inputstream_read;
    stream->read_bytes = (int (*)(void*, unsigned char*, int))inputstream_read_bytes;
    stream->available = (int (*)(void*))inputstream_available;
    stream->skip = (long (*)(void*, long))inputstream_skip;
    stream->close = (void (*)(void*))inputstream_close;
    
    return 0;
}

/**
 * Close an InputStream
 * Base implementation - subclasses should override
 */
void inputstream_close(InputStream* stream) {
    if (!stream) {
        return;
    }
    
    stream->is_open = 0;
}

/**
 * Read a single byte from the stream
 * Base implementation returns error - subclasses must override
 */
int inputstream_read(InputStream* stream) {
    if (!stream || !stream->is_open) {
        return -1;
    }
    
    /* This is a virtual function - subclasses must implement */
    fprintf(stderr, "Error: inputstream_read() not implemented by subclass\n");
    return -1;
}

/**
 * Read multiple bytes into a buffer
 * Base implementation uses read() - subclasses can override for efficiency
 */
int inputstream_read_bytes(InputStream* stream, uint8_t* buf, uint16_t len) {
    uint16_t i;
    int byte;
    
    if (!stream || !buf || len == 0) {
        return -1;
    }
    
    if (!stream->is_open) {
        return -1;
    }
    
    /* Read bytes one at a time using read() */
    for (i = 0; i < len; i++) {
        byte = inputstream_read(stream);
        if (byte == -1) {
            /* EOF or error - return number of bytes read so far */
            return (int)i;
        }
        buf[i] = (uint8_t)byte;
    }
    
    return (int)len;
}

/**
 * Get number of bytes available for reading
 * Base implementation returns 0 - subclasses should override
 */
int inputstream_available(InputStream* stream) {
    if (!stream || !stream->is_open) {
        return -1;
    }
    
    /* This is a virtual function - subclasses should implement */
    return 0;
}

/**
 * Skip over and discard n bytes of data
 * Base implementation uses read() - subclasses can override for efficiency
 */
int inputstream_skip(InputStream* stream, uint16_t n) {
    uint16_t i;
    int byte;
    
    if (!stream || !stream->is_open) {
        return -1;
    }
    
    /* Skip by reading and discarding bytes */
    for (i = 0; i < n; i++) {
        byte = inputstream_read(stream);
        if (byte == -1) {
            /* EOF or error - return number of bytes skipped */
            return (int)i;
        }
    }
    
    return (int)n;
}

/**
 * Check if stream is open
 */
uint8_t inputstream_is_open(InputStream* stream) {
    if (!stream) {
        return 0;
    }
    return stream->is_open;
}

/**
 * Get current position in stream
 */
uint16_t inputstream_get_position(InputStream* stream) {
    if (!stream) {
        return 0;
    }
    return stream->position;
}
