#include "string_manipulator.h"

static void resize_(String *string, const size_t stringSize);

void initialize(String *string)
{
    string->buffer = NULL;
    string->stringSize = 0;
    string->bufferSize = 0;
}

void destroy(String *string)
{
    free(string->buffer);
    string->buffer = NULL;
}

void set(String *string, const char *input)
{
    free(string->buffer);
    size_t newStringSize = strlen(input);
    size_t newBufferSize = newStringSize + 1;
    string->buffer = malloc(newBufferSize);
    if (string->buffer == NULL)
    {
        print("Error. Could not set string. Memory problem\n");
        return;
    }
    memcpy(string->buffer, input, newStringSize + 1);
    string->stringSize = newStringSize;
    string->bufferSize = newBufferSize;
}

String concatenate(const String first, const String second)
{
    // Create a new string and reserve enough memory
    String output;
    initialize(&output);
    resize_(&output, first.stringSize + second.stringSize);

    // Copy both strings to the output buffer
    memcpy(output.buffer, first.buffer, first.stringSize);
    memcpy(output.buffer + first.stringSize, second.stringSize, second.stringSize);
    output.buffer[first.stringSize + second.stringSize] = '\0';
    return output;
}

static void resize_(String *string, const size_t stringSize)
{
    // If original buffer was empty
    size_t bufferSize = stringSize + 1;
    if (string->buffer == NULL)
    {
        string->buffer = malloc(bufferSize);
    }
    // If there was a string in the original buffer
    else
    {
        string->buffer = realloc(string->buffer, bufferSize);
    }
    // Check again after memory allocation
    if (string->buffer == NULL)
    {
        printf("Error: could not resize string. Not enough memory\n");
        return;
    }
    string->stringSize = stringSize;
    string->bufferSize = bufferSize;
}
