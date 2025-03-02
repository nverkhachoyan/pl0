#ifndef LEXER_H
#define LEXER_H

#include <_string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Lexer Lexer;

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
  int value;
} Token;

Lexer *lexer_create(const char *);
void lexer_free(Lexer *);
int lexer_next_token(Lexer *, Token **);
int lexer_num_tokens(Lexer *);
Token *lexer_copy_tokens(Lexer *);
Token *lexer_token_copy(Token *);
void lexer_token_free(Token *);

#endif
