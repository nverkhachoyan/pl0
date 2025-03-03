#include "token.h"
#include <stdio.h>
#include <string.h>

typedef struct {
  const char *keyword;
  TokenType type;
} KeywordMap;

static const KeywordMap KEYWORDS[] = {
    {"const", TOKEN_CONST},
    {"var", TOKEN_VAR},
    {"procedure", TOKEN_PROCEDURE},
    {"call", TOKEN_CALL},
    {"begin", TOKEN_BEGIN},
    {"end", TOKEN_END},
    {"if", TOKEN_IF},
    {"then", TOKEN_THEN},
    {"while", TOKEN_WHILE},
    {"do", TOKEN_DO},
    {"odd", TOKEN_ODD},
    {NULL, 0} // sentinel
};

Token *token_create(TokenType token_type, const char *str_rep,
                    const Span *span) {
  if (!str_rep || !span) {
    return NULL;
  }

  Token *tok = calloc(1, sizeof(Token));
  if (!tok)
    return NULL;

  tok->token_type = token_type;
  if (str_rep) {
    tok->str_rep = strdup(str_rep);
    if (!tok->str_rep) {
      free(tok);
      return NULL;
    }
  }

  tok->span = *span;
  return tok;
}

TokenType keyword_get_type(const char *str) {
  for (int i = 0; KEYWORDS[i].keyword != NULL; i++) {
    if (strcmp(str, KEYWORDS[i].keyword) == 0)
      return KEYWORDS[i].type;
  }
  return TOKEN_IDENT;
}

void token_free(Token *tok) {
  if (!tok)
    return;

  if (tok->str_rep) {
    free(tok->str_rep);
  }

  free(tok);
}

Token *lexer_token_copy(Token *tok) {
  if (!tok) {
    return NULL;
  }
  Token *copy = malloc(sizeof(Token));
  if (!copy) {
    perror("Error allocating memory for token copy");
    return NULL;
  }
  copy->token_type = tok->token_type;
  copy->str_rep = strdup(tok->str_rep);
  if (!copy->str_rep) {
    free(copy);
    return NULL;
  }
  return copy;
}