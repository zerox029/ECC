//
// Created by emma on 8/16/23.
//

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "generator.h"
#include "parser.h"

char* user_input = "";

void error_at(char* loc, char* fmt, ...) {
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

void error(char* fmt, ...) {
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

void generate_comparison(char* operator_instruction) {
    printf("    cmp rax, rdi\n");
    printf("    %s al\n", operator_instruction);
    printf("    movzb rax, al\n");
}

void generate_division() {
    printf("    cqo\n");
    printf("    idiv rdi\n");
}

void generate(Node* node) {
    if(node->kind == ND_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    generate(node->lhs);
    generate(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->kind) {
        case ND_EQ:
            generate_comparison("sete");
            break;
        case ND_NEQ:
            generate_comparison("setne");
            break;
        case ND_LT:
            generate_comparison("setl");
            break;
        case ND_LEQ:
            generate_comparison("setle");
            break;
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
            generate_division();
            break;
    }

    printf("    push rax\n");
}