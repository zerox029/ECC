//
// Created by emma on 23/08/23.
//

#include "parser.h"
#include "stmt.h"
#include "../lib/vector.h"

// stmt = expr ";"
//        | label ("(" ((primary ",")* primary)? ")")? "{" stmt* "}"
//        | "{" stmt* "}"
//        | "if" "(" expr ")" stmt ("else" stmt)?
//        | "while" "(" expr ")" stmt
//        | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//        | "return" expr ";"
Node* stmt() {
  return return_statement()
    ?: if_else_statement()
    ?: while_loop()
    ?: for_loop()
    ?: code_block()
    ?: singular_expression();
}

Node* return_statement() {
  if(consume(TK_RETURN)) {
    Node* node = calloc(1, sizeof(Node));
    node->branches = vector_create();

    node->kind = ND_RETURN;
    vector_add(&node->branches, expr());
    expect(TK_SMCOLON);

    return node;
  }

  return NULL;
}

Node* if_else_statement() {
  if(consume(TK_IF)) {
    Node* node = calloc(1, sizeof(Node));
    node->branches = vector_create();

    node->kind = ND_IF;
    node->branches = vector_create();

    // Condition
    expect(TK_OP_PAR);
    vector_add(&node->branches, expr());
    expect(TK_CL_PAR);

    // Consequent
    vector_add(&node->branches, stmt());

    // Alternative
    if(consume(TK_ELSE)) {
      vector_add(&node->branches, stmt());
    }

    return node;
  }

  return NULL;
}

Node* while_loop() {
  if(consume(TK_WHILE)) {
    Node* node = calloc(1, sizeof(Node));
    node->branches = vector_create();

    node->kind = ND_WHILE;

    // Condition
    expect(TK_OP_PAR);
    vector_add(&node->branches, expr());
    expect(TK_CL_PAR);

    // Consequent
    vector_add(&node->branches, stmt());

    return node;
  }

  return NULL;
}

Node* for_loop() {
  if(consume(TK_FOR)) {
    Node* node = calloc(1, sizeof(Node));
    node->branches = vector_create();

    node->kind = ND_FOR;

    expect(TK_OP_PAR);

    if(!isNextTokenOfType(TK_SMCOLON)) {
      vector_add(&node->branches, expr());
    }
    expect(TK_SMCOLON);

    if(!isNextTokenOfType(TK_SMCOLON)) {
      vector_add(&node->branches, expr());
    }
    expect(TK_SMCOLON);

    if(!isNextTokenOfType(TK_OP_PAR)) {
      vector_add(&node->branches, expr());
    }

    expect(TK_CL_PAR);

    vector_add(&node->branches, stmt());

    return node;
  }

  return NULL;
}

Node* code_block() {
  if(consume(TK_OP_BLK)) {
    Node* node = calloc(1, sizeof(Node));
    node->branches = vector_create();

    node->kind = ND_BLOCK;

    while(!isNextTokenOfType(TK_CL_BLK)) {
      vector_add(&node->branches, stmt());
    }

    expect(TK_CL_BLK);

    return node;
  }

  return NULL;
}

Node* singular_expression() {
  Node* node = calloc(1, sizeof(Node));
  node->branches = vector_create();

  node = expr();
  expect(TK_SMCOLON);

  return node;
}