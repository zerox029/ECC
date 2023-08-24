//
// Created by emma on 23/08/23.
//

#include <string.h>
#include "../lib/vector.h"
#include "primary.h"

LVar* locals;

static LVar* find_lvar(Token* tok) {
  for (LVar* var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }

  return NULL;
}

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
    return function(&tok) ?: variable(&tok);
  }
}

Node* function(Token** tok) {
  // Function calls
  if(isNextTokenOfType(TK_OP_PAR)) {
    Node* node = calloc(1, sizeof(Node));

    node->kind = ND_FN_CALL;

    node->name = malloc((*tok)->len + 1);
    strncpy(node->name, (*tok)->str, (size_t)(*tok)->len);
    node->name[(*tok)->len + 1] = '\0';

    // Parameters
    expect(TK_OP_PAR);

    if(!isNextTokenOfType(TK_CL_PAR)) {
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

Node* variable(Token** tok) {
  Node* node = calloc(1, sizeof(Node));

  node->kind = ND_LVAR;

  LVar* lvar = find_lvar(*tok);
  if (lvar) { // If variable was already declared, reuse its offset
    node->offset = lvar->offset;
  } else { // If it's a new variable, create it set its offset 8 bytes after the last variable
    lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = (*tok)->str;
    lvar->len = (*tok)->len;

    if (locals) {
      lvar->offset = locals->offset + 8;
    } else {
      lvar->offset = 0;
    }

    node->offset = lvar->offset;
    locals = lvar;
  }

  return node;
}