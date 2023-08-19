//
// Created by emma on 8/16/23.
//

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "generator.h"

Token* token;

// Returns true of the current token is the expected type and moves to the next token
bool consume(char* operator) {
    if(token->kind != TK_RESERVED ||
       strlen(operator) != token->len ||
       memcmp(token->str, operator, token->len) != 0)
    {
        return false;
    }

    token = token->next;
    return true;
}

Token* consume_label() {
    if(token->kind == TK_LABEL) {
        Token* returnToken = token;
        token = token->next;

        return returnToken;
    }
    else {
        return NULL;
    }
}

// Throws an error if the current token is not of the expected type and moves to the next token otherwise
void expect(char* operator) {
    if(token->kind != TK_RESERVED ||
       strlen(operator) != token->len ||
       memcmp(token->str, operator, token->len) != 0)
    {
        error_at(token->str, "Was not'%s'", operator);
    }

    token = token->next;
}

// Throws and error if the current token is not a number, returns the value and moves to the next token otherwise
int expect_number() {
    if(token->kind != TK_NUM)
    {
        error_at(token->str, "Was not a number");
    }

    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Token* new_token(TokenKind kind, Token* cur, char* str, int len) {
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;

    return tok;
}

bool startsWith(char* p, char* q) {
    return memcmp(p, q, strlen(q)) == 0;
}

// Creates a linked list from a string of characters
Token* tokenize(char* p) {
    Token head;
    head.next = NULL;
    Token* cur = &head;

    while(*p) {
        if (isspace(*p)) {
            p++;

            continue;
        }

        // Multi-character punctuator
        if(startsWith(p, "==") || startsWith(p, "!=") || startsWith(p, "<=") || startsWith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;

            continue;
        }

        // Punctuator
        if(strchr("+-*/=()<>;",*p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);

            continue;
        }

        // Label
        if('a' <= *p && *p <= 'z') {
            cur = new_token(TK_LABEL, cur, p++, 1);
            cur->len = 1;

            continue;
        }

        // Digit
        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p, 0);
            char* q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;

            continue;
        }

        error("Couldn't tokenize");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}