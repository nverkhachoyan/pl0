#include "span.h"

void span_init(Span *span, const char *filename) {
  span->line = 1;
  span->col = 1;
  span->start = 0;
  span->end = 0;
  span->filename = filename;
}

void span_advance(Span *span, char ch) {
  if (ch == '\n') {
    span->line++;
    span->col = 1;
  } else {
    span->col++;
  }
  span->end++;
}

void span_mark_start(Span *span) { span->start = span->end; }

void span_mark_end(Span *span) { span->end = span->end; }
