#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "span.h"
#include "token.h"

typedef struct Lexer Lexer;

Lexer *lexer_create(const char *);
void lexer_free(Lexer *);
int lexer_next_token(Lexer *, Token **);
int lexer_num_tokens(Lexer *);

static int grow_tok_stream(Lexer *);
static void reset_tok_stream(Lexer *);

#endif
