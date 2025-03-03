#ifndef LEXER_ERROR_H
#define LEXER_ERROR_H

#include "span.h"
#include <stdlib.h>

typedef enum {
  LEXER_ERROR_INVALID_CHAR,
  LEXER_ERROR_UNTERMINATED_NUMBER,
  LEXER_ERROR_INVALID_IDENTIFIER,
  LEXER_ERROR_FILE_ERROR,
  LEXER_ERROR_FILE_READ,
  LEXER_ERROR_UNEXPECTED_EOF,
  LEXER_ERROR_MEMORY_ALLOCATION,
  LEXER_ERROR_BUFFER_OVERFLOW,
  LEXER_ERROR_INVALID_NUMBER,
  LEXER_ERROR_NUMBER_OVERFLOW
} LexerErrorCode;

typedef struct {
  LexerErrorCode code;
  char message[256];
  Span span;
} LexerError;

const char *lexer_error_to_string(LexerErrorCode code);
void lexer_format_error(const LexerError *error, char *buffer,
                        size_t buffer_size);

#endif