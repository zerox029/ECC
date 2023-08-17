//
// Created by emma on 8/16/23.
//

#ifndef ECC_PARSER_H
#define ECC_PARSER_H

typedef enum {
    ND_EQ,  // ==
    ND_NEQ, // !=
    ND_LT,  // <
    ND_LEQ, // <=
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM  // Integer
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *new_node(NodeKind, Node*, Node*);
Node *new_node_num(int);

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

#endif //ECC_PARSER_H