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

int PieceTable_init( PieceTable* pt );
void PieceTable_free( PieceTable* pt );
int PieceTable_load_file( PieceTable* pt, const char* file_name );
int PieceTable_insert( PieceTable* pt, char* text, int64_t global_pos );
//
// returns position in add buffer, -1 if error
int64_t PieceTable_append_to_add_buffer( PieceTable* pt, char* src,
                                         size_t src_len );
PT_Piece* PieceTable_find_piece_by_global_pos( PieceTable* pt, int64_t pos );
int PieceTable_output_final( PieceTable* pt );
int PieceTable_read_piece( PieceTable* pt, PT_Piece* p, char* buf );
void PieceTable_dump_piece( PieceTable* pt, PT_Piece* p );
void PieceTable_dump_pieces( PieceTable* pt );
int PT_Piece_shift_pieces_behind( PT_Piece* p, int64_t shift );

#endif //  __PIECE_TABLE_H__