#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "span.h"
#include "token.h"

typedef struct Lexer Lexer;

Lexer *lexer_create(const char *);
void lexer_free(Lexer *);
int lexer_next_token(Lexer *, Token **);
int lexer_num_tokens(Lexer *);
const LexerError *lexer_get_errors(const Lexer *, size_t *);

#endif
