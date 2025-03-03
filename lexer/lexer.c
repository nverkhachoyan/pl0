#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "lexer_buffer.h"
#include "span.h"
#include "token.h"

#define MAX_LEXER_ERRORS 100

static int grow_tok_stream(Lexer *);
static void reset_tok_stream(Lexer *);
static void lexer_add_error(Lexer *, LexerErrorCode, const char *,
                            const Span *);

struct Lexer {
  char *input;
  long file_size;
  int num_tokens;
  char ch;
  int pos;
  LexerBuffer *buf;
  Span span;
  LexerError errors[MAX_LEXER_ERRORS];
  size_t error_count;
};

static void advance(Lexer *lexer) {
  lexer->ch = lexer->input[lexer->pos++];
  span_advance(&lexer->span, lexer->ch);
}

static char peek(Lexer *lexer) {
  if (lexer->pos >= lexer->file_size)
    return '\0';
  return lexer->input[lexer->pos];
}

static void skip_whitespace(Lexer *lexer) {
  while (isspace(lexer->ch)) {
    advance(lexer);
    span_advance(&lexer->span, lexer->ch);
  }
}

static Token *read_ident(Lexer *lexer) {
  if (!lexer || !lexer->buf) {
    return NULL;
  }

  span_mark_start(&lexer->span);
  buffer_reset(lexer->buf);

  while (isalpha(lexer->ch) || isdigit(lexer->ch)) {
    if (buffer_append(lexer->buf, lexer->ch) != 0) {
      lexer_add_error(lexer, LEXER_ERROR_INVALID_CHAR,
                      "Failed to append character to identifier", &lexer->span);
      return NULL;
    }
    advance(lexer);
  }

  span_mark_end(&lexer->span);

  char *str = buffer_to_str(lexer->buf);
  if (!str) {
    perror("failed to convert buff stream to string\n");
    return NULL;
  }

  TokenType keyword_type = keyword_get_type(str);
  Token *tok = token_create(keyword_type, str, &lexer->span);
  if (!tok) {
    perror("failed to craete token\n");
    free(str);
    return NULL;
  }

  free(str);
  return tok;
}

static Token *read_number(Lexer *lexer) {
  if (!lexer || !lexer->buf) {
    return NULL;
  }

  span_mark_start(&lexer->span);
  buffer_reset(lexer->buf);

  while (isdigit(lexer->ch)) {
    if (buffer_append(lexer->buf, lexer->ch) != 0) {
      lexer_add_error(lexer, LEXER_ERROR_BUFFER_OVERFLOW,
                      "failed to append digit to buffer", &lexer->span);
      return NULL;
    }
    advance(lexer);
  }

  span_mark_end(&lexer->span);

  if (isalpha(lexer->ch)) {
    lexer_add_error(lexer, LEXER_ERROR_INVALID_NUMBER,
                    "Encountered an alpha char in number literal",
                    &lexer->span);
    return NULL;
  }

  char *str = buffer_to_str(lexer->buf);
  if (!str) {
    perror("failed to convert buff stream to string\n");
    return NULL;
  }

  Token *tok = token_create(TOKEN_NUMBER, str, &lexer->span);
  if (!tok) {
    perror("failed to create token\n");
    free(str);
    return NULL;
  }

  return tok;
}

static void lexer_add_error(Lexer *lexer, LexerErrorCode code,
                            const char *message, const Span *span) {
  if (!lexer || !message || !span || lexer->error_count >= MAX_LEXER_ERRORS)
    return;

  LexerError *error = &lexer->errors[lexer->error_count++];
  error->code = code;
  strncpy(error->message, message, sizeof(error->message) - 1);
  error->message[sizeof(error->message) - 1] = '\0';
  error->span = *span;
}

static void lexer_clear_errors(Lexer *lexer) { lexer->error_count = 0; }

