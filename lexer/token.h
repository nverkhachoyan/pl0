#ifndef TOKEN_H
#define TOKEN_H

#include "span.h"
#include <stdlib.h>

typedef enum {
  TOKEN_CONST,
  TOKEN_VAR,
  TOKEN_PROCEDURE,
  TOKEN_IDENT,
  TOKEN_ASSIGN,
  TOKEN_NUMBER,
  TOKEN_COMMA,
  TOKEN_SEMICOLON,
  TOKEN_L_PAREN,
  TOKEN_R_PAREN,
  TOKEN_CALL,
  TOKEN_BEGIN,
  TOKEN_END,
  TOKEN_IF,
  TOKEN_THEN,
  TOKEN_WHILE,
  TOKEN_DO,
  TOKEN_ODD,
  TOKEN_EQUAL,
  TOKEN_NOT_EQUAL,
  TOKEN_LT,
  TOKEN_GT,
  TOKEN_LTE,
  TOKEN_GTE,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_TIMES,
  TOKEN_DIVIDE,
  TOKEN_PERIOD
} TokenType;

typedef struct {
  TokenType token_type;
  char *str_rep;
  Span span;
} Token;

Token *token_create(TokenType token_type, const char *str_rep, const Span *);
void token_free(Token *tok);
int which_keyword(char *str);
TokenType keyword_get_type(const char *str);

#endif
