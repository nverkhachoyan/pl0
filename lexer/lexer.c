#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Lexer {
  char ch;
  int pos;
  char tok_stream[1024];
  int stream_size;
  long file_size;
  char *input;
  int num_tokens;
};

static void advance(Lexer *lexer) { lexer->ch = lexer->input[lexer->pos++]; }

static char peek(Lexer *lexer) {
  if (lexer->pos >= lexer->file_size)
    return '\0';
  return lexer->input[lexer->pos];
}

static void skip_whitespace_and_comments(Lexer *lexer) {
  while (isspace(lexer->ch)) {
    advance(lexer);
  }
}

static Token *consume_ident(Lexer *lexer) {
  char *str = malloc(lexer->stream_size + 1);
  if (!str)
    return NULL;

  memcpy(str, lexer->tok_stream, lexer->stream_size);
  str[lexer->stream_size] = '\0';

  Token *tok = calloc(1, sizeof(Token));
  if (!tok) {
    free(str);
    return NULL;
  }

  tok->token_type = TOKEN_IDENT;
  tok->str_rep = str;

  memset(lexer->tok_stream, 0, sizeof(lexer->tok_stream));
  lexer->stream_size = 0;

  return tok;
}

static int which_keyword(char *str) {
  if (strcmp(str, "const") == 0)
    return TOKEN_CONST;
  if (strcmp(str, "var") == 0)
    return TOKEN_VAR;
  if (strcmp(str, "procedure") == 0)
    return TOKEN_PROCEDURE;
  if (strcmp(str, "call") == 0)
    return TOKEN_CALL;
  if (strcmp(str, "begin") == 0)
    return TOKEN_BEGIN;
  if (strcmp(str, "end") == 0)
    return TOKEN_END;
  if (strcmp(str, "if") == 0)
    return TOKEN_IF;
  if (strcmp(str, "then") == 0)
    return TOKEN_THEN;
  if (strcmp(str, "while") == 0)
    return TOKEN_WHILE;
  if (strcmp(str, "do") == 0)
    return TOKEN_DO;
  if (strcmp(str, "odd") == 0)
    return TOKEN_ODD;

  return -1;
}

static Token *consume_keyword(Lexer *lexer) {
  char *str = malloc((lexer->stream_size + 1) * sizeof(char));
  if (str == NULL) {
    perror("Failed to allocate memory for str_rep for keyword");
    return NULL;
  }

  memcpy(str, lexer->tok_stream, lexer->stream_size);
  str[lexer->stream_size] = '\0';
  int which = which_keyword(str);
  Token *tok = calloc(1, sizeof(Token));
  if (!tok) {
    perror("Failed to allocate memory for token");
    free(str);
    return NULL;
  }

  tok->token_type = which;
  tok->str_rep = str;

  memset(lexer->tok_stream, 0, sizeof(lexer->tok_stream));
  lexer->stream_size = 0;

  return tok;
}

static Token *create_token(int token_type, const char *str_rep) {
  Token *tok = calloc(1, sizeof(Token));
  if (!tok)
    return NULL;

  tok->token_type = token_type;
  tok->str_rep = strdup(str_rep);
  if (!tok->str_rep) {
    free(tok);
    return NULL;
  }
  return tok;
}

