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

int PieceTable_load_file( PieceTable* pt, char* filename ) {
  // open file
  FILE* f = fopen( filename, "r" );
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

  pt->pieces_length = 1;
  pt->pieces_tail = head;

  return MIM_SUCCESS;
}

int PieceTable_insert( PieceTable* pt, char* text, int64_t global_pos ) {
  LOG_MESSAGE( "inserting %s at %d", text, global_pos );

  // check valid position
  if ( global_pos < 0 || global_pos > pt->total_length ) {
    LOG_ERROR( "tried to insert at invalid position: %d, should be >= 0, <= %d",
               global_pos, pt->total_length );
    return MIM_FAILURE;
  }

  size_t src_len = strlen( text );

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

    assert( pt->pieces_length == 0 &&
            "pieces length should be 0 if head is null" );
    pt->pieces_length = 1;
    pt->total_length += src_len;

    return MIM_SUCCESS;
  }

  // put in front of head
  if ( global_pos == 0 ) {
    pt->pieces_head->prev = new_piece;

    new_piece->next = pt->pieces_head;
    new_piece->prev = NULL;

    pt->pieces_head = new_piece;

    if ( PT_Piece_shift_pieces_behind( new_piece, new_piece->length ) ==
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

    pt->pieces_length++;
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

    if ( PT_Piece_shift_pieces_behind( new_piece, new_piece->length ) ==
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

  pt->pieces_length++;
  pt->total_length += src_len;

  if ( PT_Piece_shift_pieces_behind( new_piece, new_piece->length ) ==
       MIM_FAILURE ) {
    LOG_ERROR( "error while shifting pieces back" );
    return MIM_FAILURE;
  }

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

PT_Piece* PieceTable_find_piece_by_global_pos( PieceTable* pt, int64_t pos ) {
  if ( pos < 0 || pos > pt->total_length ) {
    LOG_ERROR(
        "tried to find piece at invalid position: %d, should be >= 0, <= %d",
        pos, pt->total_length );
    return NULL;
  }

  if ( pt->pieces_head == NULL ) {
    LOG_ERROR( "tried to find piece but pieces head is null" );
    return NULL;
  }

  PT_Piece* curr = pt->pieces_head;
  while ( pos > curr->global_pos + curr->length ) {
    curr = curr->next;
    if ( curr == NULL ) {
      LOG_ERROR( "couldn't find piece for pos: %d", pos );
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

int PT_Piece_shift_pieces_behind( PT_Piece* p, int64_t shift ) {
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