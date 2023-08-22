//
// Created by emma on 8/16/23.
//

#ifndef ECC_PARSER_H
#define ECC_PARSER_H

#include "tokenizer.h"

typedef enum {
  ND_ASSIGN, // =
  ND_EQ,     // ==
  ND_NEQ,    // !=
  ND_LT,     // <
  ND_LEQ,    // <=
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_RETURN, // return
  ND_IF,     // if
  ND_WHILE,  // while
  ND_FOR,    // for
  ND_LVAR,   // Local variable
  ND_NUM     // Integer
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node* lhs; //left-hand side branch
  Node* rhs; //right-hand side branch
  Node* condition; //third branch (used for branching conditions)
  Node* update; //fourth branch (used in for loops)
  int val;
  int offset;
};

typedef struct LVar LVar;

struct LVar {
  LVar* next;
  char* name;
  int len;
  int offset;
};

extern Node* code[100];

Node* new_node(NodeKind, Node*, Node*);
Node* new_node_num(int);

LVar* find_lvar(Token* tok);

void program();
Node* stmt();
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* unary();
Node* primary();

#endif //ECC_PARSER_H