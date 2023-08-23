#include <stdio.h>
#include "parser/parser.h"
#include "tokenizer.h"
#include "generator.h"
#include "lib/vector.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Argument count invalid\n");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);
  program();

  generate_prologue();

  for (int i = 0; i < vector_size(code); i++) {
    generate(code[i]);
  }

  return 0;
}