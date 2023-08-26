//
// Created by emma on 23/08/23.
//

#ifndef ECC_SRC_PARSER_PRIMARY_H_
#define ECC_SRC_PARSER_PRIMARY_H_

#include "parser.h"

Node* parentheses();
Node* label_node();
Node* function(Token* tok);

#endif //ECC_SRC_PARSER_PRIMARY_H_