static int _next_token(Lexer *lexer, Token **token_out) {
  if (lexer->ch == '\0')
    return 1;

  skip_whitespace_and_comments(lexer);

  if (lexer->ch == ':' && peek(lexer) == '=') {
    advance(lexer);
    advance(lexer);
    *token_out = create_token(TOKEN_ASSIGN, ":=");
  } else if (lexer->ch == '<' && peek(lexer) == '=') {
    advance(lexer);
    advance(lexer);
    *token_out = create_token(TOKEN_LTE, "<=");
  } else if (lexer->ch == '>' && peek(lexer) == '=') {
    advance(lexer);
    advance(lexer);
    *token_out = create_token(TOKEN_GTE, ">=");
  } else {
    switch (lexer->ch) {
    case ';':
      *token_out = create_token(TOKEN_SEMICOLON, ";");
      advance(lexer);
      break;
    case '-':
      *token_out = create_token(TOKEN_MINUS, "-");
      advance(lexer);
      break;
    case '+':
      *token_out = create_token(TOKEN_PLUS, "+");
      advance(lexer);
      break;
    case '=':
      *token_out = create_token(TOKEN_EQUAL, "=");
      advance(lexer);
      break;
    case ',':
      *token_out = create_token(TOKEN_COMMA, ",");
      advance(lexer);
      break;
    case '.':
      *token_out = create_token(TOKEN_PERIOD, ".");
      advance(lexer);
      break;
    case '*':
      *token_out = create_token(TOKEN_TIMES, "*");
      advance(lexer);
      break;
    case '/':
      *token_out = create_token(TOKEN_DIVIDE, "/");
      advance(lexer);
      break;
    case '#':
      *token_out = create_token(TOKEN_NOT_EQUAL, "#");
      advance(lexer);
      break;
    case '<':
      *token_out = create_token(TOKEN_LT, "<");
      advance(lexer);
      break;
    case '>':
      *token_out = create_token(TOKEN_GT, ">");
      advance(lexer);
      break;
    default:
      if (isdigit(lexer->ch)) {
        char number[256] = {0};
        int i = 0;

        while (isdigit(lexer->ch) && i < 255) {
          number[i++] = lexer->ch;
          advance(lexer);
        }

        number[i] = '\0';

        *token_out = create_token(TOKEN_NUMBER, number);
        if (*token_out) {
          (*token_out)->value = atoi(number);
        }

      } else if (isalpha(lexer->ch)) {
        lexer->stream_size = 0;

        while ((isalpha(lexer->ch) || isdigit(lexer->ch)) &&
               lexer->stream_size < sizeof(lexer->tok_stream) - 1) {
          lexer->tok_stream[lexer->stream_size++] = lexer->ch;
          advance(lexer);
        }

        if (lexer->stream_size > 0) {
          char temp[1024];
          memcpy(temp, lexer->tok_stream, lexer->stream_size);
          temp[lexer->stream_size] = '\0';

          if (which_keyword(temp) != -1) {
            *token_out = consume_keyword(lexer);
            return 0;
          } else {
            *token_out = consume_ident(lexer);
            return 0;
          }
        } else {
          advance(lexer);
          return 2;
        }
      } else {
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

int lexer_next_token(Lexer *lexer, Token **token_out) {
  if (!lexer || !token_out)
    return -1;

  *token_out = NULL;

  int result;
  do {
    result = _next_token(lexer, token_out);

    if (result < 0 || result == 1)
      return result;

    if (result == 0)
      return 0;

    if (*token_out) {
      lexer_token_free(*token_out);
      *token_out = NULL;
    }

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

  FILE *fptr = fopen(filename, "rb");
  if (!fptr) {
    perror("Error opening file");
    free(lexer);
    return NULL;
  }

  fseek(fptr, 0, SEEK_END);
  lexer->file_size = ftell(fptr);
  rewind(fptr);

  lexer->input = (char *)malloc(lexer->file_size + 1);
  if (lexer->input == NULL) {
    perror("Error allocating memory for input");
    fclose(fptr);
    free(lexer);
    return NULL;
  }

  size_t bytes_read = fread(lexer->input, 1, lexer->file_size, fptr);
  if (bytes_read != (size_t)lexer->file_size) {
    perror("Error reading file");
    free(lexer->input);
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
  if (lexer == NULL) {
    perror("Cannot free a NULL pointer");
    return;
  }

  free(lexer->input);
  free(lexer);
}

Token *lexer_token_copy(Token *tok) {
  if (!tok) {
    perror("Cannot copy from a NULL pointer");
    return NULL;
  }
  Token *copy = malloc(sizeof(Token));
  if (!copy) {
    perror("Error allocating memory for token copy");
    return NULL;
  }
  copy->token_type = tok->token_type;
  copy->value = tok->value;
  copy->str_rep = strdup(tok->str_rep);
  if (!copy->str_rep) {
    free(copy);
    return NULL;
  }
  return copy;
}

void lexer_token_free(Token *tok) {
  if (!tok) {
    perror("Cannot free a NULL pointer");
    return;
  }

  if (tok->str_rep) {
    free(tok->str_rep);
  }
  free(tok);
}

int lexer_num_tokens(Lexer *lexer) { return lexer->num_tokens; }
