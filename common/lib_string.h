#ifndef _LIB_STRING_H
#define _LIB_STRING_H

#include <common.h>

#define CHAR_LF			 10
#define CHAR_CR			 13
#define CHAR_ESC		 27
#define CHAR_BACKSPACE	 8
#define CHAR_TAB		 9
#define CHAR_SPACE		 32

#define CHAR_UP			 65
#define CHAR_DOWN		 66
#define CHAR_RIGHT		 67
#define CHAR_LEFT		 68
#define COMMAND_STACK_SIZE 8

int16u_t tokenize(int8s_t **token, int8s_t *string);
void init_lib_string();
void int2str(int8s_t *str, int32s_t value, int32s_t radix);
int32u_t str2int(int8s_t *str, int32u_t radix);
void com_stack_push(int8s_t* command);
void com_stack_pop(int8s_t* Dest, int8s_t KeyID);

#endif
