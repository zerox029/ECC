//
// Created by emma on 8/16/23.
//

#ifndef ECC2_TOKENIZER_H
#define ECC2_TOKENIZER_H

#include <stdbool.h>

typedef struct Token Token;

typedef enum {
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
} TokenKind;

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
};

Token *token;

bool consume(char);
void expect(char);
int expect_number();
bool at_eof();
Token *new_token(TokenKind, Token*, char*);
Token *tokenize(char*);

#endif //ECC2_TOKENIZER_H