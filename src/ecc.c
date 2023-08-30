#include <stdio.h>
#include "parser/parser.h"
#include "tokenizer.h"
#include "generator.h"
#include "lib/vector.h"

void alloc4(int** starting_point, int a, int b, int c, int d) {
  *starting_point = malloc(4 * sizeof(int));
  (*starting_point)[0] = a;
  (*starting_point)[1] = b;
  (*starting_point)[2] = c;
  (*starting_point)[3] = d;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Argument count invalid\n");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);
  program();

  generate_file_prologue();

  for (int i = 0; i < vector_size(code); i++) {
    generate(code[i]);
  }

  return 0;
}