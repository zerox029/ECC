//
// Created by emma on 8/16/23.
//

/* Current production rules:
program    = stmt*
stmt       = expr ";"
             | label ("(" ((primary ",")* primary)? ")")? "{" stmt* "}"
             | "{" stmt* "}"
             | "if" "(" expr ")" stmt ("else" stmt)?
             | "while" "(" expr ")" stmt
             | "for" "(" expr? ";" expr? ";" expr? ")" stmt
             | "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
             | ("++" | "--")? unary
             | "*" unary
             | "&" unary
primary    = num
             | label ("(" ((primary ",")* primary)? ")")?
             | "(" expr ")"
*/

#include <stdlib.h>
#include "parser.h"
#include "../lib/vector.h"

Node** code;
char* current_function_name = "GLOBAL";

// Creates a new, non-numerical, node
// TODO: Update the parameters to a VAlist
Node* new_node(NodeKind kind, Node* lhs, Node* rhs) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = kind;

  node->branches = vector_create();
  vector_add(&node->branches, lhs);
  vector_add(&node->branches, rhs);

  return node;
}

// Creates a new numerical leaf node
Node* new_node_num(int val) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;

  return node;
}

// program = stmt*
void program() {
  code = vector_create();
  while (!at_eof()) {
    vector_add(&code, stmt());
  }
}

// expr = assign
Node* expr() {
  return assign();
}

// assign = equality ("=" assign)?
Node* assign() {
  Node* node = equality();

  if (consume(TK_ASSIGN)) {
    node = new_node(ND_ASSIGN, node, assign());
  }

  return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node* equality() {
  Node* node = relational();

  for (;;) {
    if (consume(TK_EQ)) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume(TK_NE)) {
      node = new_node(ND_NEQ, node, relational());
    } else {
      return node;
    }
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node* relational() {
  Node* node = add();

  for (;;) {
    if (consume(TK_LT)) {
      node = new_node(ND_LT, node, add());
    } else if (consume(TK_LTE)) {
      node = new_node(ND_LEQ, node, add());
    } else if (consume(TK_GT)) {
      node = new_node(ND_LT, add(), node);
    } else if (consume(TK_GTE)) {
      node = new_node(ND_LEQ, add(), node);
    } else {
      return node;
    }
  }
}

// add = mul ("+" mul | "-" mul)*
Node* add() {
  Node* node = mul();

  for (;;) {
    if (consume(TK_PLUS)) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume(TK_MINUS)) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

// mul = unary ("*" unary | "/" unary)*
Node* mul() {
  Node* node = unary();

  for (;;) {
    if (consume(TK_STAR)) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume(TK_SLASH)) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

// unary = ("+" | "-")? primary
//         | ("++" | "--")? unary
//         | "*" unary
//         | "&" unary
Node* unary() {
  if (consume(TK_PLUS)) {
    return primary();
  }
  if (consume(TK_MINUS)) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }
  if(consume(TK_INCREMENT)) {
    return new_node(ND_ADD, new_node_num(1), unary());
  }
  if(consume(TK_DECREMENT)) {
    return new_node(ND_SUB, unary(), new_node_num(1));
  }
  if(consume(TK_STAR)) {
    return new_node(ND_DEREF, unary(), NULL);
  }
  if(consume(TK_AMPERSAND)) {
    return new_node(ND_ADDR, unary(), NULL);
  }

  return primary();
}