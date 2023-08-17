//
// Created by emma on 8/16/23.
//

#ifndef ECC2_GENERATOR_H
#define ECC2_GENERATOR_H

#include "parser.h"

char *user_input;

void error_at(char*, char*, ...);
void error(char*, ...);
void generate_intro();
void generate_return();
void generate(Node *node);

#endif //ECC2_GENERATOR_H