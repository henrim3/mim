#ifndef __MIM_EDITOR_H__
#define __MIM_EDITOR_H__

#include <stdio.h>

typedef struct {
  char* file_name;
  FILE* file_ptr;
  char* text_buffer;
  int cursor_x;
  int cursor_y;
} Editor;

void Editor_load_file( Editor* editor, char* file_name );

#endif // __MIM_EDITOR_H__