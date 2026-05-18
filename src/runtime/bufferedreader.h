#ifndef BUFFEREDREADER_H
#define BUFFEREDREADER_H

#include "fileinputstream.h"

/* BufferedReader structure */
typedef struct {
    FileInputStream* input_stream;  /* Underlying input stream */
    char* buffer;                   /* Line buffer */
    int buffer_size;                /* Size of line buffer */
    int buffer_pos;                 /* Current position in buffer */
    int buffer_len;                 /* Length of data in buffer */
    int eof_reached;                /* EOF flag */
} BufferedReader;

/* Constructor/Destructor */
BufferedReader* bufferedreader_new(FileInputStream* stream, int buffer_size);
void bufferedreader_delete(BufferedReader* reader);

/* Read operations */
char* bufferedreader_read_line(BufferedReader* reader);
int bufferedreader_read(BufferedReader* reader);
void bufferedreader_close(BufferedReader* reader);

#endif /* BUFFEREDREADER_H */
