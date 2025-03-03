#include "error.h"
#include <stdio.h>

const char *lexer_error_to_string(LexerErrorCode code) {
  switch (code) {
  case LEXER_ERROR_INVALID_CHAR:
    return "Invalid character";
  case LEXER_ERROR_UNTERMINATED_NUMBER:
    return "Unterminated number literal";
  case LEXER_ERROR_INVALID_NUMBER:
    return "Invalid number literal";
  case LEXER_ERROR_INVALID_IDENTIFIER:
    return "Invalid identifier";
  case LEXER_ERROR_FILE_ERROR:
    return "File error";
  case LEXER_ERROR_FILE_READ:
    return "Failed to read from file";
  case LEXER_ERROR_UNEXPECTED_EOF:
    return "Unexpected end of file";
  case LEXER_ERROR_MEMORY_ALLOCATION:
    return "Memory allocation failure";
  case LEXER_ERROR_NUMBER_OVERFLOW:
    return "Number literal too large";
  case LEXER_ERROR_BUFFER_OVERFLOW:
    return "Lexer buffer overflow";
  default:
    return "Unknown error";
  }
}

void lexer_format_error(const LexerError *error, char *buffer,
                        size_t buffer_size) {
  snprintf(buffer, buffer_size, "%s at %s:%d:%d - %s",
           lexer_error_to_string(error->code), error->span.filename,
           error->span.line, error->span.col, error->message);
}
