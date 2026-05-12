#include "bufferedwriter.h"
#include "../vm/memory.h"
#include <string.h>
#include <stdio.h>

/* Default buffer size for line writing */
#define DEFAULT_BUFFER_SIZE 256

/* Constructor */
BufferedWriter* bufferedwriter_new(FileOutputStream* stream, int buffer_size) {
    BufferedWriter* writer;
    
    if (!stream) {
        return NULL;
    }
    
    /* Allocate writer structure */
    writer = (BufferedWriter*)memory_alloc(sizeof(BufferedWriter));
    if (!writer) {
        return NULL;
    }
    
    /* Set buffer size */
    if (buffer_size <= 0) {
        buffer_size = DEFAULT_BUFFER_SIZE;
    }
    
    /* Allocate line buffer */
    writer->buffer = (char*)memory_alloc(buffer_size);
    if (!writer->buffer) {
        memory_free(writer);
        return NULL;
    }
    
    /* Initialize fields */
    writer->output_stream = stream;
    writer->buffer_size = buffer_size;
    writer->buffer_pos = 0;
    
    return writer;
}

/* Destructor */
void bufferedwriter_delete(BufferedWriter* writer) {
    if (!writer) {
        return;
    }
    
    /* Flush any remaining data */
    bufferedwriter_flush(writer);
    
    /* Free buffer */
    if (writer->buffer) {
        memory_free(writer->buffer);
    }
    
    /* Note: We don't close or free the output stream - caller owns it */
    
    /* Free writer structure */
    memory_free(writer);
}

/* Write a single character */
void bufferedwriter_write(BufferedWriter* writer, int ch) {
    if (!writer || !writer->buffer || !writer->output_stream) {
        return;
    }
    
    /* Add character to buffer */
    writer->buffer[writer->buffer_pos++] = (char)ch;
    
    /* Flush if buffer is full */
    if (writer->buffer_pos >= writer->buffer_size) {
        bufferedwriter_flush(writer);
    }
}

/* Write a string */
void bufferedwriter_write_string(BufferedWriter* writer, const char* str) {
    int i;
    int len;
    
    if (!writer || !str) {
        return;
    }
    
    len = strlen(str);
    for (i = 0; i < len; i++) {
        bufferedwriter_write(writer, str[i]);
    }
}

/* Write a line (string + newline) */
void bufferedwriter_write_line(BufferedWriter* writer, const char* str) {
    if (!writer) {
        return;
    }
    
    if (str) {
        bufferedwriter_write_string(writer, str);
    }
    bufferedwriter_new_line(writer);
}

/* Write a newline */
void bufferedwriter_new_line(BufferedWriter* writer) {
    if (!writer) {
        return;
    }
    
    /* Write DOS-style line ending (CR-LF) */
    bufferedwriter_write(writer, '\r');
    bufferedwriter_write(writer, '\n');
}

/* Flush the buffer */
void bufferedwriter_flush(BufferedWriter* writer) {
    if (!writer || !writer->buffer || !writer->output_stream) {
        return;
    }
    
    if (writer->buffer_pos == 0) {
        return;  /* Nothing to flush */
    }
    
    /* Write buffer to underlying stream */
    fileoutputstream_write_bytes(writer->output_stream,
                                  (const unsigned char*)writer->buffer,
                                  writer->buffer_pos);
    
    /* Flush the underlying FileOutputStream to disk */
    fileoutputstream_flush(writer->output_stream);
    
    /* Reset buffer position */
    writer->buffer_pos = 0;
}

/* Close the writer */
void bufferedwriter_close(BufferedWriter* writer) {
    if (!writer) {
        return;
    }
    
    /* Flush any remaining data */
    bufferedwriter_flush(writer);
    
    /* Note: We don't close the underlying stream - caller owns it */
}

// Made with Bob
