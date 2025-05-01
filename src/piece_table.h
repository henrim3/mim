#ifndef __PIECE_TABLE_H__
#define __PIECE_TABLE_H__

#include <stdint.h>

typedef enum {
  PT_ORIGINAL_BUFFER,
  PT_ADD_BUFFER,
} PieceTableBufferType;

typedef struct PT_Piece_struct {
  int64_t global_pos;
  int64_t buf_pos;
  int64_t length;
  PieceTableBufferType src_buf;
  struct PT_Piece_struct* prev;
  struct PT_Piece_struct* next;
} PT_Piece;

typedef struct {
  char* original_buffer;
  char* add_buffer;
  int64_t original_buffer_size;
  int64_t add_buffer_size;
  int64_t add_buffer_length;
  int64_t total_length;
  PT_Piece* pieces_head;
  PT_Piece* pieces_tail;
  size_t pieces_length;
} PieceTable;

void PieceTable_free( PieceTable* pt );
int PieceTable_load_file( PieceTable* pt, char* filename );
int PieceTable_insert( PieceTable* pt, int64_t pos, char* src );
int PieceTable_output( PieceTable* pt );
int PieceTable_read_piece( PieceTable* pt, PT_Piece* p, char* buf );
void PieceTable_output_pieces( PieceTable* pt );
PT_Piece* PieceTable_find_piece_by_global_pos( PieceTable* pt, int64_t pos );

void PT_Piece_output( PT_Piece* p );

#endif //  __PIECE_TABLE_H__