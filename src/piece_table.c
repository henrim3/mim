#include "piece_table.h"

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "logging.h"

#define INITIAL_ADD_BUFFER_SIZE 256

PieceTable* PieceTable_new() {
  PieceTable* pt = calloc( 1, sizeof( PieceTable ) );
  if ( pt == NULL ) {
    LOG_ERROR( "calloc() while allocating new piece table" );
    return NULL;
  }
  pt->cursor_global_pos = -1;
  return pt;
}

void PieceTable_free( PieceTable* pt ) {
  free( pt->original_buffer );
  free( pt->add_buffer );

  PT_Piece* curr = pt->pieces_head;

  while ( curr != NULL ) {
    PT_Piece* next = curr->next;
    free( curr );
    curr = next;
  }
}

int PieceTable_load_file( PieceTable* pt, const char* file_name ) {
  // open file
  FILE* f = fopen( file_name, "r" );
  if ( f == NULL ) {
    LOG_PERROR( "fopen() error while opening file" );
    return MIM_FAILURE;
  }

  // get file size
  if ( fseek( f, 0L, SEEK_END ) == -1 ) {
    LOG_PERROR( "fseek() error while getting file length" );
    return MIM_FAILURE;
  }
  long file_length = ftell( f );

  pt->original_buffer = malloc( file_length * sizeof( char ) + 1 );

  if ( pt->original_buffer == NULL ) {
    LOG_PERROR( "malloc() error while loading file" );
    return MIM_FAILURE;
  }

  pt->original_buffer_size = file_length;
  pt->original_buffer[file_length] = '\0';
  pt->total_length = file_length;

  // back to beginning to read
  rewind( f );
  fread( pt->original_buffer, sizeof( char ), file_length, f );

  if ( fclose( f ) == EOF ) {
    LOG_PERROR( "fclose() error" );
    return MIM_FAILURE;
  }

  assert( pt->pieces_head == NULL && "pieces should be empty on load" );
  assert( pt->pieces_tail == NULL && "pieces should be empty on load" );

  // create pieces head
  pt->pieces_head = malloc( sizeof( PT_Piece ) );
  if ( pt->pieces_head == NULL ) {
    LOG_PERROR( "malloc() error while allocating first piece" );
  }

  PT_Piece* head = pt->pieces_head;
  head->global_pos = 0;
  head->buf_pos = 0;
  head->length = file_length;
  head->src_buf = PT_ORIGINAL_BUFFER;
  head->prev = NULL;
  head->next = NULL;

  pt->pieces_tail = head;

  return MIM_SUCCESS;
}

