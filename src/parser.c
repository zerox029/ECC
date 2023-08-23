//
// Created by emma on 8/16/23.
//

/* Current production rules:
program    = stmt*
stmt       = expr ";"
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
primary    = num
             | label ("(" ((primary ",")* primary)? ")")?
             | "(" expr ")"
*/

#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "tokenizer.h"
#include "lib/vector.h"

Node* code[100];
LVar* locals;

// Creates a new, non-numerical, node
static Node* new_node(NodeKind kind, Node* lhs, Node* rhs) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = kind;

  node->branches = vector_create();
  vector_add(&node->branches, lhs);
  vector_add(&node->branches, rhs);

  return node;
}

// Creates a new numerical leaf node
static Node* new_node_num(int val) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;

  return node;
}

static LVar* find_lvar(Token* tok) {
  for (LVar* var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }

  return NULL;
}

void program() {
  int i = 0;
  while (!at_eof()) {
    code[i++] = stmt();
  }

  code[i] = NULL;
}

// stmt = expr ";"
//        | "{" stmt* "}"
//        | "if" "(" expr ")" stmt ("else" stmt)?
//        | "while" "(" expr ")" stmt
//        | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//        | "return" expr ";"
Node* stmt() {
  Node* node = calloc(1, sizeof(Node));
  node->branches = vector_create();

  if(consume(TK_RETURN)) {
    node->kind = ND_RETURN;
    vector_add(&node->branches, expr());
    expect(TK_SMCOLON);
  }
  else if(consume(TK_IF)) {
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
  }
  else if(consume(TK_WHILE)) {
    node->kind = ND_WHILE;

    // Condition
    expect(TK_OP_PAR);
    vector_add(&node->branches, expr());
    expect(TK_CL_PAR);

    // Consequent
    vector_add(&node->branches, stmt());
  }
  else if(consume(TK_FOR)) {
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
  }
  else if(consume(TK_OP_BLK)) {
    node->kind = ND_BLOCK;

    while(!isNextTokenOfType(TK_CL_BLK)) {
      vector_add(&node->branches, stmt());
    }

    expect(TK_CL_BLK);
  }
  else {
    node = expr();
    expect(TK_SMCOLON);
  }

  return node;
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
Node* unary() {
  if (consume(TK_PLUS)) {
    return primary();
  }
  if (consume(TK_MINUS)) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }

  return primary();
}

// primary = num
//           | label ("(" ((primary ",")* primary)? ")")?
//           | "(" expr ")"
Node* primary() {
  if (consume(TK_OP_PAR)) {
    Node* node = expr();
    expect(TK_CL_PAR);

    return node;
  }

  Token* tok = consume(TK_LABEL);
  if (tok) {
    Node* node = calloc(1, sizeof(Node));

    if(isNextTokenOfType(TK_OP_PAR)) { // Function calls
      node->kind = ND_FN_CALL;

      node->name = malloc(tok->len + 1);
      strncpy(node->name, tok->str, (size_t)tok->len);
      node->name[tok->len + 1] = '\0';

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
    } else { // Variables
      node->kind = ND_LVAR;

      LVar* lvar = find_lvar(tok);
      if (lvar) {
        node->offset = lvar->offset;
      } else {
        lvar = calloc(1, sizeof(LVar));
        lvar->next = locals;
        lvar->name = tok->str;
        lvar->len = tok->len;

        if (locals) {
          lvar->offset = locals->offset + 8;
        } else {
          lvar->offset = 0;
        }

        node->offset = lvar->offset;
        locals = lvar;
      }
    }

    return node;
  }

  return new_node_num(expect_number());
}