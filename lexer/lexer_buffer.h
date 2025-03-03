#ifndef LEXER_BUFFER_H
#define LEXER_BUFFER_H

#include <stddef.h>

typedef struct {
  char *data;
  int val;
  size_t cap;
  size_t len;
} LexerBuffer;

LexerBuffer *buffer_alloc(size_t);
void buffer_free(LexerBuffer *);
int buffer_append(LexerBuffer *, char);
void buffer_reset(LexerBuffer *);
char *buffer_to_str(const LexerBuffer *);

static int buffer_grow(LexerBuffer *buf);

#endif