int PieceTable_insert_char( PieceTable* pt, char c, int64_t global_pos ) {
  // keep a "cursor" of current inserting to avoid keep making pieces
  // then you can move the cursor when the user moves their cursor as well
  // but for now check if the global_pos is at the current cursor or else move
  // the cursor

  if ( global_pos < 0 || global_pos > pt->total_length ) {
    LOG_ERROR(
        "tried to insert at invalid position: %d, should be >= 0 and < %d",
        global_pos, pt->total_length );
    return MIM_FAILURE;
  }

  int64_t buf_pos = pt->add_buffer_length;
  if ( PieceTable_append_char_to_add_buffer( pt, c ) == MIM_FAILURE ) {
    LOG_ERROR( "error while appending char to add buffer" );
    return MIM_FAILURE;
  }

  // check if pieces is empty (no file loaded)
  if ( pt->pieces_head == NULL ) {
    LOG_MESSAGE( "no pieces head, creating first piece" );
    assert( pt->pieces_tail == NULL && "tail should be null if head is null" );

    PT_Piece* new_head = PT_Piece_new();
    if ( new_head == NULL ) {
      LOG_ERROR( "error while creating first piece" );
      return MIM_FAILURE;
    }

    // both head and tail since first piece
    pt->pieces_head = new_head;
    pt->pieces_tail = new_head;

    new_head->global_pos = global_pos;
    new_head->buf_pos = 0;
    new_head->length = 1;
    new_head->src_buf = PT_ADD_BUFFER;

    // save pos and piece for next time
    pt->cursor_piece = new_head;
    pt->cursor_global_pos = 1;
    pt->total_length++;

    return MIM_SUCCESS;
  }

  // see if modifying at same place as last time
  if ( global_pos == pt->cursor_global_pos ) {
    LOG_MESSAGE( "inserting after last" );

    pt->cursor_piece->length++;
    pt->cursor_global_pos++;
    pt->total_length++;

    PT_Piece_shift_pieces_after( pt->cursor_piece, 1 );

    return MIM_SUCCESS;
  }

  // move "cursor"
  pt->cursor_global_pos = global_pos + 1;

  PT_Piece* insert_at;

  // see if modifying in same piece
  if ( pt->cursor_piece != NULL &&
       PT_Piece_global_pos_in_piece( pt->cursor_piece, global_pos ) ) {
    LOG_MESSAGE( "inserting in same piece as last time" );
    insert_at = pt->cursor_piece;
  } else {
    // look for piece
    LOG_MESSAGE( "looking for piece for global pos: %d", global_pos );
    insert_at = PieceTable_find_piece_by_global_pos( pt, global_pos );
    if ( insert_at == NULL ) {
      LOG_ERROR( "error while finding piece to insert at" );
      return MIM_FAILURE;
    }
  }

  PT_Piece* new_piece = PT_Piece_new();
  if ( new_piece == NULL ) {
    LOG_ERROR( "couldn't create new piece" );
    return MIM_FAILURE;
  }

  new_piece->global_pos = global_pos;
  new_piece->buf_pos = buf_pos;
  new_piece->length = 1;
  new_piece->src_buf = PT_ADD_BUFFER;

  pt->cursor_piece = new_piece;

  // still need to set prev and next

  // check if inserting between pieces (inserting at start of a piece)
  if ( global_pos == insert_at->global_pos ) {
    LOG_MESSAGE( "inserting between two pieces" );

    if ( insert_at->prev != NULL ) {
      insert_at->prev->next = new_piece;
    }

    new_piece->prev = insert_at->prev;
    new_piece->next = insert_at;

    insert_at->prev = new_piece;

    if ( insert_at == pt->pieces_head ) {
      pt->pieces_head = new_piece;
    }

    // move all pieces after new piece back
    if ( PT_Piece_shift_pieces_after( new_piece, 1 ) == MIM_FAILURE ) {
      LOG_ERROR( "couldn't shift pieces back" );
      return MIM_FAILURE;
    }

    pt->total_length++;

    return MIM_SUCCESS;
  }
  // need to split a piece
  PT_Piece* second_half = PT_Piece_new();
  if ( second_half == NULL ) {
    LOG_ERROR( "couldn't create second half piece" );
    return MIM_FAILURE;
  }

  int64_t global_pos_diff = global_pos - insert_at->global_pos;

  second_half->global_pos = global_pos + 1;
  second_half->buf_pos = insert_at->buf_pos + global_pos_diff;
  second_half->src_buf = insert_at->src_buf;

  // set prev, next
  new_piece->prev = insert_at;
  new_piece->next = second_half;

  second_half->next = insert_at->next;
  second_half->prev = new_piece;

  if ( insert_at->next != NULL ) {
    insert_at->next->prev = second_half;
  }
  insert_at->next = new_piece;

  if ( insert_at == pt->pieces_tail ) {
    pt->pieces_tail = second_half;
  }

  // adjust lengths
  second_half->length = insert_at->length - global_pos_diff;
  insert_at->length = global_pos_diff;

  // move pieces after second half back
  if ( PT_Piece_shift_pieces_after( second_half, 1 ) == MIM_FAILURE ) {
    LOG_ERROR( "couldn't shift pieces back" );
    return MIM_FAILURE;
  }

  pt->total_length++;
  return MIM_SUCCESS;
}

