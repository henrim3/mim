#ifndef __PIECE_TABLE_H__
#define __PIECE_TABLE_H__

typedef enum {
    PT_ORIGINAL_BUFFER,
    PT_ADD_BUFFER,
} PieceTableBufferType;

typedef struct Piece {
    int start;
    int length;
    PieceTableBufferType source;
    struct Piece *prev;
    struct Piece *next;
} Piece;

typedef struct PieceTable {
    char *original_buffer;
    char *add_buffer;
    long original_buffer_size;
    long add_buffer_size;
    Piece *pieces_head;
} PieceTable;

int piece_table_load_file(PieceTable *pt, char *filename);

#endif  //  __PIECE_TABLE_H__