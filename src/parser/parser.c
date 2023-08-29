//
// Created by emma on 8/16/23.
//

/* Current production rules:
program     = (func | stmt)*
func        = int label "(" (("int" label, ",")* "int" label)? ")" "{" stmt* "}"
stmt        = expr ";"
              | "{" stmt; "}"
              | "if" "(" equality ")" stmt ("else" stmt)?
              | "while" "(" equality ")" stmt
              | "for" "(" expr? ";" expr? ";" expr? ")" stmt
              | "return" equality ";"
expr        = assign | equality
assign      = ("int" "*"*)? label ("=" equality)?
equality    = relational ("==" relational | "!=" relational)*
relational  = add ("<" add | "<=" add | ">" add | ">=" add)*
add         = mul ("+" mul | "-" mul)*
mul         = unary ("*" unary | "/" unary)*
unary       =  ("+" | "-" | "++" | "--" | "*" | "&")? primary
primary     = num
              | label ("(" ((equality ",")* equality)? ")")?
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

Node* new_node_var(Token* tok, bool is_declaration, int pointer_depth) {
  Node* node = calloc(1, sizeof(Node));

  node->kind = ND_LVAR;

  LVar* lvar = find_lvar(tok->str, current_function_name);
  if (lvar) { // If variable was already declared, reuse its offset
    node->offset = lvar->offset;
    node->variable_name = lvar->name;
  } else { // If it's a new variable, create it set its offset 8 bytes after the last variable
    if(is_declaration) {
      lvar = add_symbol_to_table(tok, current_function_name, pointer_depth);
      node->offset = lvar->offset;
      node->variable_name = lvar->name;
    } else {
      error("Use of undefined identifier \"%s\"", tok->str);
    }
  }

  // Handling pointer values
  for(int i = 0; i < pointer_depth; i++) {
    Node* pointer_node = calloc(1, sizeof(Node));
    pointer_node->kind = ND_DEREF;
    pointer_node->branches = vector_create();
    vector_add(&pointer_node->branches, node);
    node = pointer_node;
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

      vector_add(&node->branches, new_node_var(parameter, true, 0));
    }
    while(consume(TK_COMMA)) { // Continue setting parameters as long as there are commas
      expect(TK_INT);
      Token* parameter = consume(TK_LABEL);

      vector_add(&node->branches, new_node_var(parameter, true, 0));
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

// assign = ("int" "*"*)? label ("=" equality)?
Node* assign() {
  int pointer_depth = 0;
  // Declaration
  if(consume(TK_INT)) {
    while(consume(TK_STAR)) {
      pointer_depth++;
    }

    Token* tok = consume(TK_LABEL);
    Node* label = new_node_var(tok, true, pointer_depth);

    // Declaration and assignment
    if(consume(TK_ASSIGN)) {
      return new_node(ND_ASSIGN, label, equality());
    }
    // Declaration only (No need to create a node, just add the variable to the symbol table)
    return new_node(ND_VAR_DEC, new_node_var(tok, true, pointer_depth), NULL);
  }

  // No declaration
  while(consume(TK_STAR)) {
    pointer_depth++;
  }
  if(is_next_token_of_type(TK_LABEL) && is_nth_token_of_type(TK_ASSIGN, 1)) {
    Token* tok = consume(TK_LABEL);

    Node* label = new_node_var(tok, false, pointer_depth);

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

// Returns how large of a data a pointer points to, that value must then be substituted for the operand when doing
// pointer arithmetic. A value of 1 reprensents no pointer.
static int get_pointer_jump_size(Node* node) {
  if(node->kind == ND_ADDR) {
    // TODO: Handle this case
    return 1;
  }
  else if(node->kind == ND_LVAR) {
    LVar* variable = find_lvar(node->variable_name, current_function_name);

    if(variable->ty->data_type != PTR)
    {
      return 1;
    }

    return (int) variable->ty->ptr_to->data_type;
  }

  return false;
}

// If pointer arithmetic, multiply the result by the size of the datatype pointed to by the pointer
static Node* handle_pointer_arithmetic(Node* node) {
  if(node->kind == ND_ADD || node->kind == ND_SUB) {
    int left_operand_jump_size = get_pointer_jump_size(node->branches[0]);
    if(left_operand_jump_size > 1) {
      Node* mul_node = new_node(ND_MUL, new_node_num(left_operand_jump_size), node->branches[1]);
      node->branches[1] = mul_node;

      return node;
    }

    int right_operand_jump_size = get_pointer_jump_size(node->branches[0]);
    if(right_operand_jump_size > 1) {
      Node* mul_node = new_node(ND_MUL, new_node_num(right_operand_jump_size), node->branches[0]);
      node->branches[0] = mul_node;

      return node;
    }
  }

  return node;
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
      return handle_pointer_arithmetic(node);
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

// unary = ("+" | "-" | "++" | "--" | "&")? primary
//         | "*" unary
Node* unary() {
  if (consume(TK_PLUS)) {
    return primary();
  }
  if (consume(TK_MINUS)) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }
  if(consume(TK_INCREMENT)) {
    return new_node(ND_ADD, new_node_num(1), primary());
  }
  if(consume(TK_DECREMENT)) {
    return new_node(ND_SUB, primary(), new_node_num(1));
  }
  if(consume(TK_STAR)) {
    return new_node(ND_DEREF, unary(), NULL);
  }
  if(consume(TK_AMPERSAND)) {
    return new_node(ND_ADDR, primary(), NULL);
  }

  return primary();
}