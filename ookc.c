#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESERVED_WORD_SIZE 4
#define BLOCK_BYTES 200
#define NUM_WORDS 3
#define NUM_OPCODES 8

// Possible words as strings
const char *WORD_LIST[NUM_WORDS] = {
    "Ook.", "Ook?", "Ook!"};

// Possible words as symbols
const unsigned char SYM_OOK_DOT = 0x0;
const unsigned char SYM_OOK_QM = 0x1;
const unsigned char SYM_OOK_EM = 0x2;
const unsigned char SYM_LIST[NUM_WORDS] = {
    SYM_OOK_DOT, SYM_OOK_QM, SYM_OOK_EM};

// Join two chars, place the first in the 4 bytes to the left, the second
// to the 4 bytes on the right
#define JOIN(val_1, val_2) ((val_1 << 4) | val_2)

// Join all pairs of reserved words into operators
// Ook. Ook?
static const unsigned char OP_INC_POINTER = JOIN(SYM_OOK_DOT, SYM_OOK_QM);
// Ook? Ook.
static const unsigned char OP_DEC_POINTER = JOIN(SYM_OOK_QM, SYM_OOK_DOT);
// Ook. Ook.
static const unsigned char OP_INC_BYTE = JOIN(SYM_OOK_DOT, SYM_OOK_DOT);
// Ook! Ook!
static const unsigned char OP_DEC_BYTE = JOIN(SYM_OOK_EM, SYM_OOK_EM);
// Ook! Ook.
static const unsigned char OP_OUT = JOIN(SYM_OOK_EM, SYM_OOK_DOT);
// Ook. Ook!
static const unsigned char OP_IN = JOIN(SYM_OOK_DOT, SYM_OOK_EM);
// Ook! Ook?
const unsigned char OP_BEGIN_LOOP = JOIN(SYM_OOK_EM, SYM_OOK_QM);
// Ook? Ook!
const unsigned char OP_END_LOOP = JOIN(SYM_OOK_QM, SYM_OOK_EM);

// Complete list of operators
const unsigned char OPCODE_LIST[NUM_OPCODES] = {
    OP_INC_POINTER,
    OP_DEC_POINTER,
    OP_INC_BYTE,
    OP_DEC_BYTE,
    OP_OUT,
    OP_IN,
    OP_BEGIN_LOOP,
    OP_END_LOOP};

const char *OPCODE_STRINGS[NUM_OPCODES] = {
    "Ook. Ook? : increment pointer",
    "Ook? Ook. : decrement pointer",
    "Ook. Ook. : increment pointed byte",
    "Ook! Ook! : decrement pointed byte",
    "Ook! Ook. : output byte as ASCII",
    "Ook. Ook! : input byte as ASCII",
    "Ook! Ook? : begin loop",
    "Ook? Ook! : end loop"};

unsigned char *parse(FILE *file, size_t *opCodesSize);
void printSymbols(const unsigned char *symbolList, const size_t symbolListSize);
void printOpcodes(const unsigned char *opcodes, const size_t opcodesSize);
unsigned char *getSymbolList(FILE *file, size_t *size);
unsigned char *getOpcodes(const unsigned char *symbolList, const size_t symbolListSize, size_t *opcodesSize);
int validWord(char *nextReservedWord);
const char *getSymbolString(const unsigned char symbol);
const char *getOpcodeString(const unsigned char opcode);

int main(int argc, char *argv[])
{
    // Check input arguments
    if (argc != 2)
    {
        printf("Usage error: file expected\n");
        return 1;
    }

    // Get the file to compile
    char *filename = argv[1];
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }

    // Parse the file into an abstract syntax tree
    size_t opcodesSize;
    unsigned char *opcodes = parse(file, &opcodesSize);
    if (opcodes == NULL)
    {
        printf("Could not parse file\n");
        return 1;
    }

    // Print the current program
    printOpcodes(opcodes, opcodesSize);

    return 0;
}

unsigned char *parse(FILE *file, size_t *opcodesSize)
{
    // Get a list of all the symbols present in the file
    size_t symbolListSize;
    unsigned char *symbolList = getSymbolList(file, &symbolListSize);
    if (symbolList == NULL)
    {
        printf("Could not parse reserved words from file\n");
        return NULL;
    }
    printSymbols(symbolList, symbolListSize);

    unsigned char *opcodes = getOpcodes(symbolList, symbolListSize, opcodesSize);
    if (opcodes == NULL)
    {
        printf("Error parsing symbol list into opcodes\n");
        return NULL;
    }
    free(symbolList);
    return opcodes;
}

