//
// Created by emma on 8/16/23.
//

/* Current production rules:
 * TODO: Update the parser to reflect the changes
program     = (func | stmt)*
func        = int label "(" (("int" label, ",")* "int" label)? ")" "{" stmt* "}"
stmt        = expr ";"
              | "{" stmt; "}"
              | "if" "(" equality ")" stmt ("else" stmt)?
              | "while" "(" equality ")" stmt
              | "for" "(" expr? ";" expr? ";" expr? ")" stmt
              | "return" expr ";"
expr        = assign | equality
assign      = ("int")? label "=" equality | "int" label
equality    = relational ("==" relational | "!=" relational)*
relational  = add ("<" add | "<=" add | ">" add | ">=" add)*
add         = mul ("+" mul | "-" mul)*
mul         = unary ("*" unary | "/" unary)*
unary       =  ("+" | "-")? primary
               | ("++" | "--")? unary
               | "*" unary
               | "&" unary
primary     = num
              | label ("(" ((primary ",")* primary)? ")")?
              | "(" expr ")"
*/

#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "primary.h"
#include "symbolTable.h"
#include "../utils.h"
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

Node* new_node_var(Token* tok) {
  Node* node = calloc(1, sizeof(Node));

  node->kind = ND_LVAR;

  LVar* lvar = find_lvar(tok, current_function_name);
  if (lvar) { // If variable was already declared, reuse its offset
    node->offset = lvar->offset;
  } else { // If it's a new variable, create it set its offset 8 bytes after the last variable
    lvar = add_symbol_to_table(tok, current_function_name);
    node->offset = lvar->offset;
  }

  return node;
}

// program = (func | stmt)*
void program() {
  code = vector_create();
  while (!at_eof()) {
    vector_add(&code, func() ?: stmt());
  }
}

// func = int label "(" (("int" label, ",")* "int" label)? ")" "{" stmt* "}"
Node* func() {
  if(consume(TK_INT)) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_FN_DEC;

    Token* tok = consume(TK_LABEL);

    // Function name
    node->name = tok->str;
    current_function_name = node->name;

    // Function parameters
    expect(TK_OP_PAR);

    if(!is_next_token_of_type(TK_CL_PAR)) { // First parameter
      expect(TK_INT);
      Token* parameter = consume(TK_LABEL);
      node->branches = vector_create();

      vector_add(&node->branches, new_node_var(parameter));
    }
    while(consume(TK_COMMA)) { // Continue setting parameters as long as there are commas
      expect(TK_INT);
      Token* parameter = consume(TK_LABEL);

      vector_add(&node->branches, new_node_var(parameter));
    }

    expect(TK_CL_PAR);

    // Function body
    if(node->branches == NULL) { // Creating the branches vector if no parameters are defined
      node->branches = vector_create();
    }
    vector_add(&node->branches, stmt());

    return node;
  }
}

// expr = assign | equality
Node* expr() {
  return assign() ?: equality();
}

// assign = ("int")? label "=" equality | "int" label
Node* assign() {
  if(consume(TK_INT)) {
    Token* tok = consume(TK_LABEL);
    Node* label = new_node_var(tok);

    // Declaration and assignment
    if(consume(TK_ASSIGN)) {
      return new_node(ND_ASSIGN, label, equality());
    }
    // Declaration only (No need to create a node, just add the variable to the symbol table)
    return new_node(ND_VAR_DEC, new_node_var(tok), NULL);
  }

  if(is_next_token_of_type(TK_LABEL) && is_nth_token_of_type(TK_ASSIGN, 1)) {
    Token* tok = consume(TK_LABEL);

    // Verify that the variable was initialized beforehand
    if(!find_lvar(tok, current_function_name)) {
      error("Use of undefined identifier \"%s\"", tok->str);
    }

    Node* label = new_node_var(tok);

    expect(TK_ASSIGN);

    return new_node(ND_ASSIGN, label, equality());
  }

  return NULL;
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