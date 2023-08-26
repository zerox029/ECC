//
// Created by emma on 23/08/23.
//

#include "parser.h"
#include "stmt.h"
#include "symbolTable.h"
#include "../lib/vector.h"

// stmt = "{" stmt; "}"
//        | "return" expr ";"
//        | "if" "(" equality ")" stmt ("else" stmt)?
//        | "while" "(" equality ")" stmt
//        | "for" "(" assign? ";" equality? ";" equality? ")" stmt
//        | expr ";"
Node* stmt() {
  return code_block()
    ?: return_statement()
    ?: if_else_statement()
    ?: while_loop()
    ?: for_loop()
    ?: singular_expression();
}

// "{" stmt; "}"
Node* code_block() {
  if(consume(TK_OP_BLK)) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->branches = vector_create();

    while(!is_next_token_of_type(TK_CL_BLK)) {
      vector_add(&node->branches, stmt());
    }

    expect(TK_CL_BLK);

    return node;
  }

  return NULL;
}

// "return" expr ";"
Node* return_statement() {
  if(consume(TK_RETURN)) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->branches = vector_create();

    vector_add(&node->branches, expr());

    expect(TK_SMCOLON);

    return node;
  }

  return NULL;
}

// "if" "(" equality ")" stmt ("else" stmt)?
Node* if_else_statement() {
  if(consume(TK_IF)) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    node->branches = vector_create();

    // Condition
    expect(TK_OP_PAR);
    vector_add(&node->branches, equality());
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

// "while" "(" equality ")" stmt
Node* while_loop() {
  if(consume(TK_WHILE)) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    node->branches = vector_create();

    // Condition
    expect(TK_OP_PAR);
    vector_add(&node->branches, equality());
    expect(TK_CL_PAR);

    // Consequent
    vector_add(&node->branches, stmt());

    return node;
  }

  return NULL;
}

// "for" "(" assign? ";" equality? ";" equality? ")" stmt
Node* for_loop() {
  if(consume(TK_FOR)) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    node->branches = vector_create();

    expect(TK_OP_PAR);

    // Assignment
    if(!is_next_token_of_type(TK_SMCOLON)) {
      vector_add(&node->branches, expr());
    }
    expect(TK_SMCOLON);

    // Condition
    if(!is_next_token_of_type(TK_SMCOLON)) {
      vector_add(&node->branches, expr());
    }
    expect(TK_SMCOLON);

    // Increment/Decrement
    if(!is_next_token_of_type(TK_OP_PAR)) {
      vector_add(&node->branches, expr());
    }

    expect(TK_CL_PAR);

    vector_add(&node->branches, stmt());

    return node;
  }

  return NULL;
}

Node* singular_expression() {
  Node* node = expr();

  expect(TK_SMCOLON);

  return node;
}