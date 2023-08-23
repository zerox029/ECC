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
  ND_FN_CALL,// Function call
  ND_LVAR,   // Local variable
  ND_NUM,    // Integer
  ND_BLOCK   // Statement blocks ({ ... })
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind; // Type of the node
  Node** branches; // Vector containing all branches
  int val; // Numerical value for variables
  int offset; // Relative position within the stack frame for variables
  char* name; // Function name
};

typedef struct LVar LVar;

struct LVar {
  LVar* next;
  char* name;
  int len;
  int offset;
};

extern Node* code[100];

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