static int _next_token(Lexer *lexer, Token **token_out) {
  if (lexer->ch == '\0')
    return 1;
  skip_whitespace(lexer);
  if (lexer->ch == '\0')
    return 1;

  span_mark_start(&lexer->span);

  if (!isprint(lexer->ch)) {
    lexer_add_error(lexer, LEXER_ERROR_INVALID_CHAR,
                    "Invalid non-printable character", &lexer->span);
    return 2;
  }

  if (lexer->ch == ':' && peek(lexer) == '=') {
    advance(lexer);
    advance(lexer);
    span_mark_end(&lexer->span);
    *token_out = token_create(TOKEN_ASSIGN, ":=", &lexer->span);
  } else if (lexer->ch == '<' && peek(lexer) == '=') {
    advance(lexer);
    advance(lexer);
    span_mark_end(&lexer->span);
    *token_out = token_create(TOKEN_LTE, "<=", &lexer->span);
  } else if (lexer->ch == '>' && peek(lexer) == '=') {
    advance(lexer);
    advance(lexer);
    span_mark_end(&lexer->span);
    *token_out = token_create(TOKEN_GTE, ">=", &lexer->span);
  } else {
    switch (lexer->ch) {
    case ';':
      advance(lexer);
      span_mark_end(&lexer->span);
      *token_out = token_create(TOKEN_SEMICOLON, ";", &lexer->span);
      break;
    case '-':
      advance(lexer);
      span_mark_end(&lexer->span);
      *token_out = token_create(TOKEN_MINUS, "-", &lexer->span);
      break;
    case '+':
      advance(lexer);
      span_mark_end(&lexer->span);
      *token_out = token_create(TOKEN_PLUS, "+", &lexer->span);
      break;
    case '=':
      advance(lexer);
      span_mark_end(&lexer->span);
      *token_out = token_create(TOKEN_EQUAL, "=", &lexer->span);
      break;
    case ',':
      advance(lexer);
      span_mark_end(&lexer->span);
      *token_out = token_create(TOKEN_COMMA, ",", &lexer->span);
      break;
    case '.':
      advance(lexer);
      span_mark_end(&lexer->span);
      *token_out = token_create(TOKEN_PERIOD, ".", &lexer->span);
      break;
    case '*':
      advance(lexer);
      span_mark_end(&lexer->span);
      *token_out = token_create(TOKEN_TIMES, "*", &lexer->span);
      break;
    case '/':
      advance(lexer);
      span_mark_end(&lexer->span);
      *token_out = token_create(TOKEN_DIVIDE, "/", &lexer->span);
      break;
    case '#':
      advance(lexer);
      span_mark_end(&lexer->span);
      *token_out = token_create(TOKEN_NOT_EQUAL, "#", &lexer->span);
      break;
    case '<':
      advance(lexer);
      span_mark_end(&lexer->span);
      *token_out = token_create(TOKEN_LT, "<", &lexer->span);
      break;
    case '>':
      advance(lexer);
      span_mark_end(&lexer->span);
      *token_out = token_create(TOKEN_GT, ">", &lexer->span);
      break;
    default:
      if (isdigit(lexer->ch)) {
        Token *tok = read_number(lexer);
        if (!tok) {
          lexer_add_error(lexer, LEXER_ERROR_UNTERMINATED_NUMBER,
                          "Failed to read number literal", &lexer->span);
          return 2;
        }
        *token_out = tok;
      } else if (isalpha(lexer->ch)) {
        Token *tok = read_ident(lexer);
        if (!tok) {
          lexer_add_error(lexer, LEXER_ERROR_INVALID_IDENTIFIER,
                          "Failed to read identifier/keyword", &lexer->span);
          return 2;
        }
        *token_out = tok;
      } else {
        char error_msg[100];
        if (lexer->ch == '\0') {
          snprintf(error_msg, sizeof(error_msg),
                   "Error: Unexpected end of file at %s:%d:%d\n",
                   lexer->span.filename, lexer->span.line, lexer->span.col);
          lexer_add_error(lexer, LEXER_ERROR_UNEXPECTED_EOF, error_msg,
                          &lexer->span);
          return 2;
        }
        snprintf(error_msg, sizeof(error_msg),
                 "Unexpected character '%c' (ASCII: %d)", lexer->ch,
                 (int)lexer->ch);
        lexer_add_error(lexer, LEXER_ERROR_INVALID_CHAR, error_msg,
                        &lexer->span);
        advance(lexer);
        return 2;
      }
    }
  }

  if (*token_out) {
    lexer->num_tokens++;
    return 0;
  }

  return -1;
}

// ***** PUBLIC API Functions *****

int lexer_next_token(Lexer *lexer, Token **token_out) {
  if (!lexer || !token_out)
    return -1;

  *token_out = NULL;
  Token *temp_token = NULL;
  int result;

  do {
    result = _next_token(lexer, &temp_token);

    if (result < 0) {
      token_free(temp_token);
      return result;
    }

    if (result == 1) {
      return result;
    }

    if (result == 0) {
      *token_out = temp_token;
      return 0;
    }

    token_free(temp_token);
    temp_token = NULL;
  } while (result == 2);

  return result;
}

Lexer *lexer_create(const char *filename) {
  Lexer *lexer;

  lexer = calloc(1, sizeof(Lexer));
  if (!lexer) {
    perror("Error allocating memory for lexer");
    return NULL;
  }
  span_init(&lexer->span, filename);

  LexerBuffer *buf = buffer_alloc(32);

  if (!buf) {
    lexer_add_error(lexer, LEXER_ERROR_MEMORY_ALLOCATION,
                    "Failed to allocate lexer buffer", &lexer->span);
    free(lexer);
    return NULL;
  }
  lexer->buf = buf;

  FILE *fptr = fopen(filename, "rb");
  if (!fptr) {
    lexer_add_error(lexer, LEXER_ERROR_FILE_READ, "Failed to open source file",
                    &lexer->span);
    buffer_free(lexer->buf);
    free(lexer);
    return NULL;
  }

  fseek(fptr, 0, SEEK_END);
  lexer->file_size = ftell(fptr);
  rewind(fptr);

  lexer->input = (char *)malloc(lexer->file_size + 1);
  if (lexer->input == NULL) {
    lexer_add_error(lexer, LEXER_ERROR_MEMORY_ALLOCATION,
                    "Failed to allocate input file buffer", &lexer->span);
    fclose(fptr);
    buffer_free(lexer->buf);
    free(lexer);
    return NULL;
  }

  size_t bytes_read = fread(lexer->input, 1, lexer->file_size, fptr);
  if (bytes_read != (size_t)lexer->file_size) {
    lexer_add_error(lexer, LEXER_ERROR_FILE_READ, "Failed to read from file",
                    &lexer->span);
    free(lexer->input);
    buffer_free(lexer->buf);
    fclose(fptr);
    free(lexer);
    return NULL;
  }

  lexer->input[lexer->file_size] = '\0';
  advance(lexer);
  fclose(fptr);
  return lexer;
}

void lexer_free(Lexer *lexer) {
  if (!lexer)
    return;

  free(lexer->input);
  buffer_free(lexer->buf);
  free(lexer);
}

int lexer_num_tokens(Lexer *lexer) { return lexer->num_tokens; }

const LexerError *lexer_get_errors(const Lexer *lexer, size_t *count) {
  if (count) {
    *count = lexer->error_count;
  }
  return lexer->errors;
}
