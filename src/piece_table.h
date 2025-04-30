#ifndef __PIECE_TABLE_H__
#define __PIECE_TABLE_H__

typedef enum {
  PT_ORIGINAL_BUFFER,
  PT_ADD_BUFFER,
} PieceTableBufferType;

typedef struct PT_Piece_struct {
  int start;
  int length;
  PieceTableBufferType source;
  struct PT_Piece_struct * prev;
  struct PT_Piece_struct * next;
} PT_Piece;

typedef struct {
  char * original_buffer;
  char * add_buffer;
  long original_buffer_size;
  long add_buffer_size;
  PT_Piece * pieces_head;
  int pieces_length;
} PieceTable;

int PieceTable_load_file( PieceTable * pt, char * filename );
int PieceTable_output( PieceTable * pt );
int PieceTable_read_piece( PieceTable * pt, Piece * p, char * buf );
void PieceTable_free( PieceTable * pt );

#endif //  __PIECE_TABLE_H__