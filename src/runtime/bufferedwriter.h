#ifndef BUFFEREDWRITER_H
#define BUFFEREDWRITER_H

#include "fileoutputstream.h"

/* BufferedWriter structure */
typedef struct {
    FileOutputStream* output_stream;  /* Underlying output stream */
    char* buffer;                     /* Line buffer */
    int buffer_size;                  /* Size of line buffer */
    int buffer_pos;                   /* Current position in buffer */
} BufferedWriter;

/* Constructor/Destructor */
BufferedWriter* bufferedwriter_new(FileOutputStream* stream, int buffer_size);
void bufferedwriter_delete(BufferedWriter* writer);

/* Write operations */
void bufferedwriter_write(BufferedWriter* writer, int ch);
void bufferedwriter_write_string(BufferedWriter* writer, const char* str);
void bufferedwriter_write_line(BufferedWriter* writer, const char* str);
void bufferedwriter_new_line(BufferedWriter* writer);
void bufferedwriter_flush(BufferedWriter* writer);
void bufferedwriter_close(BufferedWriter* writer);

#endif /* BUFFEREDWRITER_H */