int PieceTable_insert_str( PieceTable* pt, char* text, int64_t global_pos ) {
  LOG_MESSAGE( "inserting %s at %d", text, global_pos );

  // check valid position
  if ( global_pos < 0 || global_pos > pt->total_length ) {
    LOG_ERROR( "tried to insert at invalid position: %d, should be >= 0, <= %d",
               global_pos, pt->total_length );
    return MIM_FAILURE;
  }

  size_t src_len = strlen( text );

  if ( src_len == 0 ) {
    return MIM_SUCCESS;
  }

  PT_Piece* new_piece = malloc( sizeof( PT_Piece ) );
  if ( new_piece == NULL ) {
    LOG_PERROR( "malloc() error while allocating new piece" );
  }

  new_piece->global_pos = global_pos;
  new_piece->length = src_len;
  new_piece->src_buf = PT_ADD_BUFFER;

  new_piece->buf_pos = PieceTable_append_to_add_buffer( pt, text, src_len );
  if ( new_piece->buf_pos == -1 ) {
    LOG_ERROR( "error appending to add buffer" );
    return MIM_FAILURE;
  }

  // put as head if first piece
  if ( pt->pieces_head == NULL ) {
    pt->pieces_head = new_piece;
    pt->pieces_tail = new_piece;

    new_piece->next = NULL;
    new_piece->prev = NULL;

    pt->total_length += src_len;

    return MIM_SUCCESS;
  }

  // put in front of head
  if ( global_pos == 0 ) {
    pt->pieces_head->prev = new_piece;

    new_piece->next = pt->pieces_head;
    new_piece->prev = NULL;

    pt->pieces_head = new_piece;

    if ( PT_Piece_shift_pieces_after( new_piece, new_piece->length ) ==
         MIM_FAILURE ) {
      LOG_ERROR( "error while shifting pieces back" );
      return MIM_FAILURE;
    }

    return MIM_SUCCESS;
  }

  // put as tail if goes after current tail
  if ( global_pos == pt->total_length ) {
    assert( pt->pieces_tail != NULL && "pieces tail should be null" );
    pt->pieces_tail->next = new_piece;
    new_piece->prev = pt->pieces_tail;
    pt->pieces_tail = new_piece;
    new_piece->next = NULL;

    pt->total_length += src_len;

    return MIM_SUCCESS;
  }

  // find piece to insert at
  PT_Piece* insert_piece =
      PieceTable_find_piece_by_global_pos( pt, global_pos );
  if ( insert_piece == NULL ) {
    LOG_ERROR( "couldn't find piece to insert at" );
    return MIM_FAILURE;
  }

  // check if inserting at gap between pieces
  if ( insert_piece->global_pos == global_pos ) {
    LOG_MESSAGE( "HERE" );
    if ( insert_piece->prev != NULL ) {
      insert_piece->prev->next = new_piece;
    }

    new_piece->prev = insert_piece->prev;
    new_piece->next = insert_piece;

    insert_piece->next = new_piece;
    insert_piece->global_pos += src_len;

    if ( PT_Piece_shift_pieces_after( new_piece, new_piece->length ) ==
         MIM_FAILURE ) {
      LOG_ERROR( "error while shifting pieces back" );
      return MIM_FAILURE;
    }

    return MIM_SUCCESS;
  }

  PT_Piece* second_half_piece = malloc( sizeof( PT_Piece ) );
  if ( second_half_piece == NULL ) {
    LOG_PERROR( "malloc() while allocating second half of split piece" );
    return MIM_FAILURE;
  }

  // split into two pieces and insert between
  second_half_piece->src_buf = insert_piece->src_buf;
  second_half_piece->prev = new_piece;
  second_half_piece->next = insert_piece->next;

  new_piece->prev = insert_piece;
  insert_piece->next = new_piece;

  new_piece->next = second_half_piece;

  int64_t original_insert_piece_length = insert_piece->length;
  insert_piece->length = global_pos - insert_piece->global_pos;
  second_half_piece->buf_pos = insert_piece->buf_pos + insert_piece->length;
  second_half_piece->length =
      original_insert_piece_length - insert_piece->length;

  if ( pt->pieces_tail == insert_piece ) {
    pt->pieces_tail = second_half_piece;
  }

  pt->total_length += src_len;

  if ( PT_Piece_shift_pieces_after( new_piece, new_piece->length ) ==
       MIM_FAILURE ) {
    LOG_ERROR( "error while shifting pieces back" );
    return MIM_FAILURE;
  }

  return MIM_SUCCESS;
}

