// span.h
#ifndef SPAN_H
#define SPAN_H

typedef struct Span {
  int line;
  int col;
  int start;
  int end;
  const char *filename;
} Span;

void span_init(Span *span, const char *filename);
void span_advance(Span *span, char ch);
void span_mark_start(Span *span);
void span_mark_end(Span *span);

#endif