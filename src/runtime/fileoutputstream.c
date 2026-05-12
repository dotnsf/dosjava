#include "fileoutputstream.h"
#include "../vm/memory.h"
#include <string.h>
#include <dos.h>

/* DOS file access modes */
#define DOS_CREATE_WRITE 0x3C  /* Create or truncate file */
#define DOS_OPEN_WRITE 0x01    /* Open for writing */

/* Internal helper: Flush buffer to file */
static int flush_buffer(FileOutputStream* stream) {
    union REGS regs;
    struct SREGS sregs;
    
    if (!stream || stream->handle < 0 || stream->buffer_pos == 0) {
        return 0;
    }
    
    /* Write to file using INT 21h, AH=40h */
    regs.h.ah = 0x40;
    regs.w.bx = stream->handle;
    regs.w.cx = stream->buffer_pos;
    regs.w.dx = FP_OFF(stream->buffer);
    segread(&sregs);
    sregs.ds = FP_SEG(stream->buffer);
    int86x(0x21, &regs, &regs, &sregs);
    
    /* Check for error (CF flag set) */
    if (regs.w.cflag) {
        return -1;
    }
    
    /* AX contains number of bytes written */
    if (regs.w.ax != stream->buffer_pos) {
        return -1;  /* Partial write */
    }
    
    stream->file_pos += stream->buffer_pos;
    stream->buffer_pos = 0;
    
    return 0;
}

/* Constructor */
FileOutputStream* fileoutputstream_new(const char* filename) {
    FileOutputStream* stream;
    
    if (!filename) {
        return NULL;
    }
    
    /* Allocate memory */
    stream = (FileOutputStream*)memory_alloc(sizeof(FileOutputStream));
    if (!stream) {
        return NULL;
    }
    
    /* Initialize base OutputStream */
    outputstream_init((OutputStream*)stream);
    
    /* Set up virtual function table */
    stream->base.write = (void (*)(void*, int))fileoutputstream_write;
    stream->base.write_bytes = (void (*)(void*, const unsigned char*, int))fileoutputstream_write_bytes;
    stream->base.flush = (void (*)(void*))fileoutputstream_flush;
    stream->base.close = (void (*)(void*))fileoutputstream_close;
    
    /* Initialize FileOutputStream fields */
    stream->handle = -1;
    stream->buffer_pos = 0;
    stream->file_pos = 0;
    memset(stream->filename, 0, sizeof(stream->filename));
    memset(stream->buffer, 0, sizeof(stream->buffer));
    
    /* Open the file */
    if (fileoutputstream_open(stream, filename) != 0) {
        memory_free(stream);
        return NULL;
    }
    
    return stream;
}

/* Destructor */
void fileoutputstream_delete(FileOutputStream* stream) {
    if (!stream) {
        return;
    }
    
    /* Flush and close file if open */
    if (stream->handle >= 0) {
        fileoutputstream_flush(stream);
        fileoutputstream_close(stream);
    }
    
    /* Free memory */
    memory_free(stream);
}

/* Open file */
int fileoutputstream_open(FileOutputStream* stream, const char* filename) {
    union REGS regs;
    struct SREGS sregs;
    int len;
    
    if (!stream || !filename) {
        return -1;
    }
    
    /* Close if already open */
    if (stream->handle >= 0) {
        fileoutputstream_flush(stream);
        fileoutputstream_close(stream);
    }
    
    /* Copy filename (max 12 chars for 8.3 format) */
    len = strlen(filename);
    if (len > 12) {
        len = 12;
    }
    strncpy(stream->filename, filename, len);
    stream->filename[len] = '\0';
    
    /* Create/truncate file using INT 21h, AH=3Ch */
    regs.h.ah = 0x3C;
    regs.w.cx = 0;  /* Normal file attribute */
    regs.w.dx = FP_OFF(stream->filename);
    segread(&sregs);
    sregs.ds = FP_SEG(stream->filename);
    int86x(0x21, &regs, &regs, &sregs);
    
    /* Check for error (CF flag set) */
    if (regs.w.cflag) {
        stream->handle = -1;
        return -1;
    }
    
    /* AX contains file handle */
    stream->handle = regs.w.ax;
    stream->base.is_open = 1;
    stream->file_pos = 0;
    stream->buffer_pos = 0;
    
    return 0;
}

/* Close file */
void fileoutputstream_close(FileOutputStream* stream) {
    union REGS regs;
    
    if (!stream || stream->handle < 0) {
        return;
    }
    
    /* Flush any remaining data */
    flush_buffer(stream);
    
    /* Close file using INT 21h, AH=3Eh */
    regs.h.ah = 0x3E;
    regs.w.bx = stream->handle;
    int86(0x21, &regs, &regs);
    
    stream->handle = -1;
    stream->base.is_open = 0;
    stream->buffer_pos = 0;
}

/* Write single byte */
void fileoutputstream_write(FileOutputStream* stream, int byte) {
    if (!stream || stream->handle < 0) {
        return;
    }
    
    /* Add byte to buffer */
    stream->buffer[stream->buffer_pos++] = (unsigned char)byte;
    
    /* Flush if buffer is full */
    if (stream->buffer_pos >= sizeof(stream->buffer)) {
        flush_buffer(stream);
    }
}

/* Write multiple bytes */
void fileoutputstream_write_bytes(FileOutputStream* stream, const unsigned char* data, int len) {
    int i;
    int to_copy;
    int space_left;
    
    if (!stream || !data || len <= 0 || stream->handle < 0) {
        return;
    }
    
    i = 0;
    while (i < len) {
        /* Calculate space left in buffer */
        space_left = sizeof(stream->buffer) - stream->buffer_pos;
        
        /* Calculate how many bytes to copy */
        to_copy = (len - i < space_left) ? (len - i) : space_left;
        
        /* Copy to buffer */
        memcpy(stream->buffer + stream->buffer_pos, data + i, to_copy);
        stream->buffer_pos += to_copy;
        i += to_copy;
        
        /* Flush if buffer is full */
        if (stream->buffer_pos >= sizeof(stream->buffer)) {
            flush_buffer(stream);
        }
    }
}

/* Flush buffer */
void fileoutputstream_flush(FileOutputStream* stream) {
    if (!stream || stream->handle < 0) {
        return;
    }
    
    flush_buffer(stream);
}

// Made with Bob
