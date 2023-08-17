//
// Created by emma on 8/16/23.
//

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "generator.h"
#include "parser.h"

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void generate_intro() {
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
}

void generate_return() {
    printf("    pop rax\n");
    printf("    ret\n");
}

void generate(Node *node) {
    if(node->kind == ND_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    generate(node->lhs);
    generate(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->kind) {
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
    }

    printf("    push rax\n");
}