#include "lexer/lexer.h"
#include <stdio.h>

int main() {
  Lexer *lexer = lexer_create("main.pl");
  if (!lexer) {
    fprintf(stderr, "Failed to create lexer\n");
    return 1;
  }

  Token *tok = NULL;
  int result;

  while ((result = lexer_next_token(lexer, &tok)) == 0) {
    printf("%s ", tok->str_rep);

    lexer_token_free(tok);
    tok = NULL;
  }

  if (result < 0) {
    fprintf(stderr, "Error during lexing\n");
  }

  printf("\nNum tokens: %d\n", lexer_num_tokens(lexer));

  lexer_free(lexer);
  return 0;
}
