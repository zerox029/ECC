//
// Created by emma on 8/16/23.
//

#ifndef ECC_TOKENIZER_H
#define ECC_TOKENIZER_H

#include <stdbool.h>

typedef struct Token Token;

typedef enum {
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
} TokenKind;

struct Token {
    TokenKind kind;
    Token* next;
    int val;
    char* str;
    int len;
};

extern Token* token;

bool consume(char*);
void expect(char*);
int expect_number();
bool at_eof();
Token* new_token(TokenKind, Token*, char*, int);
Token* tokenize(char*);

#endif //ECC_TOKENIZER_H