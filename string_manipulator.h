#include <stdlib.h>

typedef struct String_st
{
    char *buffer;
    size_t stringSize;
    size_t bufferSize;
} String;

void initialize(String *string);

void set(String *string, const char *input);

void destroy(String *string);

String concatenate(const String first, const String second);