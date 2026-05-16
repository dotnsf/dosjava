#include "bufferedreader.h"
#include "../vm/memory.h"
#include <string.h>

/* Default buffer size for line reading */
#define DEFAULT_BUFFER_SIZE 256

/* Constructor */
BufferedReader* bufferedreader_new(FileInputStream* stream, int buffer_size) {
    BufferedReader* reader;
    
    if (!stream) {
        return NULL;
    }
    
    /* Allocate reader structure */
    reader = (BufferedReader*)memory_alloc(sizeof(BufferedReader));
    if (!reader) {
        return NULL;
    }
    
    /* Set buffer size */
    if (buffer_size <= 0) {
        buffer_size = DEFAULT_BUFFER_SIZE;
    }
    
    /* Allocate line buffer */
    reader->buffer = (char*)memory_alloc(buffer_size);
    if (!reader->buffer) {
        memory_free(reader);
        return NULL;
    }
    
    /* Initialize fields */
    reader->input_stream = stream;
    reader->buffer_size = buffer_size;
    reader->buffer_pos = 0;
    reader->buffer_len = 0;
    reader->eof_reached = 0;
    
    return reader;
}

/* Destructor */
void bufferedreader_delete(BufferedReader* reader) {
    if (!reader) {
        return;
    }
    
    /* Free buffer */
    if (reader->buffer) {
        memory_free(reader->buffer);
    }
    
    /* Note: We don't close or free the input stream - caller owns it */
    
    /* Free reader structure */
    memory_free(reader);
}

/* Read a single character */
int bufferedreader_read(BufferedReader* reader) {
    if (!reader || !reader->input_stream) {
        return -1;
    }
    
    if (reader->eof_reached) {
        return -1;
    }
    
    /* Read from underlying stream */
    return fileinputstream_read(reader->input_stream);
}

/* Read a line (up to newline or EOF) */
char* bufferedreader_read_line(BufferedReader* reader) {
    int ch;
    int pos = 0;
    
    if (!reader || !reader->buffer || !reader->input_stream) {
        return NULL;
    }
    
    if (reader->eof_reached) {
        return NULL;
    }
    
    /* Read characters until newline or EOF */
    while (pos < reader->buffer_size - 1) {
        ch = fileinputstream_read(reader->input_stream);
        
        if (ch == -1) {
            /* EOF reached */
            reader->eof_reached = 1;
            break;
        }
        
        if (ch == '\n') {
            /* End of line - don't include newline in result */
            break;
        }
        
        if (ch == '\r') {
            /* Carriage return - check for CR-LF pair */
            int next_ch = fileinputstream_read(reader->input_stream);
            if (next_ch != '\n' && next_ch != -1) {
                /* Not a CR-LF pair, put back the character */
                /* Note: We can't easily put back, so just skip CR */
            }
            break;
        }
        
        /* Add character to buffer */
        reader->buffer[pos++] = (char)ch;
    }
    
    /* Null-terminate the string */
    reader->buffer[pos] = '\0';
    
    /* Return NULL if no characters were read and EOF reached */
    if (pos == 0 && reader->eof_reached) {
        return NULL;
    }
    
    return reader->buffer;
}

/* Close the reader */
void bufferedreader_close(BufferedReader* reader) {
    if (!reader) {
        return;
    }
    
    /* Note: We don't close the underlying stream - caller owns it */
    reader->eof_reached = 1;
}
