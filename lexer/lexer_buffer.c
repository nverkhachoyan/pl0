#include "lexer_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int buffer_grow(LexerBuffer *buf) {
  if (!buf)
    return -1;

  buf->cap *= 2;
  char *new_data = realloc(buf->data, buf->cap);
  if (!new_data) {
    return -1;
  }

  buf->data = new_data;
  return 0;
}

LexerBuffer *buffer_alloc(size_t cap) {
  LexerBuffer *buf = malloc(sizeof(LexerBuffer));
  if (!buf)
    return NULL;

  buf->data = malloc(cap);
  if (!buf->data) {
    free(buf);
    return NULL;
  }

  buf->cap = cap;
  buf->len = 0;

  return buf;
}

void buffer_free(LexerBuffer *buf) {
  if (!buf)
    return;

  if (buf->data) {
    free(buf->data);
  }

  free(buf);
}

void buffer_reset(LexerBuffer *buf) {
  if (!buf)
    return;

  buf->len = 0;
}

int buffer_append(LexerBuffer *buf, char c) {
  if (buf->len >= buf->cap - 1) {
    if (buffer_grow(buf) != 0) {
      return -1;
    }
  }
  buf->data[buf->len++] = c;
  return 0;
}

char *buffer_to_str(const LexerBuffer *buf) {
  if (!buf || !buf->data) {
    return NULL;
  }

  char *str = malloc(buf->len + 1);
  if (!str) {
    return NULL;
  }

  memcpy(str, buf->data, buf->len);
  str[buf->len] = '\0';

  return str;
}
