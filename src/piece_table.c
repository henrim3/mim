#include "piece_table.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "logging.h"

#define READ_CHUNK_SIZE 64

int piece_table_load_file(PieceTable *pt, char *filename) {
    // open file
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        LOG_PERROR("fopen() error while opening file");
        return -1;
    }

    // get file size
    if (fseek(f, 0L, SEEK_END) == -1) {
        LOG_PERROR("fseek() error while getting file length");
        return -1;
    }
    long file_length = ftell(f);

    pt->original_buffer = malloc(file_length * sizeof(char) + 1);

    if (pt->original_buffer == NULL) {
        LOG_PERROR("malloc() error while loading file");
        return -1;
    }

    pt->original_buffer_size = file_length;
    pt->original_buffer[file_length] = '\0';

    // back to beginning to read
    rewind(f);
    fread(pt->original_buffer, sizeof(char), file_length, f);

    if (fclose(f) == EOF) {
        LOG_PERROR("fclose() error");
        return -1;
    }

    assert(pt->pieces_head == NULL && "pieces should be empty on load");

    // create pieces head
    pt->pieces_head = calloc(1, sizeof(Piece));
    if (pt->pieces_head == NULL) {
        LOG_PERROR("calloc() error while allocating first piece");
    }

    Piece *head = pt->pieces_head;
    head->start = 0;
    head->length = file_length;
    head->source = PT_ORIGINAL_BUFFER;

    pt->pieces_length = 1;

    return 0;
}