//
// Created by emma on 8/16/23.
//

#ifndef ECC_PARSER_H
#define ECC_PARSER_H

#include "../tokenizer.h"

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
  ND_ADDR,   // Address (&)
  ND_DEREF,  // Dereferencing (*)
  ND_FN_CALL,// Function call
  ND_FN_DEC, // Function creation
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

extern Node** code;
extern char* current_function_name;

Node* new_node(NodeKind, Node*, Node*);
Node* new_node_num(int);

void program();
Node* func();
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