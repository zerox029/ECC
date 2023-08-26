//
// Created by emma on 23/08/23.
//

#include <string.h>
#include "../lib/vector.h"
#include "parser.h"
#include "primary.h"
#include "symbolTable.h"

// primary = num
//           | label
//           | label ("(" ((primary ",")* primary)? ")")?
//           | "(" expr ")"
Node* primary() {
  return parentheses() ?: label_node() ?: new_node_num(expect_number());
}

Node* parentheses() {
  if (consume(TK_OP_PAR)) {
    Node* node = expr();
    expect(TK_CL_PAR);

    return node;
  }

  return NULL;
}

Node* label_node() {
  Token* tok = consume(TK_LABEL);
  if (tok) {
    return function(&tok) ?: new_node_var(tok);
  }
}

Node* function(Token* tok) {
  // Function calls
  if(is_next_token_of_type(TK_OP_PAR)) {
    Node* node = calloc(1, sizeof(Node));

    node->kind = ND_FN_CALL;

    node->name = calloc(1, tok->len + 1);
    strncpy(node->name, tok->str, (size_t)tok->len);
    node->name[tok->len + 1] = '\0';

    // Parameters
    expect(TK_OP_PAR);

    if(!is_next_token_of_type(TK_CL_PAR)) {
      node->branches = vector_create();
      vector_add(&node->branches, primary());
    }
    while(consume(TK_COMMA)) {
      vector_add(&node->branches, primary());
    }

    expect(TK_CL_PAR);

    return node;
  }

  return NULL;
}