#include "editor.h"

#include "common.h"
#include "logging.h"
#include "piece_table.h"
#include <stdio.h>

int Editor_load_file( Editor* editor, char* file_name ) {
  if ( PieceTable_load_file( editor->piece_table, file_name ) == MIM_FAILURE ) {
    LOG_ERROR( "error while loading file into editor" );
    return MIM_FAILURE;
  }
}