int PieceTable_append_char_to_add_buffer( PieceTable* pt, char c ) {
  // check if need to alloc add buffer
  if ( pt->add_buffer_size == 0 ) {
    assert( pt->add_buffer_length == 0 &&
            "add buffer length should be 0 if add buffer size is 0" );
    assert( pt->add_buffer == NULL &&
            "add buffer should be null if add buffer length is null" );

    pt->add_buffer = malloc( INITIAL_ADD_BUFFER_SIZE * sizeof( char ) );
    if ( pt->add_buffer == NULL ) {
      LOG_PERROR( "malloc() during initial alloc for add buffer" );
      return MIM_FAILURE;
    }

    pt->add_buffer_size = INITIAL_ADD_BUFFER_SIZE;
  }

  // check if need to grow
  // + 2 for null terminator
  if ( pt->add_buffer_size < pt->add_buffer_length + 2 ) {
    pt->add_buffer_size *= 2;
    pt->add_buffer =
        realloc( pt->add_buffer, pt->add_buffer_size * sizeof( char ) );
  }

  pt->add_buffer[pt->add_buffer_length] = c;
  pt->add_buffer_length++;

  return MIM_SUCCESS;
}

int64_t PieceTable_append_to_add_buffer( PieceTable* pt, char* src,
                                         size_t src_len ) {
  // grow add buffer if necessary
  int64_t new_length = pt->add_buffer_length + src_len;
  int64_t old_length = pt->add_buffer_length;

  if ( pt->add_buffer_size == 0 ) {
    assert( pt->add_buffer_length == 0 &&
            "add buffer length should be 0 if add buffer size is 0" );
    pt->add_buffer = malloc( INITIAL_ADD_BUFFER_SIZE * sizeof( char ) );

    if ( pt->add_buffer == NULL ) {
      LOG_PERROR( "malloc() during initial alloc for add buffer" );
      return -1;
    }

    pt->add_buffer_size = INITIAL_ADD_BUFFER_SIZE;
  }

  // note: +1 because null terminator
  while ( pt->add_buffer_size < new_length + 1 ) {
    pt->add_buffer_size *= 2;
    pt->add_buffer =
        realloc( pt->add_buffer, pt->add_buffer_size * sizeof( char ) );

    if ( pt->add_buffer == NULL ) {
      LOG_PERROR( "realloc() while growing add buffer" );
      return -1;
    }
  }

  // add text to end of add buffer
  strcpy( pt->add_buffer + pt->add_buffer_length, src );
  assert( pt->add_buffer[new_length] == '\0' &&
          "strcpy should put null terminator" );

  pt->add_buffer_length = new_length;

  return old_length;
}

PT_Piece* PieceTable_find_piece_by_global_pos( PieceTable* pt,
                                               int64_t global_pos ) {
  if ( global_pos < 0 || global_pos > pt->total_length ) {
    LOG_ERROR(
        "tried to find piece at invalid position: %d, should be >= 0, <= %d",
        global_pos, pt->total_length );
    return NULL;
  }

  if ( pt->pieces_head == NULL ) {
    LOG_ERROR( "tried to find piece but pieces head is null" );
    return NULL;
  }

  PT_Piece* curr = pt->pieces_head;
  while ( !PT_Piece_global_pos_in_piece( curr, global_pos ) ) {
    curr = curr->next;
    if ( curr == NULL ) {
      LOG_ERROR( "couldn't find piece for global_pos: %d", global_pos );
      return NULL;
    }
  }

  return curr;
}

int PieceTable_output_final( PieceTable* pt ) {
  PT_Piece* curr = pt->pieces_head;

  if ( curr == NULL ) {
    LOG_ERROR( "pieces head is null" );
    return MIM_FAILURE;
  }

  while ( curr != NULL ) {
    char* buf = malloc( curr->length * sizeof( char ) + 1 );
    if ( buf == NULL ) {
      LOG_PERROR( "malloc()" );
    }

    if ( PieceTable_read_piece( pt, curr, buf ) == MIM_FAILURE ) {
      return MIM_FAILURE;
    }
    printf( "%s", buf );

    free( buf );

    curr = curr->next;
  }

  printf( "\n" );

  return MIM_SUCCESS;
}

