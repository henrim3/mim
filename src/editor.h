#ifndef __MIM_EDITOR_H__
#define __MIM_EDITOR_H__

#include "piece_table.h"
#include <stdio.h>

typedef struct {
  char* file_name;
  FILE* file_ptr;
  PieceTable* piece_table;
  int cursor_x;
  int cursor_y;
} Editor;

int Editor_load_file( Editor* editor, char* file_name );

#endif // __MIM_EDITOR_H__