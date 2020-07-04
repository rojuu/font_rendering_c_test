#include "utils.h"

uint8_t *read_entire_binary_file(char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Attempted to open file '%s' but it doesn't exist\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);

    uint8_t *buffer;
    buffer = malloc(size * sizeof(*buffer));

    rewind(file);
    fread(buffer, 1, size, file);

    return buffer;
}

void free_binary_file_contents(uint8_t *contents) { free(contents); }