int PieceTable_read_piece( PieceTable* pt, PT_Piece* p, char* buf ) {
  char* read_from_buf;

  if ( p->src_buf == PT_ORIGINAL_BUFFER ) {
    read_from_buf = pt->original_buffer;
  } else if ( p->src_buf == PT_ADD_BUFFER ) {
    read_from_buf = pt->add_buffer;
  } else {
    LOG_ERROR( "piece has invalid src_buf: %d", p->src_buf );
    return MIM_FAILURE;
  }

  memcpy( buf, read_from_buf + p->buf_pos, p->length );
  buf[p->length] = '\0';

  return MIM_SUCCESS;
}

void PieceTable_dump( PieceTable* pt ) {
  printf( "Piece Table at %p\n", pt );
  printf( "  original_buffer: %s\n", pt->original_buffer );
  printf( "  original_buffer_size: %d\n", pt->original_buffer_size );
  printf( "  add_buffer: %s\n", pt->add_buffer );
  printf( "  add_buffer_length: %d\n", pt->add_buffer_length );
  printf( "  add_buffer_size: %d\n", pt->add_buffer_size );
  printf( "  total_length: %d\n", pt->total_length );
  printf( "  cursor_global_pos: %d\n", pt->cursor_global_pos );
  printf( "  cursor_piece: %p\n", pt->cursor_piece );
  printf( "  pieces_head: %p\n", pt->pieces_head );
  printf( "  pieces_tail: %p\n", pt->pieces_tail );
  printf( "  final output: " );
  PieceTable_output_final( pt );
  printf( "\n" );
}

void PieceTable_dump_piece( PieceTable* pt, PT_Piece* p ) {
  printf( "Piece at %p:\n", p );
  printf( "  global_pos: %" PRId64 "\n", p->global_pos );
  printf( "  buf_pos: %" PRId64 "\n", p->buf_pos );
  printf( "  length: %" PRId64 "\n", p->length );

  if ( p->src_buf == PT_ORIGINAL_BUFFER ) {
    printf( "  src_buf: PT_ORIGINAL_BUFFER\n" );
  } else if ( p->src_buf == PT_ADD_BUFFER ) {
    printf( "  src_buf: PT_ADD_BUFFER\n" );
  } else {
    printf( "  src_buf: invalid\n" );
  }

  char* buf = malloc( p->length * sizeof( char ) );
  if ( PieceTable_read_piece( pt, p, buf ) == MIM_SUCCESS ) {
    printf( "  text: %s\n", buf );
  } else {
    printf( "  text: invalid\n" );
  }
  free( buf );

  if ( p->prev == NULL ) {
    printf( "  prev: NULL\n" );
  } else {
    printf( "  prev: %p\n", p->prev );
  }

  if ( p->next == NULL ) {
    printf( "  next: NULL\n" );
  } else {
    printf( "  next: %p\n", p->next );
  }
}

void PieceTable_dump_pieces( PieceTable* pt ) {
  PT_Piece* curr = pt->pieces_head;

  while ( curr != NULL ) {
    PieceTable_dump_piece( pt, curr );
    printf( "\n" );
    curr = curr->next;
  }
}

PT_Piece* PT_Piece_new() {
  PT_Piece* p = calloc( 1, sizeof( PT_Piece ) );
  if ( p == NULL ) {
    LOG_PERROR( "calloc() while allocating new piece" );
    return NULL;
  }
  return p;
}

int PT_Piece_shift_pieces_after( PT_Piece* p, int64_t shift ) {
  if ( p == NULL ) {
    LOG_ERROR( "tried to shift after null piece" );
    return MIM_FAILURE;
  }

  PT_Piece* curr = p->next;

  while ( curr != NULL ) {
    curr->global_pos += shift;
    curr = curr->next;
  }

  return MIM_SUCCESS;
}

int PT_Piece_global_pos_in_piece( PT_Piece* p, int64_t global_pos ) {
  return global_pos >= p->global_pos && global_pos < p->global_pos + p->length;
}