#ifndef _utils_h
#define _utils_h

#include "common.h"

uint8_t *read_entire_binary_file(char *filename);
void free_binary_file_contents(uint8_t *contents);

#endif /* _utils_h */
