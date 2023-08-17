#include <stdio.h>
#include "parser.h"
#include "tokenizer.h"
#include "generator.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Argument count invalid\n");
        return 1;
    }

    user_input = argv[1];
    token = tokenize(user_input);
    Node *node = expr();

    generate_intro();
    generate(node);
    generate_return();

    return 0;
}