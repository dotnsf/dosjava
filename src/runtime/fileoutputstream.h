#ifndef FILEOUTPUTSTREAM_H
#define FILEOUTPUTSTREAM_H

#include "outputstream.h"

/* FileOutputStream structure */
typedef struct {
    OutputStream base;          /* Base OutputStream */
    int handle;                 /* DOS file handle */
    char filename[13];          /* 8.3 filename + null terminator */
    unsigned char buffer[512];  /* Write buffer (DOS sector size) */
    int buffer_pos;             /* Current position in buffer */
    long file_pos;              /* Current position in file */
} FileOutputStream;

/* Constructor/Destructor */
FileOutputStream* fileoutputstream_new(const char* filename);
void fileoutputstream_delete(FileOutputStream* stream);

/* File operations */
int fileoutputstream_open(FileOutputStream* stream, const char* filename);
void fileoutputstream_close(FileOutputStream* stream);

/* Write operations */
void fileoutputstream_write(FileOutputStream* stream, int byte);
void fileoutputstream_write_bytes(FileOutputStream* stream, const unsigned char* data, int len);
void fileoutputstream_flush(FileOutputStream* stream);

#endif /* FILEOUTPUTSTREAM_H */

// Made with Bob
