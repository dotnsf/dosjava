#ifndef FILEINPUTSTREAM_H
#define FILEINPUTSTREAM_H

#include "inputstream.h"

/* FileInputStream - File input stream for DOS
 * 
 * Reads data from files using DOS INT 21h file operations.
 * Supports 8.3 filename format and DOS path conventions.
 * 
 * DOS File Operations:
 * - INT 21h, AH=3Dh: Open file
 * - INT 21h, AH=3Fh: Read from file
 * - INT 21h, AH=3Eh: Close file
 * - INT 21h, AH=42h: Seek in file
 * 
 * Memory Model: Small (64KB code, 64KB data)
 * Buffer Size: 512 bytes (DOS sector size)
 */

/* FileInputStream structure */
typedef struct FileInputStream {
    InputStream base;           /* Base InputStream */
    int handle;                 /* DOS file handle (-1 if closed) */
    char filename[13];          /* 8.3 filename + null terminator */
    unsigned char buffer[512];  /* Read buffer (DOS sector size) */
    int buffer_pos;             /* Current position in buffer */
    int buffer_size;            /* Valid bytes in buffer */
    long file_pos;              /* Current file position */
    long file_size;             /* Total file size */
} FileInputStream;

/* Constructor/Destructor */
FileInputStream* fileinputstream_new(const char* filename);
void fileinputstream_delete(FileInputStream* stream);

/* File operations */
int fileinputstream_open(FileInputStream* stream, const char* filename);
void fileinputstream_close(FileInputStream* stream);

/* Stream operations (override base class) */
int fileinputstream_read(FileInputStream* stream);
int fileinputstream_read_bytes(FileInputStream* stream, unsigned char* buffer, int len);
int fileinputstream_available(FileInputStream* stream);
long fileinputstream_skip(FileInputStream* stream, long n);

/* Helper functions */
int fileinputstream_is_eof(FileInputStream* stream);
long fileinputstream_get_file_size(FileInputStream* stream);

#endif /* FILEINPUTSTREAM_H */
