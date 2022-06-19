#include "string_manipulator.h"

#define INITIAL_MEMORY 30000
#define BLOCK_BYTES_STRING

char *createGeneralTemplate()
{
    String codeText;
    set(&codeText, "@buffer = private unnamed_addr [30000 x i8]");
}