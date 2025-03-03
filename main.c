#include "lexer/error.h"
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

  size_t lexer_err_count = 0;
  const LexerError *lexer_errors = lexer_get_errors(lexer, &lexer_err_count);
  if (lexer_err_count > 0) {
    printf("\n");
    for (size_t i = 0; i < lexer_err_count; i++) {
      char msg[255];
      lexer_format_error(&lexer_errors[i], msg, 255);
      printf("Lexer Error: %s\n", msg);
    }
  }

  if (tok) {
    token_free(tok);
  }

  lexer_free(lexer);
  return exit_code;
}
