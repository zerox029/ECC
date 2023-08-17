//
// Created by emma on 8/16/23.
//

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "generator.h"

// Returns true of the current token is the expected type and moves to the next token
bool consume(char operator) {
    if(token->kind != TK_RESERVED || token->str[0] != operator)
    {
        return false;
    }

    token = token->next;
    return true;
}

// Throws an error if the current token is not of the expected type and moves to the next token otherwise
void expect(char operator) {
    if(token->kind != TK_RESERVED || token->str[0] != operator)
    {
        error_at(token->str, "Was not'%c'", operator);
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

Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;

    return tok;
}

// Creates a linked list from a string of characters
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p) {
        if (isspace(*p)) {
            p++;

            continue;
        }

        // Arithmetic operators
        if (*p == '+' || *p == '-' || *p == '*' || *p == '/') {
            cur = new_token(TK_RESERVED, cur, p++);

            continue;
        }

        // Brackets
        if (*p == '(' || *p == ')') {
            cur = new_token(TK_RESERVED, cur, p++);

            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);

            continue;
        }

        error("Couldn't tokenize");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}