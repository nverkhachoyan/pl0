#include "lexer/lexer.h"
#include "lexer/token.h"
#include <errno.h>
#include <stdio.h>

int main() {
  Lexer *lexer = lexer_create("main.pl");
  if (!lexer) {
    fprintf(stderr, "Failed to create lexer: %s\n", strerror(errno));
    return 1;
  }

  Token *tok = NULL;
  int result;
  int exit_code = 0;

  while ((result = lexer_next_token(lexer, &tok)) == 0) {
    if (!tok) {
      fprintf(stderr, "Unexpected NULL token\n");
      exit_code = 1;
      break;
    }

    printf("%s ", tok->str_rep);
    token_free(tok);
    tok = NULL;
  }

  if (tok) {
    token_free(tok);
  }

  lexer_free(lexer);
  return exit_code;
}
