#ifndef __MIM_BUFFER_H__
#define __MIM_BUFFER_H__

#include <stdio.h>

typedef struct Buffer {
    char *name;
    FILE *file_ptr;
    char *file_buffer;
} Buffer;

void buffer_init(Buffer *buf);

#endif  // __MIM_BUFFER_H__