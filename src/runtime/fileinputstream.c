#include <fcntl.h>
#include <io.h>
#include "fileinputstream.h"
#include "../vm/memory.h"
#include <string.h>
#include <dos.h>

/* DOS file access modes */
#define DOS_OPEN_READ 0x00

/* DOS error codes */
#define DOS_ERROR_FILE_NOT_FOUND 0x02
#define DOS_ERROR_ACCESS_DENIED 0x05

/* Internal helper: Fill buffer from file */
static int fill_buffer(FileInputStream* stream) {
    union REGS regs;
    struct SREGS sregs;
    
    if (!stream || stream->handle < 0) {
        return -1;
    }
    
    /* Read from file using INT 21h, AH=3Fh */
    regs.h.ah = 0x3F;
    regs.w.bx = stream->handle;
    regs.w.cx = sizeof(stream->buffer);
    regs.w.dx = FP_OFF(stream->buffer);
    segread(&sregs);
    sregs.ds = FP_SEG(stream->buffer);
    int86x(0x21, &regs, &regs, &sregs);
    
    /* Check for error (CF flag set) */
    if (regs.w.cflag) {
        return -1;
    }
    
    /* AX contains number of bytes read */
    stream->buffer_size = regs.w.ax;
    stream->buffer_pos = 0;
    
    return stream->buffer_size;
}

/* Internal helper: Get file size using seek */
static long get_file_size_internal(int handle) {
    union REGS regs;
    long size;
    
    /* Seek to end of file: INT 21h, AH=42h, AL=02h */
    regs.h.ah = 0x42;
    regs.h.al = 0x02;  /* Seek from end */
    regs.w.bx = handle;
    regs.w.cx = 0;     /* Offset high word */
    regs.w.dx = 0;     /* Offset low word */
    int86(0x21, &regs, &regs);
    
    if (regs.w.cflag) {
        return -1;
    }
    
    /* DX:AX contains file position (size) */
    size = ((long)regs.w.dx << 16) | regs.w.ax;
    
    /* Seek back to beginning: INT 21h, AH=42h, AL=00h */
    regs.h.ah = 0x42;
    regs.h.al = 0x00;  /* Seek from beginning */
    regs.w.bx = handle;
    regs.w.cx = 0;
    regs.w.dx = 0;
    int86(0x21, &regs, &regs);
    
    return size;
}

/* Constructor */
FileInputStream* fileinputstream_new(const char* filename) {
    FileInputStream* stream;
    
    if (!filename) {
        return NULL;
    }
    
    /* Allocate memory */
    stream = (FileInputStream*)memory_alloc(sizeof(FileInputStream));
    if (!stream) {
        return NULL;
    }
    
    /* Initialize base InputStream */
    inputstream_init((InputStream*)stream);
    
    /* Set up virtual function table */
    stream->base.read = (int (*)(void*))fileinputstream_read;
    stream->base.read_bytes = (int (*)(void*, unsigned char*, int))fileinputstream_read_bytes;
    stream->base.available = (int (*)(void*))fileinputstream_available;
    stream->base.skip = (long (*)(void*, long))fileinputstream_skip;
    stream->base.close = (void (*)(void*))fileinputstream_close;
    
    /* Initialize FileInputStream fields */
    stream->handle = -1;
    stream->buffer_pos = 0;
    stream->buffer_size = 0;
    stream->file_pos = 0;
    stream->file_size = 0;
    memset(stream->filename, 0, sizeof(stream->filename));
    memset(stream->buffer, 0, sizeof(stream->buffer));
    
    /* Open the file */
    if (fileinputstream_open(stream, filename) != 0) {
        memory_free(stream);
        return NULL;
    }
    
    return stream;
}

/* Destructor */
void fileinputstream_delete(FileInputStream* stream) {
    if (!stream) {
        return;
    }
    
    /* Close file if open */
    if (stream->handle >= 0) {
        fileinputstream_close(stream);
    }
    
    /* Free memory */
    memory_free(stream);
}

/* Open file */
int fileinputstream_open(FileInputStream* stream, const char* filename) {
    union REGS regs;
    struct SREGS sregs;
    int len;
    
    if (!stream || !filename) {
        return -1;
    }
    
    /* Close if already open */
    if (stream->handle >= 0) {
        fileinputstream_close(stream);
    }
    
    /* Copy filename (max 12 chars for 8.3 format) */
    len = strlen(filename);
    if (len > 12) {
        len = 12;
    }
    strncpy(stream->filename, filename, len);
    stream->filename[len] = '\0';
    
    /* Open file using INT 21h, AH=3Dh */
    regs.h.ah = 0x3D;
    regs.h.al = DOS_OPEN_READ;
    regs.w.dx = FP_OFF(stream->filename);  /* Offset of filename */
    segread(&sregs);                        /* Get current segment registers */
    sregs.ds = FP_SEG(stream->filename);   /* Segment of filename */
    int86x(0x21, &regs, &regs, &sregs);
    
    /* Check for error (CF flag set) */
    if (regs.w.cflag) {
        stream->handle = -1;
        return -1;
    }
    
    /* AX contains file handle */
    stream->handle = regs.w.ax;
    
    /* Set binary mode using DOS IOCTL to prevent CR-LF translation */
    /* INT 21h, AH=44h (IOCTL), AL=00h (Get Device Information) */
    regs.h.ah = 0x44;
    regs.h.al = 0x00;
    regs.w.bx = stream->handle;
    int86(0x21, &regs, &regs);
    
    /* Set bit 5 (binary mode) in device information word */
    /* INT 21h, AH=44h (IOCTL), AL=01h (Set Device Information) */
    regs.h.ah = 0x44;
    regs.h.al = 0x01;
    regs.w.bx = stream->handle;
    regs.w.dx |= 0x0020;  /* Set bit 5 for binary mode */
    int86(0x21, &regs, &regs);
    
    stream->base.is_open = 1;
    stream->base.position = 0;
    stream->file_pos = 0;
    stream->buffer_pos = 0;
    stream->buffer_size = 0;
    
    /* Get file size */
    stream->file_size = get_file_size_internal(stream->handle);
    
    return 0;
}

