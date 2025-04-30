#include "piece_table.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "logging.h"

#define READ_CHUNK_SIZE 64

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

  // back to beginning to read
  rewind( f );
  fread( pt->original_buffer, sizeof( char ), file_length, f );

  if ( fclose( f ) == EOF ) {
    LOG_PERROR( "fclose() error" );
    return MIM_FAILURE;
  }

  assert( pt->pieces_head == NULL && "pieces should be empty on load" );

  // create pieces head
  pt->pieces_head = calloc( 1, sizeof( PT_Piece ) );
  if ( pt->pieces_head == NULL ) {
    LOG_PERROR( "calloc() error while allocating first piece" );
  }

  PT_Piece* head = pt->pieces_head;
  head->start = 0;
  head->length = file_length;
  head->source = PT_ORIGINAL_BUFFER;

  pt->pieces_length = 1;

  return MIM_SUCCESS;
}

int PieceTable_output( PieceTable* pt ) {
  PT_Piece* curr = pt->pieces_head;

  if ( curr == NULL ) {
    LOG_ERROR( "pieces head is null" );
    return MIM_FAILURE;
  }

  while ( curr != 0 ) {
    char* buf = malloc( sizeof( char ) );
    if ( buf == NULL ) {
      LOG_PERROR( "malloc()" );
    }

    curr = curr->next;
  }

  printf( "\n" );

  return MIM_SUCCESS;
}

int PieceTable_read_piece( PieceTable* pt, PT_Piece* p, char* buf ) {
  char* read_from_buf;

  if ( p->source == PT_ORIGINAL_BUFFER ) {
    read_from_buf = pt->original_buffer;
  } else if ( p->source == PT_ADD_BUFFER ) {
    read_from_buf = pt->add_buffer;
  } else {
    LOG_ERROR( "Piece has invalid source" );
    fprintf( stderr, "source: %d\n", p->source );
    return MIM_FAILURE;
  }

  memcpy( buf, read_from_buf, p->length );
  return MIM_SUCCESS;
}