#include <stdio.h>
#include "parser.h"
#include "tokenizer.h"
#include "generator.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Argument count invalid\n");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);
  program();

  generate_prologue();

  for (int i = 0; code[i]; i++) {
    generate(code[i]);

    printf("    pop rax\n");
  }

  generate_epilogue();

  return 0;
}