/* Close file */
void fileinputstream_close(FileInputStream* stream) {
    union REGS regs;
    
    if (!stream || stream->handle < 0) {
        return;
    }
    
    /* Close file using INT 21h, AH=3Eh */
    regs.h.ah = 0x3E;
    regs.w.bx = stream->handle;
    int86(0x21, &regs, &regs);
    
    stream->handle = -1;
    stream->base.is_open = 0;
    stream->buffer_pos = 0;
    stream->buffer_size = 0;
}

/* Read single byte */
int fileinputstream_read(FileInputStream* stream) {
    if (!stream || stream->handle < 0) {
        return -1;
    }
    
    /* Check if we need to fill buffer */
    if (stream->buffer_pos >= stream->buffer_size) {
        if (fill_buffer(stream) <= 0) {
            return -1;  /* EOF or error */
        }
    }
    
    /* Return byte from buffer */
    stream->base.position++;
    stream->file_pos++;
    return stream->buffer[stream->buffer_pos++];
}

/* Read multiple bytes */
int fileinputstream_read_bytes(FileInputStream* stream, unsigned char* buffer, int len) {
    int total_read = 0;
    int to_copy;
    
    if (!stream || !buffer || len <= 0 || stream->handle < 0) {
        return -1;
    }
    
    while (total_read < len) {
        /* Check if we need to fill buffer */
        if (stream->buffer_pos >= stream->buffer_size) {
            if (fill_buffer(stream) <= 0) {
                break;  /* EOF or error */
            }
        }
        
        /* Copy from buffer */
        to_copy = stream->buffer_size - stream->buffer_pos;
        if (to_copy > len - total_read) {
            to_copy = len - total_read;
        }
        
        memcpy(buffer + total_read, stream->buffer + stream->buffer_pos, to_copy);
        stream->buffer_pos += to_copy;
        total_read += to_copy;
        stream->base.position += to_copy;
        stream->file_pos += to_copy;
    }
    
    return total_read;
}

/* Get available bytes */
int fileinputstream_available(FileInputStream* stream) {
    if (!stream || stream->handle < 0) {
        return 0;
    }
    
    /* Calculate available bytes: total file size - bytes already read by user */
    return (int)(stream->file_size - stream->file_pos);
}

/* Skip bytes */
long fileinputstream_skip(FileInputStream* stream, long n) {
    union REGS regs;
    long skipped;
    int buffered;
    
    if (!stream || n <= 0 || stream->handle < 0) {
        return 0;
    }
    
    /* Try to skip within buffer first */
    buffered = stream->buffer_size - stream->buffer_pos;
    if (n <= buffered) {
        stream->buffer_pos += (int)n;
        stream->base.position += n;
        stream->file_pos += n;
        return n;
    }
    
    /* Skip beyond buffer - use seek */
    skipped = buffered;
    n -= buffered;
    
    /* Seek forward: INT 21h, AH=42h, AL=01h */
    regs.h.ah = 0x42;
    regs.h.al = 0x01;  /* Seek from current position */
    regs.w.bx = stream->handle;
    regs.w.cx = (unsigned int)(n >> 16);  /* High word */
    regs.w.dx = (unsigned int)(n & 0xFFFF);  /* Low word */
    int86(0x21, &regs, &regs);
    
    if (regs.w.cflag) {
        return skipped;  /* Return what we skipped in buffer */
    }
    
    /* Invalidate buffer */
    stream->buffer_pos = 0;
    stream->buffer_size = 0;
    stream->base.position += n;
    stream->file_pos += n;
    
    return skipped + n;
}

/* Check if at end of file */
int fileinputstream_is_eof(FileInputStream* stream) {
    if (!stream || stream->handle < 0) {
        return 1;
    }
    
    return (stream->file_pos >= stream->file_size) && 
           (stream->buffer_pos >= stream->buffer_size);
}

/* Get file size */
long fileinputstream_get_file_size(FileInputStream* stream) {
    if (!stream) {
        return -1;
    }
    
    return stream->file_size;
}
