#ifndef __MIM_EDITOR_H__
#define __MIM_EDITOR_H__

#include <stdio.h>

typedef struct MimEditor {
    char* file_name;
    FILE* file_ptr;
    char* text_buffer;
    int cursor_x;
    int cursor_y;
} MimEditor;

void editor_load_file(MimEditor* editor, char* file_name);

#endif  // __MIM_EDITOR_H__