unsigned char *getSymbolList(FILE *file, size_t *size)
{
    // Allocate initial space for the symbol list
    unsigned char *symbolList = malloc(BLOCK_BYTES);
    size_t symbolListSize = 0;
    size_t symbolListCapacity = BLOCK_BYTES;

    // Read the file char after char
    char currentChar;
    char nextReservedWord[RESERVED_WORD_SIZE + 1];
    unsigned char nextSymbol;
    while ((currentChar = fgetc(file)) != EOF)
    {
        // If this is the beginning of a reserved word, loop to try to get one of the complete words
        if (currentChar == 'O')
        {
            memset(nextReservedWord, 0x0, RESERVED_WORD_SIZE + 1);
            nextReservedWord[0] = currentChar;
            for (int i = 1; i < RESERVED_WORD_SIZE; i++)
            {
                nextReservedWord[i] = fgetc(file);
                if (!validWord(nextReservedWord))
                {
                    printf("Word not recognised: %s\n", nextReservedWord);
                    return NULL;
                }
            }

            // Get the symbol that represents the reserved word
            for (size_t i = 0; i < NUM_WORDS; i++)
            {
                if (strcmp(nextReservedWord, WORD_LIST[i]) == 0)
                {
                    nextSymbol = SYM_LIST[i];
                    break;
                }
            }

            // Add the new symbol to the list of symbols
            // If no space in the buffer, add more
            if (symbolListSize >= symbolListCapacity)
            {
                symbolList = realloc(symbolList, symbolListCapacity += BLOCK_BYTES);
                if (symbolList == NULL)
                {
                    printf("Could not reallocate symbol list\n");
                    return NULL;
                }
            }
            symbolList[symbolListSize] = nextSymbol;
            symbolListSize++;
        }

        // Ignore whitespace
        else if (!(currentChar == ' ' || currentChar == '\n' || currentChar == '\t'))
        {
            printf("Forbidden character: %c\n", currentChar);
            return NULL;
        }
    }
    *size = symbolListSize;
    return symbolList;
}

int validWord(char *nextReservedWord)
{
    for (size_t i = 0; i < NUM_WORDS; i++)
    {
        if (strstr(WORD_LIST[i], nextReservedWord) != NULL)
        {
            return 1;
        }
    }
    printf("not found\n");
    return 0;
}

unsigned char *getOpcodes(const unsigned char *symbolList, const size_t symbolListSize, size_t *opcodesSize)
{
    // Create a list of opcodes
    unsigned char *opcodes = malloc(BLOCK_BYTES);
    *opcodesSize = 0;
    size_t opcodesCapacity = BLOCK_BYTES;
    unsigned char possibleOpcode;
    for (size_t i = 0; i < symbolListSize - 1; i += 2)
    {
        // Join both symbols in one
        possibleOpcode = JOIN(symbolList[i], symbolList[i + 1]);
        int found = 0;
        for (size_t i = 0; i < NUM_OPCODES; i++)
        {
            if (possibleOpcode == OPCODE_LIST[i])
            {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            printf("This pair of symbols does not correspond to an opcode\n");
            return NULL;
        }

        // Check if I have space in the opcode list
        if (*opcodesSize == opcodesCapacity)
        {
            opcodes = realloc(opcodes, opcodesCapacity + BLOCK_BYTES);
            opcodesCapacity += BLOCK_BYTES;
            if (opcodes == NULL)
            {
                printf("Could not realloc opcodes list\n");
                return NULL;
            }
        }

        // Add opcode to the list
        opcodes[*opcodesSize] = possibleOpcode;
        (*opcodesSize)++;
    }
    return opcodes;
}

void printSymbols(const unsigned char *symbolList, const size_t symbolListSize)
{
    for (size_t i = 0; i < symbolListSize; i++)
    {
        printf("%s\n", getSymbolString(symbolList[i]));
    }
}

void printOpcodes(const unsigned char *opcodes, const size_t opcodesSize)
{
    for (size_t i = 0; i < opcodesSize; i++)
    {
        printf("%s\n", getOpcodeString(opcodes[i]));
    }
}

const char *getSymbolString(const unsigned char symbol)
{
    for (size_t i = 0; i < NUM_WORDS; i++)
    {
        if (SYM_LIST[i] == symbol)
        {
            return WORD_LIST[i];
        }
    }
    return NULL;
}

const char *getOpcodeString(const unsigned char opcode)
{
    for (size_t i = 0; i < NUM_OPCODES; i++)
    {
        if (OPCODE_LIST[i] == opcode)
        {
            return OPCODE_STRINGS[i];
        }
    }
    return NULL;
}