//
// Created by emma on 8/16/23.
//

#ifndef ECC_GENERATOR_H
#define ECC_GENERATOR_H

#include "parser.h"

extern char* user_input;

void error_at(char*, char*, ...);
void error(char*, ...);
void generate_intro();
void generate_return();
void generate(Node*);

#endif //ECC_GENERATOR_H